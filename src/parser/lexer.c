/**
 * @file lexer.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For tokenizing a shell input string.
 */
#include <stdlib.h>

#include "parser/lexer.h"
#include "parser/shell-tokens.h"

size_t lexer_generate_tokens(char *buf, size_t max_tok, Token *tok[max_tok])
{
        StringIterator iter;
        string_iterator_ctor(&iter, buf);

        size_t count = 0; // number of tokens consumed
        char c1, c2;

        while (iter.vptr->has_next(&iter)) {
                if (count > max_tok) {
                        // no more space for tokens
                        break;
                }

                // peek ahead to next two characters
                c1 = iter.vptr->peek(&iter, 0);
                c2 = iter.vptr->peek(&iter, 1);

                // tokenize value
                if (IS_WHITESPACE(c1)) {
                        // ignore whitespace
                        iter.vptr->next(&iter);
                        continue;
                } else if (IS_CMT_SYM(c1, c2)) {
                        // token is a comment
                        tok[count] = malloc(sizeof(Token));
                        comment_token_ctor((CommentToken *) tok[count]);
                } else if (IS_INPUT_REDIR_OP(c1, c2)) {
                        // token is an input redirection operator
                        tok[count] = malloc(sizeof(Token));
                        inputredir_token_ctor((InputRedirToken *) tok[count]);
                } else if (IS_OUTPUT_REDIR_OP(c1, c2)) {
                        // token is an output redirection operator
                        tok[count] = malloc(sizeof(Token));
                        outputredir_token_ctor((OutputRedirToken *) tok[count]);
                } else if (IS_BG_CTRL_OP(c1, c2)) {
                        // token is a background control operator
                        tok[count] = malloc(sizeof(Token));
                        bgctrl_token_ctor((BGCtrlToken *) tok[count]);
                } else if (IS_NEWLINE(c1)) {
                        // token is a newline
                        tok[count] = malloc(sizeof(Token));
                        newline_token_ctor((NewlineToken *) tok[count]);
                } else {
                        // otherwise, token is a word
                        tok[count] = malloc(sizeof(Token));
                        word_token_ctor((WordToken *) tok[count]);
                }

                // consume token
                ((Token *) tok[count])->vptr->take((Token *) tok[count], &iter);

                count++;
        }

        string_iterator_dtor(&iter);

        return count;
}
