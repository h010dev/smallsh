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
#include <cmocka.h>
#include <stdlib.h>

#include "string-iterator.c"

static void StringIterator_ctor_initializesValues(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;

        StringIterator_ctor(&itr, string);

        assert_string_equal(string, itr._private->string);
        assert_int_equal(string[0], *itr._private->current);
}

static void StringIterator_dtor_test_deletesData(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;

        StringIterator_ctor(&itr, string);
        StringIterator_dtor(&itr);

        assert_null(itr.vptr);
        assert_null(itr._private);
}

static void StringIterator_next_test_advancesCurrent(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        const char *cur;

        StringIterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                cur = itr.vptr->next(&itr);
                assert_int_equal(string[i], cur[0]);
        }
}

static void StringIterator_peek_test_returnsCurrent(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        char cur;

        StringIterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                cur = itr.vptr->peek(&itr);
                assert_int_equal(string[i], cur);
                itr.vptr->next(&itr);
        }
}

static void StringIterator_hasNext_test_detectsNullTerminator(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        bool has_next;

        StringIterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                has_next = itr.vptr->has_next(&itr);
                assert_true(has_next);
                itr.vptr->next(&itr);
        }
        has_next = itr.vptr->has_next(&itr);
        assert_false(has_next);
}

static void StringIterator_hasNext_test_detectsNewline(void **state)
{
        (void) state;
        char *string = "hello \nworld";
        size_t len = 6; /* newline pos */
        StringIterator itr;
        bool has_next;

        StringIterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                has_next = itr.vptr->has_next(&itr);
                assert_true(has_next);
                itr.vptr->next(&itr);
        }
        has_next = itr.vptr->has_next(&itr);
        assert_false(has_next);
}

static void StringIterator_slice_test_handlesOutOfBounds(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;
        const char *from;
        char *slice;

        StringIterator_ctor(&itr, string);

        // slice when cursor at start of string should fail
        from = itr._private->current;
        slice = itr.vptr->slice(&itr, from);
        assert_null(slice);

        // slice when from is before string start should fail
        from = itr._private->current;
        slice = itr.vptr->slice(&itr, --from);
        assert_null(slice);

        // slice when from is at cursor should fail
        from = itr.vptr->next(&itr);
        slice = itr.vptr->slice(&itr, ++from);
        assert_null(slice);

        // slice when from is past cursor should fail
        (void) itr.vptr->next(&itr);
        from = itr._private->current;
        slice = itr.vptr->slice(&itr, ++from);
        assert_null(slice);
}

static void StringIterator_slice_test_returnsSlice(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;
        const char *from, *start;
        char *slice;

        StringIterator_ctor(&itr, string);

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

static void StringIterator_munchChar_beforeEOL(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#";
        char sym = '#';
        char *slice;

        // iterate to character
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munchChar(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal('\0', *iter.vptr->next(&iter));
}

static void StringIterator_munchChar_beforeNewline(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#\n";
        char sym = '#';
        char *slice;

        // iterate to character
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munchChar(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal('\n', *iter.vptr->next(&iter));
}

static void StringIterator_munchChar_loneChar(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "# hello world";
        char sym = '#';
        char *slice;

        // iterate to character
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munchChar(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal(' ', *iter.vptr->next(&iter));
}

static void StringIterator_munchChar_followingChars(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#hello world";
        char sym = '#';
        char *slice;

        // iterate to character
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // eat character
        slice = iter.vptr->munchChar(&iter);
        assert_string_equal("#", slice);

        // iter should point at end of character
        assert_int_equal('h', *iter.vptr->next(&iter));
}

static void StringIterator_munchWord_beforeEOL(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd";
        char *slice;

        StringIterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munchWord(&iter);
        assert_string_equal("cmd", slice);

        // iter should point at end of word
        assert_int_equal('\0', *iter.vptr->next(&iter));
}

static void StringIterator_munchWord_beforeNewline(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd\n";
        char *slice;

        StringIterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munchWord(&iter);
        assert_string_equal("cmd", slice);

        // iter should point at end of word
        assert_int_equal('\n', *iter.vptr->next(&iter));
}

static void StringIterator_munchWord_followingWords(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt &";
        char *slice;

        StringIterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munchWord(&iter);
        assert_string_equal("cmd", slice);

        // iter should point at end of word
        assert_int_equal(' ', *iter.vptr->next(&iter));
}

static void StringIterator_munchWord_singleChar(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "c > file1.txt &";
        char *slice;

        StringIterator_ctor(&iter, string);

        // eat word
        slice = iter.vptr->munchWord(&iter);
        assert_string_equal("c", slice);

        // iter should point at end of word
        assert_int_equal(' ', *iter.vptr->next(&iter));
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        StringIterator_ctor_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_next_test_advancesCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_peek_test_returnsCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_hasNext_test_detectsNullTerminator, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_hasNext_test_detectsNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_slice_test_handlesOutOfBounds, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_slice_test_returnsSlice, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchChar_beforeEOL, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchChar_beforeNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchChar_loneChar, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchChar_followingChars, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchWord_beforeEOL, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchWord_beforeNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchWord_followingWords, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_munchWord_singleChar, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
