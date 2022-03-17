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

#define STRING_ITERATOR_EOL '\0' /**< signified end of line for iterator */

typedef struct {
        char *string; /**< the string we are iterating over */
        char *cur; /**< cursor position of string */
} SH_StringIterator;

/**
 * @brief Constructs a new @c StringIterator object.
 *
 * The general implementation of a vtable, usage of const for the main object,
 * and other OOP concepts were derived from this publication.
 *
 * <br><br>
 *
 * Source: https://www.state-machine.com/doc/AN_OOP_in_C.pdf*
 * @param str the string to iterate over
 * @note A copy is made of @p string, so the caller can free/overwrite the
 * string safely after calling this constructor. Note this also means the
 * caller must destroy the data via calling the @c SH_DestroyStringIterator function.
 */
SH_StringIterator *SH_CreateStringIterator(char *str);

/**
 * @brief Destroys a @c StringIterator object, freeing its members and
 * re-initializing them to @c NULL in the process.
 * @param iter the @c StringIterator object to destroy
 * @note The caller is responsible for freeing @p self afterwards if it was
 * originally allocated on the heap.
 */
void SH_DestroyStringIterator(SH_StringIterator **iter);

/**
 * @brief Iterates over a single character and returns copy to caller.
 *
 * Iterator is updated to point to immediate character after end of
 * copied character.
 * @param iter pointer to iterator object
 * @return string containing copied character
 * @note The caller is responsible for freeing the return value after
 * they are done using it.
 */
char *SH_StringIteratorConsumeChar(SH_StringIterator *iter);

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
 * @param iter pointer to iterator object
 * @return string containing copied word
 * @note The caller is responsible for freeing the return value after
 * they are done using it.
 */
char *SH_StringIteratorConsumeWord(SH_StringIterator *iter);

/**
 * @brief Let's caller know if there are any more characters left to
 * parse from internal string.
 *
 * If the current iterator is positioned at a null terminator ('\\0')
 * character, then it is assumed that the iterator has been exhausted.
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
 * @pre It is assumed that the iterator is always within bounds of the
 * string it is iterating. Behavior is undefined if this is not the case.
 * @param iter pointer to iterator object
 * @return true if iterator is positioned at a parseable character,
 * false otherwise
 */
bool SH_StringIteratorHasNext(SH_StringIterator const *iter);

/**
 * @brief Returns character that the iterator is pointing at, then
 * advances to the next character.
 * @pre It is assumed that the iterator is always within bounds of the
 * string it is iterating. Behavior is undefined if this is not the case.
 * @param iter pointer to iterator object
 * @return pointer to the character pointed to by the iterator
 */
char *SH_StringIteratorNext(SH_StringIterator *iter);

/**
 * @brief Returns the nth character from the iterators cursor, without
 * advancing to the it.
 *
 * <br>
 *
 * If @p offset is equal to 0, then the character at the current
 * cursor is retrieved.
 *
 * <br>
 *
 * If @p offset is positive, then the iterator peaks ahead of the cursor.
 *
 * <br>
 *
 * This function is provided to allow for look-ahead parsing by peeking
 * at the next character in the iterator.
 * @pre It is assumed that the iterator is always within bounds of the
 * string it is iterating. Behavior is undefined if this is not the case.
 * @param iter pointer to iterator object
 * @param offset how far ahead to peak
 * @return the nth character past the iterators cursor, or EOL if no
 * more characters to seek past
 */
char SH_StringIteratorPeek(SH_StringIterator *iter, unsigned int offset);

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
char *SH_StringIteratorSlice(SH_StringIterator *self, char const *from);

#endif //SMALLSH_STRING_ITERATOR_H
