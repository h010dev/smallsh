/**
 * @file string-iterator.c
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief For iterating over a single-line string.
 *
 * This is a utility module for aiding in parsing strings into tokens. The
 * functions defined here allow for iterating over a string character by
 * character, until either a newline is reached or null terminator.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#include "string-iterator.h"

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
 * @brief Hides @c StringIterator members from client code.
 */
struct StringIteratorPrivate {
        char *string; /**< the string we are iterating over */
        char *current; /**< cursor position of string */
};

/**
 * @brief Implementation of @c StringIterator::has_next().
 * @param self pointer to iterator object
 * @return true if iterator is positioned at parseable character, false
 * otherwise
 */
static inline bool StringIterator_hasNext_(StringIterator const * const self)
{
        char itr = *self->_private->current;
        return itr != '\n' && itr != '\0';
}

/**
 * @brief Implementation of @c StringIterator::next().
 * @param self pointer to iterator object
 * @return the character pointed to by the iterator
 */
static inline char StringIterator_next_(StringIterator const * const self)
{
        char next = *self->_private->current;
        self->_private->current++;
        return next;
}

/**
 * @brief Implementation of @c StringIterator::peek().
 * @param self pointer to iterator object
 * @return the character pointed to by the iterator
 */
static inline char StringIterator_peek_(StringIterator const * const self)
{
        return *self->_private->current;
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
void StringIterator_ctor(StringIterator * const self, char *string)
{
        static struct StringIteratorVtbl const vtbl = {
                .has_next = &StringIterator_hasNext_,
                .next = &StringIterator_next_,
                .peek = &StringIterator_peek_,
        };
        self->vptr = &vtbl;
        self->_private = malloc(sizeof(struct StringIteratorPrivate));

        self->_private->string = strdup(string);
        self->_private->current = &self->_private->string[0];
}

void StringIterator_dtor(StringIterator *self)
{
        self->vptr = NULL;

        free(self->_private->string);
        self->_private->string = NULL;
        self->_private->current = NULL;

        free(self->_private);
        self->_private = NULL;
}
