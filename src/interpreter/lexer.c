/**
 * @file lexer.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief For tokenizing a shell input string.
 */
#include <stdlib.h>

#include "interpreter/lexer.h"
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
 * FUNCTIONS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
size_t SH_LexerGenerateTokens(char * const buf, size_t const max_tok,
                              SH_Token *toks[max_tok])
{
        SH_StringIterator *iter
        __attribute__((cleanup(SH_DestroyStringIterator)));

        iter = SH_CreateStringIterator(buf);
        size_t count = 0; // number of tokens consumed
        char c1, c2;

        while (SH_StringIteratorHasNext(iter)) {
                if (count > max_tok) {
                        // no more space for tokens
                        break;
                }

                // peek ahead to next two characters
                c1 = SH_StringIteratorPeek(iter, 0);
                c2 = SH_StringIteratorPeek(iter, 1);

                // tokenize value
                if (IS_WHITESPACE(c1)) {
                        // ignore whitespace
                        SH_StringIteratorNext(iter);
                        continue;
                } else if (IS_CMT_SYM(c1)) {
                        if (count == 0) {
                                // token is a comment
                                toks[count] = SH_CreateToken(TOK_CMT);
                        } else {
                                /* Comment symbol is part of a word token. */
                                goto consume_word;
                        }
                } else if (IS_INPUT_REDIR_OP(c1, c2)) {
                        // token is an input redirection operator
                        toks[count] = SH_CreateToken(TOK_REDIR_INPUT);
                } else if (IS_OUTPUT_REDIR_OP(c1, c2)) {
                        // token is an output redirection operator
                        toks[count] = SH_CreateToken(TOK_REDIR_OUTPUT);
                } else if (IS_BG_CTRL_OP(c1, c2)) {
                        // token is a background control operator
                        toks[count] = SH_CreateToken(TOK_CTRL_BG);
                } else if (IS_NEWLINE(c1)) {
                        // token is a newline
                        toks[count] = SH_CreateToken(TOK_CTRL_NEWLINE);
                } else {
consume_word:
                        // otherwise, token is a word
                        toks[count] = SH_CreateToken(TOK_WORD);
                }

                // consume token
                SH_TakeToken(toks[count], iter);
                count++;
        }

        return count;
}
