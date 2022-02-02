/**
 * @file parser.h
 * @author Mohamed Al-Hussein
 * @date 27 Jan 2022
 * @brief For parsing a stream of tokens into nodes for later evaluation.
 */
#ifndef SMALLSH_PARSER_H
#define SMALLSH_PARSER_H

#include <sys/types.h>

#include "token-iterator.h"
#include "utils/stack.h"
#include "statement.h"

/**
 * @brief Determines if a token is a comment token.
 */
#define is_tok_cmt(tok) ((ShellTokenType) tok.vptr->getType(&tok) == TOK_CMT)

/**
 * @brief Determines if a token is a background control token.
 */
#define is_tok_ctrl_bg(tok) ((ShellTokenType) tok.vptr->getType(&tok) == TOK_CTRL_BG)

/**
 * @brief Determines if a token is an input redirection token.
 */
#define is_tok_redir_input(tok) ((ShellTokenType) tok.vptr->getType(&tok) == TOK_REDIR_INPUT)

/**
 * @brief Determines if a token is an output redirection token.
 */
#define is_tok_redir_output(tok) ((ShellTokenType) tok.vptr->getType(&tok) == TOK_REDIR_OUTPUT)

/**
 * @brief Determines if a token is a word token.
 */
#define is_tok_word(tok) ((ShellTokenType) tok.vptr->getType(&tok) == TOK_WORD)

struct ParserPrivate;

/**
 * @brief Defines parser namespace.
 */
typedef struct Parser {
        /**
         * @brief Parses a character stream, creates tokens, and generates command
         * statements.
         *
         * <br>
         *
         * This step performs grouping of tokens based on the following production
         * rules:
         *
         * <br><br>
         *
         * node: comment | command | io_redir | bg_ctrl\n
         * comment: ^((whitespace)* '#' whitespace)\n
         * command: ^(word)+ [excluding '<', '>', '&', '#', whitespace, newline]\n
         * bg_ctrl: (whitespace '&' (whitespace | newline))$\n
         * io_redir: whitespace ('<' | '>') whitespace word\n
         * word: any consecutive characters, excluding whitespace and newline\n
         * whitespace: (' ' | '\\t')+\n
         * newline: '\\n'\n
         *
         * @param self @c Parser object
         * @param buf character stream to parse
         * @return number of statements created on success, -1 on failure
         * @note Caller is responsible for freeing parsed statements via @c
         * Statement_del.
         */
        ssize_t (*parse) (struct Parser * const self, char *buf);

        /**
         * @brief Returns parsed statements to caller.
         * @param self @c Parser object
         * @return array of @c Statement objects parsed from character stream
         */
        Statement **(*get_statements) (struct Parser * const self);

        /**
         * @brief Prints statement in a pretty-printed format to stdout.
         * @param stmt @c Statement object to print
         */
        void (*print_statement) (Statement const *stmt);

        /**
         * @brief @c Parser private data.
         */
        struct ParserPrivate *_private;
} Parser;

/**
 * @brief Initializes @p self @c Parser object.
 * @param self @c Parser object to initialize
 */
void Parser_ctor(Parser *self);

/**
 * @brief Destroys @p self @c Parser object.
 * @param self @c Parser object to destroy
 */
void Parser_dtor(Parser *self);

/**
 * @brief Substitutes all variables in a word string with their literal value,
 * and returns the modified string.
 *
 * This is the core expansion step for the parser.
 *
 * @param word word string to expand variables
 * @return new word string containing literal substitutions for all variables
 */
char *Parser_expandWord(char *word);

/**
 * @brief Extends @p string with the process's current PID.
 *
 * This is used during the expansion step of the parser, where '$$' is replaced
 * with the shell process PID.
 *
 * @pre @p ptr must point to the last character ('\\0') in the string. If
 * this is not true, then any remaining bytes after @p ptr will be overwritten
 * by the PID.
 * @param string string to extend
 * @param ptr pointer to current position in string
 * @param len input/output param for final string length
 * @return new string with PID at end, or @c NULL on error
 */
char *Parser_insertPID(char *string, char **ptr, size_t *len);

/**
 * @brief Substitutes variable followed by '$' into word and returns result. If
 * no valid variable follows '$', then the '$' is copied into word instead.
 *
 * This is used during the expansion step of the parser, where variable
 * substitution is performed. As of now, only PID expansion is supported.
 *
 * @pre @p old_ptr must point to the first '$' character.
 * @pre @p new_pointer must point to the first null ('\\0) character in @p word.
 * @pre @p len must be long enough to fit original string characters plus null
 * terminator.
 * @param word the word string to extend
 * @param old_ptr pointer to '$' character in original string
 * @param new_ptr pointer to next available character in @p word for insertion
 * @param len input/output parameter for final string length
 * @return new string with variable substituted at end, or just the '$' if no
 * valid variables present
 */
char *Parser_subVar(char *word, char **old_ptr, char **new_ptr, size_t *len);

#endif //SMALLSH_PARSER_H
