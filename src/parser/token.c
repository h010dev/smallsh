/**
 * @file token.c
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief For parsing and storing tokens.
 *
 * This is a base class for parsing strings into categorized tokens. This class
 * is not meant to be used on its own, rather it should be inherited and
 * implemented into more specific token classes.
 */
#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "parser/token.h"

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
 * @brief Hides @c Token members from sub-classes and client code.
 */
struct TokenPrivate {
        TokenType type; /**< the token's type */
        char *value; /**< the string value stored by this token */
};

/* *****************************************************************************
 * CLASS METHODS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
/**
 * @brief Virtual method definition for pretty printing a @c Token object.
 * @param self pointer to @c Token object
 */
static void Token_print_(Token const * const self)
{
        (void) self;
        assert(0);
}

/**
 * @brief Virtual method definition for 'taking', or storing a string as
 * part of the token's value.
 *
 * Implementations of this method should define how to parse a string using
 * the @iter iterator in order to retrieve a valid token string to be stored
 * at @c TokenPrivate::value.
 *
 * @param self pointer to @c Token object
 * @param iter pointer to @c StringIterator object
 */
static void Token_take_(Token const * const self,
                        StringIterator const * const iter)
{
        (void) self;
        (void) iter;
        assert(0);
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
void Token_ctor(Token * const self, TokenType type)
{
        static struct TokenVtbl const vtbl = {
                .getType = &Token_getType_,
                .getValue = &Token_getValue_,
                .print = &Token_print_,
                .setValue = &Token_setValue_,
                .take = &Token_take_,
        };
        self->vptr = &vtbl;
        self->_private = malloc(sizeof(struct TokenPrivate));

        self->_private->type = type;
        self->_private->value = NULL;
}

void Token_dtor(Token *self)
{
        self->vptr = NULL;

        self->_private->type = TOK_0;

        free(self->_private->value);
        self->_private->value = NULL;

        free(self->_private);
        self->_private = NULL;
}

/* *****************************************************************************
 * CLASS METHODS
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/
inline TokenType Token_getType_(Token const * const self)
{
        return self->_private->type;
}

char *Token_getValue_(Token const * const self)
{
        if (self->_private->value == NULL)
                return NULL;

        char *value = strdup(self->_private->value);
        return value;
}

void Token_setValue_(Token const * const self, const char * const value)
{
        // free original value before re-initializing it with new data
        if (self->_private->value != NULL) {
                free(self->_private->value);
                self->_private->value = NULL;
        }

        self->_private->value = strdup(value);
}
