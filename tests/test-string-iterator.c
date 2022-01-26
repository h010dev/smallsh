/**
 * @file test-string-iterator.c
 * @author Mohamed Al-Hussein
 * @date 25 Jan 2022
 * @brief Unit test module for string-iterator.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "string-iterator.c"

static void StringIterator_ctor_initializes_values(void **state)
{
        (void) state;
        char *string = "hello world";
        StringIterator itr;

        StringIterator_ctor(&itr, string);

        assert_string_equal(string, itr._private->string);
        assert_int_equal(string[0], *itr._private->current);
}

static void StringIterator_next_test_advancesCurrent(void **state)
{
        (void) state;
        char *string = "hello world";
        size_t len = strlen(string);
        StringIterator itr;
        char cur;

        StringIterator_ctor(&itr, string);

        for (size_t i = 0; i < len; i++) {
                cur = itr.vptr->next(&itr);
                assert_int_equal(string[i], cur);
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

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        StringIterator_ctor_initializes_values, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_next_test_advancesCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_peek_test_returnsCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_hasNext_test_detectsNullTerminator, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_hasNext_test_detectsNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        StringIterator_dtor_test_deletesData, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
