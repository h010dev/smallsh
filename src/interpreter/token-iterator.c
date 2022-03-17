/**
 * @file token-iterator.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For iterating over a collection of tokens.
 */
#include "interpreter/token-iterator.h"
#include "interpreter/token.h"
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
SH_TokenIterator *SH_CreateTokenIterator(size_t const len, SH_Token *toks[len])
{
        SH_TokenIterator *iter;

        iter = malloc(sizeof *iter);
        if (iter == NULL) {
                return NULL;
        }

        iter->len = len;
        iter->toks = toks;
        iter->cur = 0;

        return iter;
}

void SH_DestroyTokenIterator(SH_TokenIterator **iter)
{
        (*iter)->len = 0;
        (*iter)->toks = NULL;
        (*iter)->cur = 0;

        free(*iter);
        *iter = NULL;
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
bool SH_TokenIteratorHasNext(SH_TokenIterator const * const iter)
{
        SH_Token tok;
        bool in_bounds;

        // grab last token to check if it's a newline token
        tok = *iter->toks[iter->cur];

        // cursor can be at most len - 1
        in_bounds = iter->cur < iter->len;

        return in_bounds && tok.type != TOK_CTRL_NEWLINE;
}

SH_Token *SH_TokenIteratorNext(SH_TokenIterator *const iter)
{
        return iter->toks[iter->cur++];
}

SH_Token *
SH_TokenIteratorPeek(SH_TokenIterator *const iter, unsigned int const offset)
{
        SH_Token *cur;
        SH_Token *result;

        // store original index
        size_t orig = iter->cur;

        unsigned int pos = 0;
        do {
                cur = SH_TokenIteratorNext(iter);
                pos++;
        } while (pos <= offset && SH_TokenIteratorHasNext(iter));

        if (pos <= offset) {
                // newline token acts as null terminator for token array
                result = SH_CreateToken(TOK_CTRL_NEWLINE);
        } else {
                result = cur;
        }

        iter->cur = orig;

        return result;
}
