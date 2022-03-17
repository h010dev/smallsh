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

#include "builtins/builtins.h"
#include "interpreter/lexer.h"
#include "interpreter/parser.h"
#include "interpreter/token-iterator.h"
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
static int SH_ParserParseCmd(SH_Statement *stmt, SH_TokenIterator * const iter)
{
        // parse words into statement command
        size_t buf_size = 1;

        while (SH_TokenIteratorHasNext(iter)) {
                SH_Token *tok1, *tok2;
                char **tmp;

                // check if any words left to take
                tok1 = SH_TokenIteratorPeek(iter, 0);
                tok2 = SH_TokenIteratorPeek(iter, 1);
                if (tok1->type != TOK_WORD) {
                        if (tok1->type == TOK_CTRL_BG) {
                                if (tok2->type == TOK_CTRL_NEWLINE) {
                                        break; // done
                                }
                        } else {
                                break; // done
                        }
                }

                // resize buf if full
                if (stmt->cmd->count >= buf_size) {
                        buf_size *= 2;
                        tmp = realloc(stmt->cmd->args,
                                      buf_size * sizeof(char *));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->cmd->args = tmp;
                }

                // take word
                stmt->cmd->args[stmt->cmd->count++] =
                        SH_ParserExpandWord(SH_TokenIteratorNext(iter)->value);
        }

        // resize array to fit exactly argc + 1 elements for later use with exec
        char **tmp = realloc(stmt->cmd->args,
                             (stmt->cmd->count + 1) * sizeof(char *));
        if (tmp == NULL) {
                return -1; // error
        }
        stmt->cmd->args = tmp;

        // null terminate argv for exec
        stmt->cmd->args[stmt->cmd->count] = NULL;

        /* Check if command is a supported builtin. */
        if (SH_IsBuiltin(stmt->cmd->args[0])) {
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
static int SH_ParserParseIoRedir(SH_Statement *stmt,
                                 SH_TokenIterator *const iter,
                                 IORedirType const type)
{
        // filename should be a word token
        SH_Token *tok = SH_TokenIteratorPeek(iter, 1);
        if (tok->type != TOK_WORD) {
                return -1; // error
        }

        // skip past redirection operator
        (void) SH_TokenIteratorNext(iter);

        // take next word
        SH_Token *wt = SH_TokenIteratorNext(iter);

        // switch to type stream
        char **tmp;
        switch (type) {
                case IOREDIR_STDIN:
                        // take next word; extract word string into statement stdin
                        stmt->infile->streams[stmt->infile->n++] =
                                SH_ParserExpandWord(wt->value);

                        // resize strings buf
                        tmp = realloc(stmt->infile->streams,
                                      (stmt->infile->n + 1) * sizeof(char *));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->infile->streams = tmp;
                        stmt->infile->streams[
                                stmt->infile->n] = NULL;
                        break;
                case IOREDIR_STDOUT:
                        // take next work; extract word string into statement stdout
                        stmt->outfile->streams[stmt->outfile->n++] =
                                SH_ParserExpandWord(wt->value);

                        // resize strings buf
                        tmp = realloc(stmt->outfile->streams,
                                      (stmt->outfile->n + 1) * sizeof(char *));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmt->outfile->streams = tmp;
                        stmt->outfile->streams[
                                stmt->outfile->n] = NULL;
                        break;
                default:
                        return -1;
        }

        return 0;
}

/**
 * @brief Parses tokens into statements.
 * @param parser @c Parser object
 * @return number of statements created on success, -1 on failure
 */
static ssize_t SH_ParserParseStmts(SH_Parser *const parser)
{
        // initialize iterator with token stream
        SH_TokenIterator *iter __attribute__((cleanup(SH_DestroyTokenIterator)));

        iter = SH_CreateTokenIterator(parser->n_toks, parser->toks);

        // initialize statements array
        size_t buf_size = 1;
        SH_Statement **stmts = malloc(buf_size * sizeof(SH_Statement));

        // evaluate tokens from iterator stream
        ssize_t cur = 1;
        ssize_t count = 0; // number of statements consumed
        while (SH_TokenIteratorHasNext(iter)) {
                if ((size_t) count >= buf_size) {
                        buf_size *= 2;
                        SH_Statement **tmp =
                                realloc(stmts, buf_size * sizeof(SH_Statement));
                        if (tmp == NULL) {
                                return -1; // error
                        }
                        stmts = tmp;
                }

                SH_Token *tok1 = SH_TokenIteratorPeek(iter, 0);
#ifdef DEMO
                SH_Token *tok2 = SH_TokenIteratorPeek(iter, 1);
#endif
                switch (tok1->type) {
                        case TOK_CMT:
                                break; // done
                        case TOK_CTRL_BG:
                        {
#ifdef DEMO
                                if (tok2->type == TOK_CTRL_NEWLINE) {
                                        (void) SH_TokenIteratorNext(iter);
                                        stmts[count - 1]->flags |= FLAGS_BGCTRL;
                                        break;
                                }
#endif
                                if (count == 0) {
                                        // syntax error
                                }
                                (void) SH_TokenIteratorNext(iter);
                                stmts[count - 1]->flags |= FLAGS_BGCTRL;
                                cur++;
                                break;
                        }
                        case TOK_CTRL_NEWLINE:
                        {
                                if (count == 0) {
                                        // empty line
                                        break;
                                }
                                (void) SH_TokenIteratorNext(iter);
                                cur++;
                                break;
                        }
                        case TOK_REDIR_INPUT:
                        {
                                if (stmts[count] == NULL) {
                                        // syntax error
                                }
                                SH_ParserParseIoRedir(stmts[count - 1], iter,
                                                      IOREDIR_STDIN);

                                break;
                        }
                        case TOK_REDIR_OUTPUT:
                        {
                                if (stmts[count] == NULL) {
                                        // syntax error
                                }
                                SH_ParserParseIoRedir(stmts[count - 1], iter,
                                                      IOREDIR_STDOUT);

                                break;
                        }
                        case TOK_WORD:
                        {
                                if (count < cur) {
                                        stmts[count++] = SH_CreateStatement();
                                } else if (stmts[count]->infile->n > 0 ||
                                           stmts[count]->outfile->n > 0) {
                                        // syntax error
                                        break;
                                }
                                SH_ParserParseCmd(stmts[count - 1], iter);
                                break;
                        }
                        default:
                                // do cleanup
                                count = -1;
                                break;
                }
        }

        parser->stmts = stmts;
        parser->n_stmts = count;
        return count;
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
SH_Parser *SH_CreateParser(void)
{
        SH_Parser *parser;

        parser = malloc(sizeof *parser);
        if (parser == NULL) {
                return NULL;
        }

        return parser;
}

void SH_DestroyParser(SH_Parser **parser)
{
        for (size_t i = 0; i < (*parser)->n_toks; i++) {
                SH_DestroyToken(&(*parser)->toks[i]);
        }
        free((*parser)->toks);
        (*parser)->n_toks = 0;
        (*parser)->toks = NULL;

        for (ssize_t i = 0; i < (*parser)->n_stmts; i++) {
                SH_DestroyStatement(&(*parser)->stmts[i]);
        }
        free((*parser)->stmts);
        (*parser)->n_stmts = 0;
        (*parser)->stmts = NULL;

        free(*parser);
        *parser = NULL;
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
char *SH_ParserExpandWord(char * const word)
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
                        new_word = SH_ParserSubstituteVariable(new_word,
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

char *SH_ParserInsertPid(char * const str, char **ptr, size_t * const len)
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

ssize_t SH_ParserParse(SH_Parser *const parser, char *buf)
{
        // parse stream into tokens
        parser->toks = malloc(MAX_TOKENS * sizeof(SH_Token));
        parser->n_toks = SH_LexerGenerateTokens(buf, MAX_TOKENS, parser->toks);

        // parse tokens into statements
        return SH_ParserParseStmts(parser);
}

char *SH_ParserSubstituteVariable(char * const word, char **old_ptr,
                                  char **new_ptr, size_t * const len)
{
        char *sub;
        char nxt;

        // peek ahead to first char of variable to determine type
        nxt = (*old_ptr)[1];
        switch (nxt) {
                case '$':
                {
                        // replace var with pid
                        sub = SH_ParserInsertPid(word, new_ptr, len);
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
