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

#include "interpreter/token.h"

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
        TokenType tok_type; /**< the token's type */
        char *tok_value; /**< the string value stored by this token */
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
static void token_print_(Token const * const self)
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
static void token_take_(Token const * const self, StringIterator const * const iter)
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
void token_ctor(Token *self, TokenType type)
{
        static struct TokenVtbl const vtbl = {
                .get_type = &token_get_type_,
                .get_value = &token_get_value_,
                .print = &token_print_,
                .set_value = &token_set_value_,
                .take = &token_take_,
        };
        self->vptr = &vtbl;
        self->private = malloc(sizeof(struct TokenPrivate));

        self->private->tok_type = type;
        self->private->tok_value = NULL;
}

void token_dtor(Token *self)
{
        self->vptr = NULL;

        self->private->tok_type = TOK_0;

        free(self->private->tok_value);
        self->private->tok_value = NULL;

        free(self->private);
        self->private = NULL;
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
inline TokenType token_get_type_(const Token *self)
{
        return self->private->tok_type;
}

char *token_get_value_(const Token *self)
{
        if (self->private->tok_value == NULL)
                return NULL;

        char *value = strdup(self->private->tok_value);
        return value;
}

void token_set_value_(const Token *self, const char *value)
{
        // free original value before re-initializing it with new data
        if (self->private->tok_value != NULL) {
                free(self->private->tok_value);
                self->private->tok_value = NULL;
        }

        self->private->tok_value = strdup(value);
}
