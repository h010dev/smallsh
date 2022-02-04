#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdlib.h>

#include "parser/token.h"

struct TokenPrivate {
        TokenType tok_type;
        char *tok_value;
};

static void token_ctor_test_initializesValues(void **state)
{
        (void) state;
        Token tok;

        token_ctor(&tok, TOK_0);

        assert_int_equal(TOK_0, tok.private->tok_type);
        assert_null(tok.private->tok_value);
}

static void token_dtor_test_deletesData(void **state)
{
        (void) state;
        Token tok;

        token_ctor(&tok, TOK_0);
        token_dtor(&tok);

        assert_null(tok.vptr);
        assert_null(tok.private);
}

static void token_set_value_test_valueCopy(void **state)
{
        (void) state;
        Token tok;
        char *value = "hello world";

        token_ctor(&tok, TOK_0);
        tok.vptr->set_value(&tok, value);

        assert_ptr_not_equal(&value, &tok.private->tok_value);
        assert_string_equal(value, tok.private->tok_value);
}

static void token_set_value_test_overwriteOrig(void **state)
{
        (void) state;
        Token tok;
        char *orig = "hello world";
        char *new = "goodbye world";

        token_ctor(&tok, TOK_0);
        tok.vptr->set_value(&tok, orig);

        tok.vptr->set_value(&tok, new);
        assert_ptr_not_equal(&new, &tok.private->tok_value);
        assert_string_equal(new, tok.private->tok_value);
}

static void token_get_type_test_valid(void **state)
{
        (void) state;
        Token tok;

        token_ctor(&tok, TOK_0);
        assert_int_equal(TOK_0, tok.vptr->get_value(&tok));
}

static void token_get_value_test_nullValue(void **state)
{
        (void) state;
        Token tok;
        char *value;

        token_ctor(&tok, TOK_0);
        value = tok.vptr->get_value(&tok);
        assert_null(value);
}

static void token_get_value_test_valueCopy(void **state)
{
        (void) state;
        Token tok;
        char *value = "hello world";
        char *copy;

        token_ctor(&tok, TOK_0);
        tok.vptr->set_value(&tok, value);

        copy = tok.vptr->get_value(&tok);
        assert_ptr_not_equal(&value, copy);
        assert_string_equal(value, copy);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_set_value_test_valueCopy, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_set_value_test_overwriteOrig, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_get_type_test_valid, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_get_value_test_nullValue, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_get_value_test_valueCopy, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
