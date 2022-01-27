/**
 * @file token.h
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief Definitions for a token object.
 *
 * Inspiration from:
 * https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 */
#ifndef SMALLSH_TOKEN_H
#define SMALLSH_TOKEN_H

#include "string-iterator.h"

struct TokenVtbl;
struct TokenPrivate;

/**
 * @brief Defines a basic token object that has a type and value.
 */
typedef struct {
        struct TokenVtbl const *vptr; /**< virtual table */
        struct TokenPrivate *_private; /**< private data */
} Token;

/**
 * @brief Defines the base token type (class).
 */
typedef enum {
        TOKEN_0 = 0, /**< placeholder token type */
} TokenType;

/**
 * @brief VTable for a @c Token object.
 *
 * <br>
 *
 * @c take method derived from this source:
 * https://www.youtube.com/watch?v=ikw4V_x-CFI
 */
struct TokenVtbl {
        /**
         * @brief Returns the token's type identifier.
         * @param self pointer to @c Token object
         * @return the token's type identifier
         */
        TokenType (*getType) (Token const * const self);

        /**
         * @brief Returns copy of the string stored by the token.
         * @param self pointer to @c Token object
         * @return copy of string stored by @p self
         * @note The caller is responsible for freeing the return value when
         * they're done using it.
         */
        char *(*getValue) (Token const * const self);

        /**
         * @brief Prints a pretty-formatted version of @p self.
         * @param self pointer to @c Token object
         */
        void (*print) (Token const * const self);

        /**
         * @brief Copies @p value into @p self's value field.
         * @param self pointer to @c Token object
         * @param value the string to copy to @p self
         */
        void (*setValue) (Token const * const self, const char * const value);

        /**
         * @brief Virtual method definition for 'taking', or storing a string as
         * part of the token's value.
         *
         * <br>
         *
         * Implementations of this method should define how to parse a string
         * using the @iter iterator in order to retrieve a valid token string to
         * be stored at @c TokenPrivate::value.
         * @param self pointer to @c Token object
         * @param iter pointer to @c StringIterator object
         */
        void (*take) (Token const * const self, StringIterator const * const iter);
};

/**
 * @brief Constructs a new @c Token object.
 *
 * <br>
 *
 * This is a base class constructor and should only be called via a child class
 * constructor.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c Token object to initialize
 * @param type @c TokenType to to assign to @p self
 */
void Token_ctor(Token *self, TokenType type);

/**
 * @brief Destroys a @c Token object, freeing its members and re-initializing
 * them to @c NULL in the process.
 *
 * <br>
 *
 * This is a base class destructor and should only be called via a child class
 * destructor.
 * @param self pointer to @c Token object
 */
void Token_dtor(Token *self);

/**
 * @brief Implementation of @c Token::getType().
 * @param self pointer to @c Token object
 * @return the token's type identifier
 */
TokenType Token_getType_(Token const *self);

/**
 * @brief Implementation of @c Token::getValue().
 * @param self pointer to @c Token object
 * @return copy of string stored by @p self
 * @note The caller is responsible for freeing the return value when they're
 * done using it.
 */
char *Token_getValue_(Token const *self);

/**
 * @brief Implementation of @c Token::setValue().
 * @param self pointer to @c Token object
 * @param value the string to copy to @p self
 */
void Token_setValue_(Token const *self, const char *value);

#endif //SMALLSH_TOKEN_H
