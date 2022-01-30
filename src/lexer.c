/**
 * @file lexer.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For tokenizing a shell input string.
 */
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "lexer.h"
#include "shell-tokens.h"

size_t generate_tokens(char *buf, size_t n_tokens, Token *tokens[n_tokens])
{
#ifdef DEBUG
        printf("%s\n", buf);
#endif

        StringIterator iter;
        StringIterator_ctor(&iter, buf);

        size_t count = 0; // number of tokens consumed
        char c1, c2;

        while (iter.vptr->has_next(&iter)) {
                if (count > n_tokens) {
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
                        tokens[count] = malloc(sizeof(Token));
                        CommentToken_ctor((CommentToken *) tokens[count]);
                } else if (IS_INPUT_REDIR_OP(c1, c2)) {
                        // token is an input redirection operator
                        tokens[count] = malloc(sizeof(Token));
                        InputRedirToken_ctor((InputRedirToken *) tokens[count]);
                } else if (IS_OUTPUT_REDIR_OP(c1, c2)) {
                        // token is an output redirection operator
                        tokens[count] = malloc(sizeof(Token));
                        OutputRedirToken_ctor((OutputRedirToken *) tokens[count]);
                } else if (IS_BG_CTRL_OP(c1, c2)) {
                        // token is a background control operator
                        tokens[count] = malloc(sizeof(Token));
                        BGControlToken_ctor((BGControlToken *) tokens[count]);
                } else if (IS_NEWLINE(c1)) {
                        // token is a newline
                        tokens[count] = malloc(sizeof(Token));
                        NewlineToken_ctor((NewlineToken *) tokens[count]);
                } else {
                        // otherwise, token is a word
                        tokens[count] = malloc(sizeof(Token));
                        WordToken_ctor((WordToken *) tokens[count]);
                }

                // consume token
                ((Token *) tokens[count])->vptr->take((Token *) tokens[count], &iter);

                count++;
        }

        StringIterator_dtor(&iter);

#ifdef DEBUG
        if (count > 0) {
                printf("[");
                size_t i = 0;
                for (; i < count - 1; i++) {
                        ((Token *) tokens[i])->vptr->print((Token *) tokens[i]);
                        printf(", ");
                }
                ((Token *) tokens[i])->vptr->print((Token *) tokens[i]);
                printf("]\n");
        }
#endif

        return count;
}
