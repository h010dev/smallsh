/**
 * @file lexer.h
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For tokenizing a shell input string.
 */
#ifndef SMALLSH_LEXER_H
#define SMALLSH_LEXER_H

#include "interpreter/token.h"

#define MAX_TOKENS 513 /* 512 max args plus newline */

// whitespace
#define CHAR_SPACE ' '
#define CHAR_TAB '\t'
#define IS_WHITESPACE(c) (c == CHAR_SPACE || c == CHAR_TAB)

// separators
#define CHAR_NEWLINE '\n'
#define CHAR_EOL '\0'
#define IS_NEWLINE(c) (c == CHAR_NEWLINE)
#define IS_EOL(c) (c == CHAR_EOL)
#define IS_TERMINAL(c) (IS_WHITESPACE(c) || IS_EOL(c) || IS_NEWLINE(c))

// redirection operators
#define INPUT_REDIR_OP '<'
#define OUTPUT_REDIR_OP '>'
#define IS_INPUT_REDIR_OP(c1, c2) (c1 == INPUT_REDIR_OP && IS_TERMINAL(c2))
#define IS_OUTPUT_REDIR_OP(c1, c2) (c1 == OUTPUT_REDIR_OP && IS_TERMINAL(c2))

// control operators
#define BG_CTRL_OP '&'
#define IS_BG_CTRL_OP(c1, c2) (c1 == BG_CTRL_OP && IS_TERMINAL(c2))

// comment symbols
#define CMT_SYM '#'
#define IS_CMT_SYM(c1) (c1 == CMT_SYM)

/**
 * @brief Given an text stream @p buf, scans stream and generates @c Token
 * objects to store in @p tokens.
 * @param buf text stream to scan
 * @param max_tok size of @p tokens array
 * @param toks array of pointers to @c Token objects to store token data
 * @return number of tokens parsed from stream
 * @note Caller should verify that the number of tokens parsed is less than
 * @p n_tokens. If this is not the case, then input stream was not fully
 * tokenized.
 */
size_t SH_LexerGenerateTokens(char *buf, size_t max_tok, SH_Token *toks[max_tok]);

#endif //SMALLSH_LEXER_H
