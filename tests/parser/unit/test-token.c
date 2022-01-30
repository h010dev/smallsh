#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdlib.h>

#include "parser/token.h"

struct TokenPrivate {
        TokenType type;
        char *value;
};

static void Token_ctor_test_initializesValues(void **state)
{
        (void) state;
        Token tok;

        Token_ctor(&tok, TOK_0);

        assert_int_equal(TOK_0, tok._private->type);
        assert_null(tok._private->value);
}

static void Token_dtor_test_deletesData(void **state)
{
        (void) state;
        Token tok;

        Token_ctor(&tok, TOK_0);
        Token_dtor(&tok);

        assert_null(tok.vptr);
        assert_null(tok._private);
}

static void Token_setValue_test_valueCopy(void **state)
{
        (void) state;
        Token tok;
        char *value = "hello world";

        Token_ctor(&tok, TOK_0);
        tok.vptr->setValue(&tok, value);

        assert_ptr_not_equal(&value, &tok._private->value);
        assert_string_equal(value, tok._private->value);
}

static void Token_setValue_test_overwriteOrig(void **state)
{
        (void) state;
        Token tok;
        char *orig = "hello world";
        char *new = "goodbye world";

        Token_ctor(&tok, TOK_0);
        tok.vptr->setValue(&tok, orig);

        tok.vptr->setValue(&tok, new);
        assert_ptr_not_equal(&new, &tok._private->value);
        assert_string_equal(new, tok._private->value);
}

static void Token_getType_test_valid(void **state)
{
        (void) state;
        Token tok;

        Token_ctor(&tok, TOK_0);
        assert_int_equal(TOK_0, tok.vptr->getValue(&tok));
}

static void Token_getValue_test_nullValue(void **state)
{
        (void) state;
        Token tok;
        char *value;

        Token_ctor(&tok, TOK_0);
        value = tok.vptr->getValue(&tok);
        assert_null(value);
}

static void Token_getValue_test_valueCopy(void **state)
{
        (void) state;
        Token tok;
        char *value = "hello world";
        char *copy;

        Token_ctor(&tok, TOK_0);
        tok.vptr->setValue(&tok, value);

        copy = tok.vptr->getValue(&tok);
        assert_ptr_not_equal(&value, copy);
        assert_string_equal(value, copy);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        Token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Token_setValue_test_valueCopy, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Token_setValue_test_overwriteOrig, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Token_getType_test_valid, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Token_getValue_test_nullValue, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Token_getValue_test_valueCopy, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
