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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

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
        const char *string; /**< the string we are iterating over */
        const char *current; /**< cursor position of string */
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

static char *StringIterator_munchChar_(StringIterator const * const self)
{
        // grab character
        const char *start = self->vptr->next(self);

        // copy and return character
        char *slice = self->vptr->slice(self, start);
        return slice;
}

static char *StringIterator_munchWord_(StringIterator const * const self)
{
        // grab word
        const char *start = self->vptr->next(self);
        while (self->vptr->has_next(self) && self->vptr->peek(self) != ' ') {
                self->vptr->next(self);
        }

        // copy and return word
        char *slice = self->vptr->slice(self, start);
        return slice;
}

/**
 * @brief Implementation of @c StringIterator::next().
 * @param self pointer to iterator object
 * @return the character pointed to by the iterator
 */
static inline const char *StringIterator_next_(StringIterator const * const self)
{
        const char *next = self->_private->current;
        self->_private->current++;
        return next;
}

static char *StringIterator_slice_(StringIterator const * const self,
                                   const char *from)
{
        const char *l_bound = self->_private->string;
        const char *r_bound = self->_private->current;

#ifdef DEBUG
        printf("&from=%p, &l_bound=%p, &r_bound=%p\n",
               (void *) from, (void *) l_bound, (void *) r_bound);
#endif

        if (from < l_bound || from >= r_bound) {
                /* error out of bounds */
                return NULL;
        }

        ptrdiff_t len = r_bound - from;

#ifdef DEBUG
        printf("len=%ld\n", len);
#endif

        char *slice = strndup(from, len);
        return slice;
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
                .munchChar = &StringIterator_munchChar_,
                .munchWord = &StringIterator_munchWord_,
                .next = &StringIterator_next_,
                .peek = &StringIterator_peek_,
                .slice = &StringIterator_slice_,
        };
        self->vptr = &vtbl;
        self->_private = malloc(sizeof(struct StringIteratorPrivate));

        self->_private->string = strdup(string);
        self->_private->current = &self->_private->string[0];
}

void StringIterator_dtor(StringIterator *self)
{
        self->vptr = NULL;

        free((char *) self->_private->string);
        self->_private->string = NULL;
        self->_private->current = NULL;

        free(self->_private);
        self->_private = NULL;
}
