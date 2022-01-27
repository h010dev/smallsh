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
        while (iter.vptr->peek(&iter) != sym) {
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

// region InputRedirectionToken
static void InputRedirectionToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        InputRedirectionToken tok;

        InputRedirectionToken_ctor(&tok);

        assert_int_equal(INPUT_REDIRECTION_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void InputRedirectionToken_dtor_test_deletesData(void **state)
{
        (void) state;
        InputRedirectionToken tok;

        InputRedirectionToken_ctor(&tok);
        InputRedirectionToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void InputRedirectionToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd < file1.txt";
        char sym = '<';
        char *slice;
        InputRedirectionToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        InputRedirectionToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("<", slice);

        // iter should point at end of token
        assert_int_equal(' ', *iter.vptr->next(&iter));
}
// endregion

// region OutputRedirectionToken
static void OutputRedirectionToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        OutputRedirectionToken tok;

        OutputRedirectionToken_ctor(&tok);

        assert_int_equal(OUTPUT_REDIRECTION_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void OutputRedirectionToken_dtor_test_deletesData(void **state)
{
        (void) state;
        OutputRedirectionToken tok;

        OutputRedirectionToken_ctor(&tok);
        OutputRedirectionToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void OutputRedirectionToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt";
        char sym = '>';
        char *slice;
        OutputRedirectionToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        OutputRedirectionToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal(">", slice);

        // iter should point at end of token
        assert_int_equal(' ', *iter.vptr->next(&iter));
}
// endregion

// region BackgroundCommandToken
static void BackgroundCommandToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        BackgroundCommandToken tok;

        BackgroundCommandToken_ctor(&tok);

        assert_int_equal(BACKGROUND_COMMAND_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void BackgroundCommandToken_dtor_test_deletesData(void **state)
{
        (void) state;
        BackgroundCommandToken tok;

        BackgroundCommandToken_ctor(&tok);
        BackgroundCommandToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void BackgroundCommandToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt &";
        char sym = '&';
        char *slice;
        BackgroundCommandToken tok;

        // iterate to symbol
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        BackgroundCommandToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("&", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region CommandToken
static void CommandToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        CommandToken tok;

        CommandToken_ctor(&tok);

        assert_int_equal(COMMAND_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void CommandToken_dtor_test_deletesData(void **state)
{
        (void) state;
        CommandToken tok;

        CommandToken_ctor(&tok);
        CommandToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void CommandToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd";
        char *slice;
        CommandToken tok;

        StringIterator_ctor(&iter, string);

        // take token
        CommandToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("cmd", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}

// endregion

// region ArgumentToken
static void ArgumentToken_ctor_test_initializesValues(void **state)
{
        (void) state;
        ArgumentToken tok;

        ArgumentToken_ctor(&tok);

        assert_int_equal(ARGUMENT_TOKEN, tok.super.vptr->getType((Token *) &tok));
        assert_null(tok.super.vptr->getValue((Token *) &tok));
}

static void ArgumentToken_dtor_test_deletesData(void **state)
{
        (void) state;
        ArgumentToken tok;

        ArgumentToken_ctor(&tok);
        ArgumentToken_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super._private);
}

static void ArgumentToken_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd arg1";
        char *slice;
        ArgumentToken tok;

        // iterate to arg
        StringIterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter) != 'a') {
                iter.vptr->next(&iter);
        }

        // take token
        ArgumentToken_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->getValue((Token *) &tok);
        assert_string_equal("arg1", slice);

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
                        InputRedirectionToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        InputRedirectionToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        InputRedirectionToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        OutputRedirectionToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        OutputRedirectionToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        OutputRedirectionToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        BackgroundCommandToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        BackgroundCommandToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        BackgroundCommandToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        CommandToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        CommandToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        CommandToken_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        ArgumentToken_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        ArgumentToken_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        ArgumentToken_take_test_functionality, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
