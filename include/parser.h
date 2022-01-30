/**
 * @file parser.h
 * @author Mohamed Al-Hussein
 * @date 27 Jan 2022
 * @brief For parsing a stream of tokens into nodes for later evaluation.
 */
#ifndef SMALLSH_PARSER_H
#define SMALLSH_PARSER_H

#include "token-iterator.h"
#include "stack.h"

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
 * @brief Determines if a token is a redirection token.
 */
#define is_tok_redir(tok) (is_tok_redir_input(tok) || is_tok_redir_output(tok))

/**
 * @brief Determines if a token is a word token.
 */
#define is_tok_word(tok) ((ShellTokenType) tok.vptr->getType(&tok) == TOK_WORD)

struct ParserPrivate;


typedef struct Parser {
        size_t (*parse) (struct Parser * const self, char *buf);
        void (*cleanup) (struct Parser * const self, bool print_tree);
        struct ParserPrivate *_private;
} Parser;

/**
 * @brief Parses token stream from @p tokens into nodes and stores them inside
 * @p node_stack.
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
 * @pre @p node_stack should be large enough to fit at least @p n_tokens number
 * of nodes.
 * @param n_tokens number of tokens in token stream
 * @param tokens token stream to parse
 * @param node_stack output stack for parsed nodes
 * @return number of nodes parsed
 * @note Nodes added to the stack, as well as their data members, are all
 * allocated on the heap. Caller is responsible for freeing them after they're
 * no longer needed.
 */
size_t parse(size_t n_tokens, Token *tokens[n_tokens], Stack *node_stack);

void Parser_ctor(Parser *self);
void Parser_dtor(Parser *self);

#endif //SMALLSH_PARSER_H
