/**
 * @file string-iterator.h
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief For iterating over a single-line string.
 *
 * This is a utility module for aiding in parsing strings into tokens. The
 * functions defined here allow for iterating over a string character by
 * character, until either a newline is reached or null terminator.
 */
#ifndef SMALLSH_STRING_ITERATOR_H
#define SMALLSH_STRING_ITERATOR_H

#include <stdbool.h>

struct StringIteratorVtbl;
struct StringIteratorPrivate;

/**
 * @brief @c StringIterator object definition.
 */
typedef struct {
        struct StringIteratorVtbl const *vptr; /**< virtual table */
        struct StringIteratorPrivate *_private; /**< private data */
} StringIterator;

/**
 * @brief VTable for a @c StringIterator object.
 *
 * Iterator methods derived from this source:
 * https://www.youtube.com/watch?v=hnlmbeQa31E
 */
struct StringIteratorVtbl {
        /**
         * @brief Let's caller know if there are any more characters left to
         * parse from internal string.
         *
         * If the current iterator is positioned at a newline ('\\n') or null
         * ('\\0') character, then it is assumed that the iterator has been
         * exhausted.
         *
         * Note this means that multi-line strings cannot be parsed
         * by this iterator without failing prematurely.
         *
         * @example
         * <br>
         *
         * <pre>string = "hello world\\0"</pre>
         * <pre>cursor:            ^     </pre>
         *
         * <br>
         *
         * In this case, the cursor is at 'l'. The next letter is not '\\0' or
         * '\\n' so it returns true.
         *
         * <br>
         *
         * <pre>string = "hello world\\0"</pre>
         * <pre>cursor:             ^    </pre>
         *
         * <br>
         *
         * In this case, the cursor is at 'd'. The next letter is '\\0' so it
         * returns false.
         *
         * <br>
         *
         * <pre>string = "hello world\\n goodbye world\\0"</pre>
         * <pre>cursor:             ^                     </pre>
         *
         * <br>
         *
         * In this case, the cursor is at 'd'. The next letter is '\\n' so it
         * returns false, even though the string has technically not been
         * exhausted.
         *
         * @pre It is assumed that the iterator is always within bounds of the
         * string it is iterating. Behavior is undefined if this is not the case.
         * @param self pointer to iterator object
         * @return true if iterator is positioned at a parseable character,
         * false otherwise
         */
        bool (*has_next) (StringIterator const * const self);

        /**
         * @brief Iterates over a single character and returns copy to caller.
         *
         * Iterator is updated to point to immediate character after end of
         * copied character.
         * @param self pointer to iterator object
         * @return string containing copied character
         * @note The caller is responsible for freeing the return value after
         * they are done using it.
         */
        char *(*munchChar) (StringIterator const * const self);

        /**
         * @brief Iterates over a single word and returns copy to caller.
         *
         * <br>
         *
         * A word is any sequence of characters, excluding a space character.
         *
         * <br><br>
         *
         * Iterator is updated to point to immediate character after end of
         * copied word.
         * @param self pointer to iterator object
         * @return string containing copied word
         * @note The caller is responsible for freeing the return value after
         * they are done using it.
         */
        char *(*munchWord) (StringIterator const * const self);

        /**
         * @brief Returns character that the iterator is pointing at, then
         * advances to the next character.
         * @pre It is assumed that the iterator is always within bounds of the
         * string it is iterating. Behavior is undefined if this is not the case.
         * @param self pointer to iterator object
         * @return pointer to the character pointed to by the iterator
         */
        const char *(*next) (StringIterator const * const self);

        /**
         * @brief Returns character that iterator is pointing at, without
         * advancing to the next character.
         *
         * This function is provided to allow for look-ahead parsing by peeking
         * at the next character in the iterator.
         *
         * @pre It is assumed that the iterator is always within bounds of the
         * string it is iterating. Behavior is undefined if this is not the case.
         * @param self pointer to iterator object
         * @return the character pointed to by the iterator
         */
        char (*peek) (StringIterator const * const self);

        /**
         * @brief Returns a slice copied from the pointer starting at @p from
         * and ending at the iterators current position - 1.
         *
         * @pre @p from must point to a valid address within the iterators
         * internal string. This value should be retrieved by a prior call to
         * @c StringIterator::next(). This function will perform bounds checking
         * to verify the above is true, and will return @c NULL if it is not.
         * @param self pointer to iterator object
         * @param from pointer to a position within iterator string
         * @return a slice of the iterator string on success, @c NULL on failure
         * @note The caller is responsible for freeing the slice after they're
         * done using it.
         */
        char *(*slice) (StringIterator const * const self, const char *from);
};

/**
 * @brief Constructs a new @c StringIterator object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param self @c StringIterator object to initialize
 * @param string the string to iterate over
 * @note A copy is made of @p string, so the caller can free/overwrite the
 * string safely after calling this constructor. Note this also means the
 * caller must destroy the data via calling the @c StringIterator_dtor function.
 */
void StringIterator_ctor(StringIterator *self, char *string);

/**
 * @brief Destroys a @c StringIterator object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param self the @c StringIterator object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void StringIterator_dtor(StringIterator *self);

#endif //SMALLSH_STRING_ITERATOR_H
