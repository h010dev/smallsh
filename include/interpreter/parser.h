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
 * @brief Defines parser namespace.
 */
typedef struct SH_Parser {
        size_t n_toks; /**< number of tokens parsed */
        SH_Token **toks; /**< parsed tokens */
        ssize_t n_stmts; /**< number of statements created */
        SH_Statement **stmts; /**< statements created */
} SH_Parser;

/**
 * @brief Initializes @p self @c Parser object.
 * @param self @c Parser object to initialize
 */
SH_Parser *SH_CreateParser(void);

/**
 * @brief Destroys @p parser @c Parser object.
 * @param parser @c Parser object to destroy
 */
void SH_DestroyParser(SH_Parser **parser);

/**
 * @brief Substitutes all variables in a word string with their literal value,
 * and returns the modified string.
 *
 * This is the core expansion step for the parser.
 *
 * @param word word string to expand variables
 * @return new word string containing literal substitutions for all variables
 */
char *SH_ParserExpandWord(char *word);

/**
 * @brief Extends @p string with the process's current PID.
 *
 * This is used during the expansion step of the parser, where '$$' is replaced
 * with the shell process PID.
 *
 * @pre @p ptr must point to the last character ('\\0') in the string. If
 * this is not true, then any remaining bytes after @p ptr will be overwritten
 * by the PID.
 * @param str string to extend
 * @param ptr pointer to current position in string
 * @param len input/output param for final string length
 * @return new string with PID at end, or @c NULL on error
 */
char *SH_ParserInsertPid(char *str, char **ptr, size_t *len);

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
 * @param parser @c Parser object
 * @param buf character stream to parse
 * @return number of statements created on success, -1 on failure
 * @note Caller is responsible for freeing parsed statements via @c
 * SH_DestroyStatement.
 */
ssize_t SH_ParserParse(SH_Parser *parser, char *buf);

/**
 * @brief Prints statement in a pretty-printed format to stdout.
 * @param stmt @c Statement object to print
 */
void *SH_ParserPrintStmt(SH_Statement const *stmt);

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
char *SH_ParserSubstituteVariable(char *word, char **old_ptr, char **new_ptr,
                                  size_t *len);

#endif //SMALLSH_PARSER_H
