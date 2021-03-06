/**
 * @file test-string-iterator.c
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief Unit test module for string-iterator.c.
 */
#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdlib.h>

#include <string.h>

#include "utils/string-iterator.h"

struct StringIteratorPrivate {
        const char *si_str;
        const char *si_cur;
};

static void string_iterator_ctor_test_initializesValues(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;

        string_iterator_ctor(&itr, string);

        assert_string_equal(string, itr.private->si_str);
        assert_int_equal(string[0], *itr.private->si_cur);
}

static void string_iterator_dtor_test_deletesData(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;

        string_iterator_ctor(&itr, string);
        string_iterator_dtor(&itr);

        assert_null(itr.vptr);
        assert_null(itr.private);
}

static void string_iterator_next_test_advancesCurrent(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        const char *cur;

        string_iterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                cur = itr.vptr->next(&itr);
                assert_int_equal(string[i], cur[0]);
        }
}

static void string_iterator_peek_test_returnsCurrent(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        char cur;

        string_iterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                cur = itr.vptr->peek(&itr, 0);
                assert_int_equal(string[i], cur);
                itr.vptr->next(&itr);
        }
}

static void string_iterator_peek_test_returnsOffset(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        char cur;

        string_iterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                cur = itr.vptr->peek(&itr, i);
                assert_int_equal(string[i], cur);
        }

        // cursor should remain at original position
        assert_int_equal('h', itr.vptr->peek(&itr, 0));
}

static void string_iterator_peek_test_EOL(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        char cur;

        string_iterator_ctor(&itr, string);

        // go to last valid character
        for (size_t i = 0; i < len - 1; i++) {
                itr.vptr->next(&itr);
        }

        // cur should be valid
        assert_int_equal('d', itr.vptr->peek(&itr, 0));

        // next should not be valid
        for (size_t i = 1; i < 5; i++) {
                cur = itr.vptr->peek(&itr, i);
                assert_int_equal(STRING_ITERATOR_EOL, cur);
        }

        // cursor should remain at original position
        assert_int_equal('d', itr.vptr->peek(&itr, 0));
}

static void string_iterator_has_next_test_detectsNullTerminator(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        bool has_next;

        string_iterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                has_next = itr.vptr->has_next(&itr);
                assert_true(has_next);
                itr.vptr->next(&itr);
        }
        has_next = itr.vptr->has_next(&itr);
        assert_false(has_next);
}

static void string_iterator_slice_test_handlesOutOfBounds(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;
        const char *from;
        char *slice;

        string_iterator_ctor(&itr, string);

        // slice when cursor at start of string should fail
        from = itr.private->si_cur;
        slice = itr.vptr->slice(&itr, from);
        assert_null(slice);

        // slice when from is before string start should fail
        from = itr.private->si_cur;
        slice = itr.vptr->slice(&itr, --from);
        assert_null(slice);

        // slice when from is at cursor should fail
        from = itr.vptr->next(&itr);
        slice = itr.vptr->slice(&itr, ++from);
        assert_null(slice);

        // slice when from is past cursor should fail
        (void) itr.vptr->next(&itr);
        from = itr.private->si_cur;
        slice = itr.vptr->slice(&itr, ++from);
        assert_null(slice);
}

static void string_iterator_slice_test_returnsSlice(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;
        const char *from, *start;
        char *slice;

        string_iterator_ctor(&itr, string);

        // slice of first char in string should work
        start = itr.vptr->next(&itr);
        slice = itr.vptr->slice(&itr, start);
        assert_non_null(slice);
        assert_string_equal("h", slice);
        free(slice);

        // slice of single char in middle of string should work
        for (int i = 0; i < 4; i++) {
                from = itr.vptr->next(&itr);
        }
        slice = itr.vptr->slice(&itr, from);
        assert_non_null(slice);
        assert_string_equal("o", slice);
        free(slice);

        // slice of single char at end - 1 should work
        for (int i = 0; i < 6; i++) {
                from = itr.vptr->next(&itr);
        }
        slice = itr.vptr->slice(&itr, from);
        assert_non_null(slice);
        assert_string_equal("d", slice);
        free(slice);

        // slice of multi-char string should work
        slice = itr.vptr->slice(&itr, start);
        assert_non_null(slice);
        assert_string_equal("hello world", slice);
        free(slice);
}

static void string_iterator_test_munch_char_beforeEOL(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#";
        char sym = '#';
        char *slice;

        // iterate to character
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munch_char(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal('\0', *iter.vptr->next(&iter));
}

static void string_iterator_test_munch_char_beforeNewline(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#\n";
        char sym = '#';
        char *slice;

        // iterate to character
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munch_char(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal('\n', *iter.vptr->next(&iter));
}

static void string_iterator_test_munch_char_loneChar(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "# hello world";
        char sym = '#';
        char *slice;

        // iterate to character
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munch_char(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal(' ', *iter.vptr->next(&iter));
}

static void string_iterator_test_munch_char_followingChars(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#hello world";
        char sym = '#';
        char *slice;

        // iterate to character
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munch_char(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal('h', *iter.vptr->next(&iter));
}

static void string_iterator_test_munch_word_beforeEOL(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd";
        char *slice;

        string_iterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munch_word(&iter);
        assert_string_equal("cmd", slice);

        // iter should point at end of word
        assert_int_equal('\0', *iter.vptr->next(&iter));
}

static void string_iterator_test_munch_word_followingWords(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt &";
        char *slice;

        string_iterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munch_word(&iter);
        assert_string_equal("cmd", slice);

        // iter should point at end of word
        assert_int_equal(' ', *iter.vptr->next(&iter));
}

static void string_iterator_test_munch_word_singleChar(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "c > file1.txt &";
        char *slice;

        string_iterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munch_word(&iter);
        assert_string_equal("c", slice);

        // iter should point at end of word
        assert_int_equal(' ', *iter.vptr->next(&iter));
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        string_iterator_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_next_test_advancesCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_peek_test_returnsCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_peek_test_returnsOffset, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_peek_test_EOL, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_has_next_test_detectsNullTerminator, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_slice_test_handlesOutOfBounds, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_slice_test_returnsSlice, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_char_beforeEOL, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_char_beforeNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_char_loneChar, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_char_followingChars, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_word_beforeEOL, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_word_followingWords, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        string_iterator_test_munch_word_singleChar, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
