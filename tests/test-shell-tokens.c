#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "shell-tokens.h"

// region CommentToken
static void CommentToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        CommentToken tok;

        CommentToken_ctor(&tok);

        assert_int_equal(COMMENT_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void CommentToken_dtor_test_deletesData(void **state)
{
        (void) state;
        CommentToken tok;

        CommentToken_ctor(&tok);
        CommentToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void CommentToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#";
        char sym = '#';
        char *slice;
        CommentToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        CommentToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("#", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region InputRedirToken
static void InputRedirToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        InputRedirToken tok;

        InputRedirToken_ctor(&tok);

        assert_int_equal(INPUT_REDIR_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void InputRedirToken_dtor_test_deletesData(void **state)
{
        (void) state;
        InputRedirToken tok;

        InputRedirToken_ctor(&tok);
        InputRedirToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void InputRedirToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd < file1.txt";
        char sym = '<';
        char *slice;
        InputRedirToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        InputRedirToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("<", slice);

        // iter should point at end of token
        assert_int_equal(' ', *iter.vptr->next(&iter));
}
// endregion

// region OutputRedirToken
static void OutputRedirToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        OutputRedirToken tok;

        OutputRedirToken_ctor(&tok);

        assert_int_equal(OUTPUT_REDIR_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void OutputRedirToken_dtor_test_deletesData(void **state)
{
        (void) state;
        OutputRedirToken tok;

        OutputRedirToken_ctor(&tok);
        OutputRedirToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void OutputRedirToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt";
        char sym = '>';
        char *slice;
        OutputRedirToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        OutputRedirToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal(">", slice);

        // iter should point at end of token
        assert_int_equal(' ', *iter.vptr->next(&iter));
}
// endregion

// region BackgroundCommandToken
static void BGControlToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        BGControlToken tok;

        BGControlToken_ctor(&tok);

        assert_int_equal(BG_CONTROL_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void BGControlToken_dtor_test_deletesData(void **state)
{
        (void) state;
        BGControlToken tok;

        BGControlToken_ctor(&tok);
        BGControlToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void BGControlToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt &";
        char sym = '&';
        char *slice;
        BGControlToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        BGControlToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("&", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region WordToken
static void WordToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        WordToken tok;

        WordToken_ctor(&tok);

        assert_int_equal(WORD_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void WordToken_dtor_test_deletesData(void **state)
{
        (void) state;
        WordToken tok;

        WordToken_ctor(&tok);
        WordToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void WordToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd";
        char *slice;
        WordToken tok;

        StringIterator_ctor(&iter, string);

        // take token
        WordToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("cmd", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region NewlineToken
static void NewlineToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        NewlineToken tok;

        NewlineToken_ctor(&tok);

        assert_int_equal(NEWLINE_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void NewlineToken_dtor_test_deletesData(void **state)
{
        (void) state;
        NewlineToken tok;

        NewlineToken_ctor(&tok);
        NewlineToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void NewlineToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "\n";
        char sym = '\n';
        char *slice;
        NewlineToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        NewlineToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("\n", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        CommentToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        CommentToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        CommentToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        InputRedirToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        InputRedirToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        InputRedirToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        OutputRedirToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        OutputRedirToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        OutputRedirToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        BGControlToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        BGControlToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        BGControlToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        WordToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        WordToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        WordToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NewlineToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NewlineToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NewlineToken_take_test_functionality, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
