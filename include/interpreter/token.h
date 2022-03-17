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

#include "utils/string-iterator.h"

struct TokenVtbl;
struct TokenPrivate;

/**
 * @brief Defines the base token type (class).
 */
typedef enum {
        TOK_0 = 0, /**< placeholder token type */
        TOK_CMT = 1, /**< a comment is identified by '#' at the start of the stream */
        TOK_CTRL_BG = 2, /**< a background is identified by '&' at the end of the stream */
        TOK_CTRL_NEWLINE = 3, /**< a newline is identified by '\\n' */
        TOK_REDIR_INPUT = 4, /**< an input redirection is a '<' to redirect file io */
        TOK_REDIR_OUTPUT = 5, /**< an output redirection is a '>' to redirect file io */
        TOK_WORD = 6, /**< a basic token is any word */
        TOK_COUNT = 7, /**< count of tokens to allow iterating over them */
} SH_TokenType;

/**
 * @brief Defines a basic token object that has a type and value.
 */
typedef struct {
        SH_TokenType type; /**< the token's type */
        char *value; /**< the string value stored by this token */
} SH_Token;

/**
 * @brief Returns copy of the string stored by the token.
 * @param token pointer to @c Token object
 * @return copy of string stored by @p token
 * @note The caller is responsible for freeing the return value when
 * they're done using it.
 */
char *SH_GetTokenValue(SH_Token const *token);

/**
 * @brief Prints a pretty-formatted version of @p token.
 * @param token pointer to @c Token object
 */
void SH_PrintToken(SH_Token const *token);

/**
 * @brief Copies @p value into @p self's value field.
 * @param token pointer to @c Token object
 * @param value the string to copy to @p token
 */
void SH_SetTokenValue(SH_Token *token, char const *value);

/**
 * @brief Virtual method definition for 'taking', or storing a string as
 * part of the token's value.
 *
 * <br>
 *
 * Implementations of this method should define how to parse a string
 * using the @iter iterator in order to retrieve a valid token string to
 * be stored at @c TokenPrivate::value.
 * @param token pointer to @c Token object
 * @param iter pointer to @c StringIterator object
 */
void SH_TakeToken(SH_Token *token, SH_StringIterator *iter);

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
 * @param type @c TokenType to to assign to @p self
 */
SH_Token *SH_CreateToken(SH_TokenType type);

/**
 * @brief Destroys a @c Token object, freeing its members and re-initializing
 * them to @c NULL in the process.
 *
 * <br>
 *
 * This is a base class destructor and should only be called via a child class
 * destructor.
 * @param token pointer to @c Token object
 */
void SH_DestroyToken(SH_Token **token);

#endif //SMALLSH_TOKEN_H
