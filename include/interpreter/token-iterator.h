/**
 * @file token-iterator.h
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief For iterating over a collection of tokens.
 */
#ifndef SMALLSH_TOKEN_ITERATOR_H
#define SMALLSH_TOKEN_ITERATOR_H

#include <stdlib.h>

#include "shell-tokens.h"

struct TokenIteratorVtbl;
struct TokenIteratorPrivate;

/**
 * @brief @c TokenIterator object definition.
 */
typedef struct {
        struct TokenIteratorVtbl const *vptr; /**< virtual table */
        struct TokenIteratorPrivate *private; /**< private data */
} TokenIterator;

/**
 * @brief VTable for @c TokenIterator object.
 *
 * Iterator methods derived from this source:
 * https://www.youtube.com/watch?v=hnlmbeQa31E
 */
struct TokenIteratorVtbl {
        /**
         * @brief Let's caller know if there are any more tokens left to
         * parse from iterable.
         *
         * <br>
         *
         * The iterable is deemed exhausted under two conditions:
         *
         * <ol>
         * <li>The iterator is pointing at the last token in the iterable.</li>
         * <li>The next token in the iterable is a newline token.</li>
         * </ol>
         * @param self pointer to iterator object
         * @return true if iterator is positioned at a valid token, false
         * otherwise
         */
        bool (*has_next) (TokenIterator const * const self);

        /**
         * @brief Returns the token the iterator is pointing at, then advances
         * to the next token.
         * @pre It is assumed that the iterator can retrieve the next token.
         * Behavior is undefined if this is not the case. Caller should use
         * @c TokenIterator::has_next() to verify before calling this method.
         * @param self pointer to iterator object
         * @return pointer to next token in iterable
         */
        Token *(*next) (TokenIterator const * const self);

        /**
         * @brief Returns the nth token from the iterators cursor, without
         * advancing to it.
         *
         * <br>
         *
         * If @p offset is equal to 0, then the token at the current position
         * of the iterator is retrieved.
         *
         * <br>
         *
         * If @p offset is positive, then the iterator will return the token
         * that is @offset elements ahead of the current cursor.
         *
         * This function is provided to allow for look-ahead parsing by peeking
         * at the next token in the iterator.
         * @param self pointer to iterator object
         * @param offset how far ahead to peek
         * @return the nth character past the iterators cursor, or an empty
         * @c NewlineToken object if no more tokens to seek past
         */
        Token (*peek) (TokenIterator const * const self, unsigned int offset);
};

/**
 * @brief Constructs a new @c TokenIterator object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @pre if @p tokens does not contain exactly @p len tokens ,then @p tokens
 * must be terminated by a newline token to signify the end of the stream.
 * Behavior is undefined if this is not the case.
 * @param self @c TokenIterator object to initialize
 * @param len length of the token array
 * @param tok the token array to iterate over
 * @note No copy is made of @p tokens, so the caller must not free or modify
 * the parameter until they are finished using the iterator. Likewise, the
 * caller cannot guarantee that the tokens array is left unmodified at the end
 * of the iterators lifespan.
 */
void token_iterator_ctor(TokenIterator *self, size_t len, Token *tok[len]);

/**
 * @brief Destroys a @c TokenIterator object, freeing its members and
 * re-initializing them to @c NULL in the process.
 *
 * The exception to this is that the @c tokens array is not freed, as that is
 * up to the caller to perform.
 * @param self The caller is responsible for freeing @p self afterwards if it
 * was originally allocated on the heap.
 */
void token_iterator_dtor(TokenIterator *self);

#endif //SMALLSH_TOKEN_ITERATOR_H
