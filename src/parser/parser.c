/**
 * @file parser.c
 * @author Mohamed Al-Hussein
 * @date 27 Jan 2022
 * @brief For parsing a stream of tokens into nodes for later evaluation.
 */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>

#include "parser/parser.h"
#include "parser/token-iterator.h"
#include "parser/lexer.h"

/* *****************************************************************************
 * PRIVATE DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * OBJECTS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Hides @c Parser members from client code.
 */
struct ParserPrivate {
        size_t n_tokens; /**< number of tokens parsed */
        Token **tokens; /**< parsed tokens */
        ssize_t n_stmts; /**< number of statements created */
        Statement **stmts; /**< statements created */
};

/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Parses a command into @p stmt command statement.
 * @param stmt @c Statement object to add command to
 * @param iter iterator to extract command tokens from
 * @return 0 on success, -1 on failure
 */
static int Parser_parseCmd(Statement *stmt, TokenIterator *iter)
{
        // parse words into statement command
        size_t buf_size = 1;

        while (iter->vptr->has_next(iter)) {
                Token tok;
                char **tmp;

                // check if any words left to take
                tok = iter->vptr->peek(iter, 0);
                if (!is_tok_word(tok)) {
                        break; // done
                }

                // resize buf if full
                if (stmt->cmd->argc >= buf_size) {
                        buf_size *= 2;
                        tmp = realloc(stmt->cmd->argv, buf_size * sizeof(char));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->cmd->argv = tmp;
                }

                // TODO: look into creating add methods to statement class
                // take word
                WordToken *word = (WordToken *) iter->vptr->next(iter);
                char *word_str = word->super.vptr->getValue((Token *) word);
                stmt->cmd->argv[stmt->cmd->argc++] = Parser_expandWord(word_str);
        }

        // resize array to fit exactly argc + 1 elements for later use with exec
        char **tmp = realloc(stmt->cmd->argv, (stmt->cmd->argc + 1) * sizeof(char));
        if (tmp == NULL) {
                return -1; // error
        }
        stmt->cmd->argv = tmp;

        // null terminate argv for exec
        stmt->cmd->argv[stmt->cmd->argc] = NULL;

        // TODO: move these to builtins module
        char *cmd = stmt->cmd->argv[0];
        if (strcmp("cd", cmd) == 0 ||
            strcmp("exit", cmd) == 0 ||
            strcmp("status", cmd) == 0) {
                stmt->flags |= FLAGS_BUILTIN;
        }

        return 0;
}

/**
 * @brief Parses an io redirection command into @p stmt command statement.
 * @param stmt @c Statement object to add io redirection command to
 * @param iter iterator to extract io redirection tokens from
 * @param type type of io redirection to store
 * @return 0 on success, -1 on failure
 */
static int Parser_parseIORedir(Statement *stmt, TokenIterator *iter, IORedirType type)
{
        // filename should be a word token
        Token tok = iter->vptr->peek(iter, 1);
        if (!is_tok_word(tok)) {
                return -1; // error
        }

        // skip past redirection operator
        (void) iter->vptr->next(iter);

        // take next word
        WordToken *word = (WordToken *) iter->vptr->next(iter);

        // switch to type stream
        char *word_str;
        char **tmp;
        switch (type) {
                case IO_REDIR_STDIN:
                        // take next word
                        word_str = word->super.vptr->getValue((Token *) word);

                        // extract word string into statement stdin
                        stmt->stdin_->streams[stmt->stdin_->n++] = Parser_expandWord(
                                word_str);

                        // resize strings buf
                        tmp = realloc(stmt->stdin_->streams, (stmt->stdin_->n + 1) * sizeof(char));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->stdin_->streams = tmp;
                        break;
                case IO_REDIR_STDOUT:
                        // take next word
                        word_str = word->super.vptr->getValue((Token *) word);

                        // extract word string into statement stdout
                        stmt->stdout_->streams[stmt->stdout_->n++] = Parser_expandWord(
                                word_str);

                        // resize strings buf
                        tmp = realloc(stmt->stdout_->streams, (stmt->stdout_->n + 1) * sizeof(char));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->stdout_->streams = tmp;
                        break;
                default:
                        return -1;
        }

        return 0;
}

/**
 * @brief Parses tokens into statements.
 * @param self @c Parser object
 * @return number of statements created on success, -1 on failure
 */
static ssize_t Parser_parseStatements_(struct Parser * const self)
{
        // initialize iterator with token stream
        TokenIterator iter;
        TokenIterator_ctor(&iter, self->_private->n_tokens, self->_private->tokens);

        // initialize statements array
        size_t buf_size = 1;
        Statement **stmts = malloc(buf_size * sizeof(Statement));

        // evaluate tokens from iterator stream
        ssize_t cur = 1;
        ssize_t count = 0; // number of statements consumed
        while (iter.vptr->has_next(&iter)) {
                if ((size_t) count >= buf_size) {
                        buf_size *= 2;
                        Statement **tmp = realloc(stmts, buf_size * sizeof(Statement));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmts = tmp;
                }

                Token tok = iter.vptr->peek(&iter, 0);

                if (is_tok_cmt(tok)) {
                        break; // done
                } else if (is_tok_ctrl_bg(tok)) {
                        if (count == 0) {
                                // syntax error
                        }
                        (void) iter.vptr->next(&iter);
                        stmts[count - 1]->flags |= FLAGS_BGCTRL;
                        cur++;
                } else if (is_tok_redir_input(tok)) {
                        if (stmts[count] == NULL) {
                                // syntax error
                        }
                        Parser_parseIORedir(stmts[count - 1], &iter,
                                            IO_REDIR_STDIN);
                } else if (is_tok_redir_output(tok)) {
                        if (stmts[count] == NULL) {
                                // syntax error
                        }
                        Parser_parseIORedir(stmts[count - 1], &iter,
                                            IO_REDIR_STDOUT);
                } else if (is_tok_word(tok)) {
                        if (count < cur) {
                                stmts[count++] = Statement_new();
                        } else if (stmts[count]->stdin_->n > 0 || stmts[count]->stdout_->n > 0) {
                                // syntax error
                                break;
                        }
                        Parser_parseCmd(stmts[count - 1], &iter);
                } else {
                        // do cleanup
                        count = -1;
                        break;
                }
        }

        TokenIterator_dtor(&iter);

        self->_private->stmts = stmts;
        self->_private->n_stmts = count;
        return count;
}

