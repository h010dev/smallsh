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
bool SH_StringIteratorHasNext(SH_StringIterator const * const iter)
{
        char itr = *iter->cur;
        return itr != '\0';
}

char *SH_StringIteratorConsumeChar(SH_StringIterator *const iter)
{
        // grab character
        char *start = SH_StringIteratorNext(iter);

        // copy and return character
        char *slice = SH_StringIteratorSlice(iter, start);
        return slice;
}

char *SH_StringIteratorConsumeWord(SH_StringIterator *const iter)
{
        // grab word
        char *start = SH_StringIteratorNext(iter);
        char *slice;
        char c;
        while (SH_StringIteratorHasNext(iter)) {
                c = SH_StringIteratorPeek(iter, 0);
                switch (c) {
                        // stop at terminal character
                        case ' ':
                        case '\t':
                        case '\n':
                                goto seek_fin;
                        default:
                                break;
                }
                SH_StringIteratorNext(iter);
        }

seek_fin:
        // copy and return word
        slice = SH_StringIteratorSlice(iter, start);
        return slice;
}

char *SH_StringIteratorNext(SH_StringIterator *const iter)
{
        char *next = iter->cur++;
        return next;
}

char *SH_StringIteratorSlice(SH_StringIterator *const iter, char const *from)
{
        char *l_bound = iter->string;
        char *r_bound = iter->cur;

        if (from < l_bound || from >= r_bound) {
                /* error out of bounds */
                return NULL;
        }

        ptrdiff_t len = r_bound - from;

        char *slice = strndup(from, len);
        return slice;
}

char SH_StringIteratorPeek(SH_StringIterator *const iter, unsigned int offset)
{
        char *orig, *cur;
        char result;
        unsigned int pos;

        // store pointer to originating character
        orig = iter->cur;

        // seek to offset or end of stream, whichever comes first
        pos = 0;
        do {
                cur = SH_StringIteratorNext(iter);
                pos++;
        } while (pos <= offset && SH_StringIteratorHasNext(iter));

        // check if iterator exhausted before reaching offset
        if (pos <= offset)
                result = STRING_ITERATOR_EOL;
        else
                result = *cur;

        // reset pointer to originating character
        iter->cur = orig;

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
SH_StringIterator *SH_CreateStringIterator(char *str)
{
        SH_StringIterator *iter;

        iter = malloc(sizeof *iter);
        if (iter == NULL) {
                return NULL;
        }

        iter->string = strdup(str);
        iter->cur = &iter->string[0];

        return iter;
}

void SH_DestroyStringIterator(SH_StringIterator **iter)
{
        if (*iter) {
                free((*iter)->string);
                (*iter)->string = NULL;
                (*iter)->cur = NULL;

                free(*iter);
                *iter = NULL;
        }
}
