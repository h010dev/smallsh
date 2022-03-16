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

#include "interpreter/parser.h"
#include "interpreter/token-iterator.h"
#include "interpreter/lexer.h"
#include "builtins/builtins.h"

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
        size_t parser_num_tok; /**< number of tokens parsed */
        Token **parser_tok; /**< parsed tokens */
        ssize_t parser_num_stmt; /**< number of statements created */
        Statement **parser_stmt; /**< statements created */
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
static int parser_parse_cmd_(Statement *stmt, TokenIterator *iter)
{
        // parse words into statement command
        size_t buf_size = 1;

        while (iter->vptr->has_next(iter)) {
                Token tok1, tok2;
                char **tmp;

                // check if any words left to take
                tok1 = iter->vptr->peek(iter, 0);
                tok2 = iter->vptr->peek(iter, 1);
                if (!is_tok_word(tok1)) {
                        if (is_tok_ctrl_bg(tok1)) {
                                if (is_tok_ctrl_newline(tok2)) {
                                        break; // done
                                }
                        } else if (!is_tok_word(tok1)) {
                                break; // done
                        }
                }

                // resize buf if full
                if (stmt->stmt_cmd->cmd_argc >= buf_size) {
                        buf_size *= 2;
                        tmp = realloc(stmt->stmt_cmd->cmd_argv,
                                      buf_size * sizeof(char *));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->stmt_cmd->cmd_argv = tmp;
                }

                // take word
                WordToken *wt = (WordToken *) iter->vptr->next(iter);
                char *wt_str = wt->super.vptr->get_value((Token *) wt);
                stmt->stmt_cmd->cmd_argv[stmt->stmt_cmd->cmd_argc++] =
                        parser_expand_word(wt_str);
                free(wt_str);
        }

        // resize array to fit exactly argc + 1 elements for later use with exec
        char **tmp = realloc(stmt->stmt_cmd->cmd_argv,
                             (stmt->stmt_cmd->cmd_argc + 1) * sizeof(char *));
        if (tmp == NULL) {
                return -1; // error
        }
        stmt->stmt_cmd->cmd_argv = tmp;

        // null terminate argv for exec
        stmt->stmt_cmd->cmd_argv[stmt->stmt_cmd->cmd_argc] = NULL;

        /* Check if command is a supported builtin. */
        if (SH_IsBuiltin(stmt->stmt_cmd->cmd_argv[0])) {
                stmt->stmt_flags |= FLAGS_BUILTIN;
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
static int parser_parse_ioredir_(Statement *stmt, TokenIterator *iter,
                                 IORedirType type)
{
        // filename should be a word token
        Token tok = iter->vptr->peek(iter, 1);
        if (!is_tok_word(tok)) {
                return -1; // error
        }

        // skip past redirection operator
        (void) iter->vptr->next(iter);

        // take next word
        WordToken *wt = (WordToken *) iter->vptr->next(iter);

        // switch to type stream
        char *wt_str;
        char **tmp;
        switch (type) {
                case IOREDIR_STDIN:
                        // take next word
                        wt_str = wt->super.vptr->get_value((Token *) wt);

                        // extract word string into statement stdin
                        stmt->stmt_stdin->stdin_streams[
                                stmt->stmt_stdin->stdin_num_streams++] =
                                parser_expand_word(wt_str);
                        free(wt_str);

                        // resize strings buf
                        tmp = realloc(stmt->stmt_stdin->stdin_streams,
                                      (stmt->stmt_stdin->stdin_num_streams + 1)
                                      * sizeof(char *));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->stmt_stdin->stdin_streams = tmp;
                        stmt->stmt_stdin->stdin_streams[
                                stmt->stmt_stdin->stdin_num_streams] = NULL;
                        break;
                case IOREDIR_STDOUT:
                        // take next word
                        wt_str = wt->super.vptr->get_value((Token *) wt);

                        // extract word string into statement stdout
                        stmt->stmt_stdout->stdout_streams[
                                stmt->stmt_stdout->stdout_num_streams++] =
                                parser_expand_word(wt_str);
                        free(wt_str);

                        // resize strings buf
                        tmp = realloc(stmt->stmt_stdout->stdout_streams,
                                      (stmt->stmt_stdout->stdout_num_streams + 1)
                                      * sizeof(char *));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->stmt_stdout->stdout_streams = tmp;
                        stmt->stmt_stdout->stdout_streams[
                                stmt->stmt_stdout->stdout_num_streams] = NULL;
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
static ssize_t parser_parse_statements_(struct Parser * const self)
{
        // initialize iterator with token stream
        TokenIterator iter;
        token_iterator_ctor(&iter, self->private->parser_num_tok,
                            self->private->parser_tok);

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

                Token tok1 = iter.vptr->peek(&iter, 0);
#ifdef DEMO
                Token tok2 = iter.vptr->peek(&iter, 1);
#endif

                if (is_tok_cmt(tok1)) {
                        break; // done
                } else if (is_tok_ctrl_bg(tok1)) {
#ifdef DEMO
                        if (is_tok_ctrl_newline(tok2)) {
                                (void) iter.vptr->next(&iter);
                                stmts[count - 1]->stmt_flags |= FLAGS_BGCTRL;
                                break;
                        }
#endif
                        if (count == 0) {
                                // syntax error
                        }
                        (void) iter.vptr->next(&iter);
                        stmts[count - 1]->stmt_flags |= FLAGS_BGCTRL;
                        cur++;
                } else if (is_tok_ctrl_newline(tok1)) {
                        if (count == 0) {
                                // empty line
                                break;
                        }
                        (void) iter.vptr->next(&iter);
                        cur++;
                } else if (is_tok_redir_input(tok1)) {
                        if (stmts[count] == NULL) {
                                // syntax error
                        }
                        parser_parse_ioredir_(stmts[count - 1], &iter,
                                              IOREDIR_STDIN);
                } else if (is_tok_redir_output(tok1)) {
                        if (stmts[count] == NULL) {
                                // syntax error
                        }
                        parser_parse_ioredir_(stmts[count - 1], &iter,
                                              IOREDIR_STDOUT);
                } else if (is_tok_word(tok1)) {
                        if (count < cur) {
                                stmts[count++] = statement_new();
                        } else if (stmts[count]->stmt_stdin->stdin_num_streams > 0 ||
                        stmts[count]->stmt_stdout->stdout_num_streams > 0) {
                                // syntax error
                                break;
                        }
                        parser_parse_cmd_(stmts[count - 1], &iter);
                } else {
                        // do cleanup
                        count = -1;
                        break;
                }
        }

        token_iterator_dtor(&iter);

        self->private->parser_stmt = stmts;
        self->private->parser_num_stmt = count;
        return count;
}

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
 * statement_del.
 */
static ssize_t parser_parse_(Parser * const self, char *buf)
{
        // parse stream into tokens
        self->private->parser_tok = malloc(MAX_TOKENS * sizeof(Token));
        self->private->parser_num_tok = lexer_generate_tokens(buf, MAX_TOKENS,
                                                              self->private->parser_tok);

        // parse tokens into statements
        return parser_parse_statements_(self);
}

/**
 * @brief Returns parsed statements to caller.
 *
 * Implementation of @c Parser::get_statements().
 *
 * @param self @c Parser object
 * @return array of @c Statement objects parsed from character stream
 */
static inline Statement **parser_get_statements_(struct Parser * const self)
{
        return self->private->parser_stmt;
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
void parser_ctor(Parser *self)
{
        self->parse = &parser_parse_;
        self->get_statements = &parser_get_statements_;
        self->print_statement = &statement_print;
        self->private = malloc(sizeof(struct ParserPrivate));
}

void parser_dtor(Parser *self)
{
        for (size_t i = 0; i < self->private->parser_num_tok; i++) {
                token_dtor(self->private->parser_tok[i]);
                free(self->private->parser_tok[i]);
                self->private->parser_tok[i] = NULL;
        }
        free(self->private->parser_tok);
        self->private->parser_num_tok = 0;
        self->private->parser_tok = NULL;

        for (ssize_t i = 0; i < self->private->parser_num_stmt; i++) {
                statement_del(&self->private->parser_stmt[i]);
        }
        free(self->private->parser_stmt);
        self->private->parser_num_stmt = 0;
        self->private->parser_stmt = NULL;

        free(self->private);
        self->private = NULL;

        self->parse = NULL;
        self->get_statements = NULL;
        self->print_statement = NULL;
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
char *parser_insert_pid(char *str, char **ptr, size_t *len)
{
        char *result;

        // get pid and convert to string
#ifdef TEST
        // can't test valid results with randomized pid!
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
                free(pid_str);
                return NULL;
        }

        // save current index since realloc may change string location
        ptrdiff_t idx = *ptr - &str[0];

        // update copy size to fit pid
        size_t new_size = *len + pid_len - 2;
        result = calloc(new_size, sizeof(char));
        if (result == NULL) {
                free(pid_str);
                return NULL;
        }

        char *tmp = strncpy(result, str, new_size - 1);
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

        free(pid_str);

        return result;
}

char *parser_substitute_variable(char *word, char **old_ptr, char **new_ptr,
                                 size_t *len)
{
        char *sub;
        char nxt;

        // peek ahead to first char of variable to determine type
        nxt = (*old_ptr)[1];
        switch (nxt) {
                case '$':
                {
                        // replace var with pid
                        sub = parser_insert_pid(word, new_ptr, len);
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

char *parser_expand_word(char *word)
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
                        new_word = parser_substitute_variable(new_word,
                                                              &old_ptr,
                                                              &new_ptr,
                                                              &len);
                } else {
                        // no expansion needed, so just copy over byte
                        *new_ptr = *old_ptr;
                        new_ptr++;
                }
        }

        return new_word;
}