// TODO: add free/del method for tokens
/**
 * @brief Parses a character stream, creates tokens, and generates command
 * statements.
 *
 * Implementation of @c Parser::parse().
 *
 * @param self @c Parser object
 * @param buf character stream to parse
 * @return number of statements created on success, -1 on failure
 * @note Caller is responsible for freeing parsed statements via @c
 * Statement_del.
 */
static ssize_t Parser_parse_(Parser * const self, char *buf)
{
        // parse stream into tokens
        self->_private->tokens = malloc(MAX_TOKENS * sizeof(Token));
        self->_private->n_tokens = generate_tokens(buf, MAX_TOKENS,
                                                   self->_private->tokens);

        // parse tokens into statements
        return Parser_parseStatements_(self);
}

/**
 * @brief Returns parsed statements to caller.
 *
 * Implementation of @c Parser::get_statements().
 *
 * @param self @c Parser object
 * @return array of @c Statement objects parsed from character stream
 */
static inline Statement **Parser_getStatements_(struct Parser * const self)
{
        return self->_private->stmts;
}

/* *****************************************************************************
 * PUBLIC DEFINITIONS
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/* *****************************************************************************
 * CONSTRUCTORS + DESTRUCTORS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
void Parser_ctor(Parser *self)
{
        self->parse = &Parser_parse_;
        self->get_statements = &Parser_getStatements_;
        self->print_statement = &Statement_print;
        self->_private = malloc(sizeof(struct ParserPrivate));
}

// TODO: implement dtor
void Parser_dtor(Parser *self)
{
        (void) self;
}

/* *****************************************************************************
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
char *Parser_insertPID(char *string, char **ptr, size_t *len)
{
        char *result;

        // get pid and convert to string
#ifdef TEST
        // can't test valid results with randomized pid!
        // TODO: This could be done via mocking getpid() result. Find out how.
        pid_t pid = 123456;
#else
        pid_t pid = getpid();
#endif
        char *pid_str = calloc(sizeof(intmax_t) + 1, sizeof(char));
        if (pid_str == NULL) {
                return NULL;
        }

        size_t pid_len = snprintf(pid_str, sizeof(intmax_t), "%d", pid);
        if (pid_len >= sizeof(intmax_t)) {
                return NULL;
        }

        // save current index since realloc may change string location
        ptrdiff_t idx = *ptr - &string[0];

        // update copy size to fit pid
        size_t new_size = *len + pid_len - 2;
        result = calloc(new_size, sizeof(char));
        if (result == NULL) {
                return NULL;
        }

        char *tmp = strncpy(result, string, new_size - 1);
        if (tmp == NULL) {
                return NULL;
        }

        result = tmp;
        result[new_size - 1] = '\0';

        // reset copy pointer to point to end of result string
        char *new_ptr = &result[0] + idx;

        // extend result string with PID
        for (size_t i = 0; i < pid_len; i++) {
                *new_ptr++ = pid_str[i];
        }

        *len = new_size;
        *ptr = new_ptr;

        return result;
}

char *Parser_subVar(char *word, char **old_ptr, char **new_ptr, size_t *len)
{
        char *sub;
        char nxt;

        // peek ahead to first char of variable to determine type
        nxt = (*old_ptr)[1];
        switch (nxt) {
                case '$':
                {
                        // replace var with pid
                        sub = Parser_insertPID(word, new_ptr, len);
                        if (sub == NULL) {
                                return NULL;
                        }

                        // seek past '$'
                        (*old_ptr)++;
                        break;
                }
                default:
                {
                        sub = word;
                        // insert '$' at next available position
                        ptrdiff_t idx = *new_ptr - &sub[0]; // insertion offset
                        sub[idx] = **old_ptr;

                        // seek to next free position
                        (*new_ptr)++;
                        break;
                }
        }

        return sub;
}

char *Parser_expandWord(char *word)
{
        char *new_word, *old_ptr, *new_ptr;
        size_t len;

        /*
         * Allocate space for new word string. At a minimum it will be same
         * length as original word.
         */
        len = strlen(word) + 1;
        new_word = calloc(len, sizeof(char));

        // track copy and insert positions in respective strings
        old_ptr = &word[0];
        new_ptr = &new_word[0];

        /*
         * Copy over every byte from original word to new word, expanding
         * variables when required.
         */
        for (; *old_ptr != '\0'; old_ptr++) {
                if (*old_ptr == '$') {
                        // attempt to perform variable expansion
                        new_word = Parser_subVar(new_word, &old_ptr, &new_ptr, &len);
                } else {
                        // no expansion needed, so just copy over byte
                        *new_ptr = *old_ptr;
                        new_ptr++;
                }
        }

        return new_word;
}
