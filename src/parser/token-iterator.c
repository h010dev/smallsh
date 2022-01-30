/**
 * @file token-iterator.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For iterating over a collection of tokens.
 */
#include "parser/token-iterator.h"

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
        size_t len; /**< length of iterable */
        Token **tokens; /**< token array we are iterating over */
        size_t current; /**< cursor position of iterator */
};

/**
 * @brief Implementation of @c TokenIterator::has_next().
 * @param self pointer to iterator object
 * @return true if iterator is positioned at a parseable token, false
 * otherwise
 */
static inline bool TokenIterator_hasNext_(TokenIterator const * const self)
{
        Token iter;
        ShellTokenType type;
        bool in_bounds;

        // grab last token to check if it's a newline token
        iter = *self->_private->tokens[self->_private->current];
        type = (ShellTokenType) iter.vptr->getType(&iter);

        // cursor can be at most len - 1
        in_bounds = self->_private->current < self->_private->len;

        return in_bounds && type != TOK_CTRL_NEWLINE;
}

/**
 * @brief Implementation of @c TokenIterator::next().
 * @param self pointer to iterator object
 * @return pointer to next token in iterable
 */
static inline Token *TokenIterator_next_(TokenIterator const * const self)
{
        return self->_private->tokens[self->_private->current++];
}

/**
 * @brief Implementation of @c TokenIterator::peek().
 * @param self pointer to iterator object
 * @param offset how far ahead to peek
 * @return token pointed to by iterator at given offset
 */
static inline Token TokenIterator_peek_(TokenIterator const * const self,
                                        unsigned int offset)
{
        Token *cur;
        Token result;

        // store original index
        size_t orig = self->_private->current;

        unsigned int pos = 0;
        do {
                cur = self->vptr->next(self);
                pos++;
        } while (pos <= offset && self->vptr->has_next(self));

        if (pos <= offset) {
                // TODO: find a better way to return null if possible
                // newline token acts as null terminator for token array
                NewlineToken n;
                NewlineToken_ctor(&n);
                result = *(Token *) &n;
        } else {
                result = *cur;
        }

        self->_private->current = orig;

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
void TokenIterator_ctor(TokenIterator *self, size_t len, Token *tokens[len])
{
        static struct TokenIteratorVtbl const vtbl = {
                .has_next = &TokenIterator_hasNext_,
                .next = &TokenIterator_next_,
                .peek = &TokenIterator_peek_,
        };
        self->vptr = &vtbl;
        self->_private = malloc(sizeof(struct TokenIteratorPrivate));

        self->_private->len = len;
        self->_private->tokens = tokens;
        self->_private->current = 0;
}

void TokenIterator_dtor(TokenIterator *self)
{
        self->vptr = NULL;
        self->_private->len = 0;
        self->_private->tokens = NULL;
        self->_private->current = 0;
        free(self->_private);
        self->_private = NULL;
}
