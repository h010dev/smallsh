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

#include "utils/string-iterator.h"

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
        const char *si_str; /**< the string we are iterating over */
        const char *si_cur; /**< cursor position of string */
};

/**
 * @brief Implementation of @c StringIterator::has_next().
 * @param self pointer to iterator object
 * @return true if iterator is positioned at parseable character, false
 * otherwise
 */
static inline bool string_iterator_has_next_(StringIterator const * const self)
{
        char itr = *self->private->si_cur;
        return itr != '\0';
}

/**
 * @brief Implementation of @c StringIterator::munchChar().
 * @param self pointer to iterator object
 * @return string containing copied character
 */
static char *string_iterator_munch_char_(StringIterator const * const self)
{
        // grab character
        const char *start = self->vptr->next(self);

        // copy and return character
        char *slice = self->vptr->slice(self, start);
        return slice;
}

/**
 * @brief Implementation of @c StringIterator::munchWord().
 * @param self pointer to iterator object
 * @return string containing copied word
 */
static char *string_iterator_munch_word_(StringIterator const * const self)
{
        // grab word
        const char *start = self->vptr->next(self);
        char *slice;
        char c;
        while (self->vptr->has_next(self)) {
                c = self->vptr->peek(self, 0);
                switch (c) {
                        // stop at terminal character
                        case ' ':
                        case '\t':
                        case '\n':
                                goto seek_fin;
                        default:
                                break;
                }
                self->vptr->next(self);
        }

seek_fin:
        // copy and return word
        slice = self->vptr->slice(self, start);
        return slice;
}

/**
 * @brief Implementation of @c StringIterator::next().
 * @param self pointer to iterator object
 * @return the character pointed to by the iterator
 */
static inline const char *string_iterator_next_(StringIterator const * const self)
{
        const char *next = self->private->si_cur++;
        return next;
}

static char *string_iterator_slice_(StringIterator const * const self, const char *from)
{
        const char *l_bound = self->private->si_str;
        const char *r_bound = self->private->si_cur;

        if (from < l_bound || from >= r_bound) {
                /* error out of bounds */
                return NULL;
        }

        ptrdiff_t len = r_bound - from;

        char *slice = strndup(from, len);
        return slice;
}

/**
 * @brief Implementation of @c StringIterator::peek().
 * @param self pointer to iterator object
 * @param offset how far ahead to peek
 * @return the character pointed to by the iterator
 */
static inline char string_iterator_peek_(StringIterator const * const self, unsigned int offset)
{
        const char *orig, *cur;
        char result;
        unsigned int pos;

        // store pointer to originating character
        orig = self->private->si_cur;

        // seek to offset or end of stream, whichever comes first
        pos = 0;
        do {
                cur = self->vptr->next(self);
                pos++;
        } while (pos <= offset && self->vptr->has_next(self));

        // check if iterator exhausted before reaching offset
        if (pos <= offset)
                result = STRING_ITERATOR_EOL;
        else
                result = *cur;

        // reset pointer to originating character
        self->private->si_cur = orig;

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
void string_iterator_ctor(StringIterator *self, char *str)
{
        static struct StringIteratorVtbl const vtbl = {
                .has_next = &string_iterator_has_next_,
                .munch_char = &string_iterator_munch_char_,
                .munch_word = &string_iterator_munch_word_,
                .next = &string_iterator_next_,
                .peek = &string_iterator_peek_,
                .slice = &string_iterator_slice_,
        };
        self->vptr = &vtbl;
        self->private = malloc(sizeof(struct StringIteratorPrivate));

        self->private->si_str = strdup(str);
        self->private->si_cur = &self->private->si_str[0];
}

void string_iterator_dtor(StringIterator *self)
{
        self->vptr = NULL;

        free((char *) self->private->si_str);
        self->private->si_str = NULL;
        self->private->si_cur = NULL;

        free(self->private);
        self->private = NULL;
}
