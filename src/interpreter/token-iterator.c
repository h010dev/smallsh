/**
 * @file token-iterator.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For iterating over a collection of tokens.
 */
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
 * @brief Hides @c TokenIterator members from client code.
 */
struct TokenIteratorPrivate {
        size_t ti_len; /**< length of iterable */
        Token **ti_tok; /**< token array we are iterating over */
        size_t ti_cur; /**< cursor position of iterator */
};

/**
 * @brief Implementation of @c TokenIterator::has_next().
 * @param self pointer to iterator object
 * @return true if iterator is positioned at a parseable token, false
 * otherwise
 */
static inline bool token_iterator_has_next_(TokenIterator const * const self)
{
        Token iter;
        ShellTokenType type;
        bool in_bounds;

        // grab last token to check if it's a newline token
        iter = *self->private->ti_tok[self->private->ti_cur];
        type = (ShellTokenType) iter.vptr->get_type(&iter);

        // cursor can be at most len - 1
        in_bounds = self->private->ti_cur < self->private->ti_len;

        return in_bounds && type != TOK_CTRL_NEWLINE;
}

/**
 * @brief Implementation of @c TokenIterator::next().
 * @param self pointer to iterator object
 * @return pointer to next token in iterable
 */
static inline Token *token_iterator_next_(TokenIterator const * const self)
{
        return self->private->ti_tok[self->private->ti_cur++];
}

/**
 * @brief Implementation of @c TokenIterator::peek().
 * @param self pointer to iterator object
 * @param offset how far ahead to peek
 * @return token pointed to by iterator at given offset
 */
static inline Token token_iterator_peek_(TokenIterator const * const self,
                                         unsigned int offset)
{
        Token *cur;
        Token result;

        // store original index
        size_t orig = self->private->ti_cur;

        unsigned int pos = 0;
        do {
                cur = self->vptr->next(self);
                pos++;
        } while (pos <= offset && self->vptr->has_next(self));

        if (pos <= offset) {
                // TODO: find a better way to return null if possible
                // newline token acts as null terminator for token array
                NewlineToken n;
                newline_token_ctor(&n);
                result = *(Token *) &n;
        } else {
                result = *cur;
        }

        self->private->ti_cur = orig;

        return result;
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
// TODO: look into benefits/tradeoffs for deep-copying the tokens array
void token_iterator_ctor(TokenIterator *self, size_t len, Token *tok[len])
{
        static struct TokenIteratorVtbl const vtbl = {
                .has_next = &token_iterator_has_next_,
                .next = &token_iterator_next_,
                .peek = &token_iterator_peek_,
        };
        self->vptr = &vtbl;
        self->private = malloc(sizeof(struct TokenIteratorPrivate));

        self->private->ti_len = len;
        self->private->ti_tok = tok;
        self->private->ti_cur = 0;
}

void token_iterator_dtor(TokenIterator *self)
{
        self->vptr = NULL;
        self->private->ti_len = 0;
        self->private->ti_tok = NULL;
        self->private->ti_cur = 0;
        free(self->private);
        self->private = NULL;
}
