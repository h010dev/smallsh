/**
 * @file test-shell-tokens.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief Unit test module for shell-tokens.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "interpreter/shell-tokens.h"

// region CommentToken
static void comment_token_ctor_test_initializesValues(void **state)
{
        (void) state;
        CommentToken tok;

        comment_token_ctor(&tok);

        assert_int_equal(TOK_CMT, tok.super.vptr->get_type((Token *) &tok));
        assert_null(tok.super.vptr->get_value((Token *) &tok));
}

static void comment_token_dtor_test_deletesData(void **state)
{
        (void) state;
        CommentToken tok;

        comment_token_ctor(&tok);
        comment_token_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super.private);
}

static void comment_token_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "#";
        char sym = '#';
        char *slice;
        CommentToken tok;

        // iterate to symbol
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        comment_token_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->get_value((Token *) &tok);
        assert_string_equal("#", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region InputRedirToken
static void inputredir_token_ctor_test_initializesValues(void **state)
{
        (void) state;
        InputRedirToken tok;

        inputredir_token_ctor(&tok);

        assert_int_equal(TOK_REDIR_INPUT, tok.super.vptr->get_type((Token *) &tok));
        assert_null(tok.super.vptr->get_value((Token *) &tok));
}

static void inputredir_token_dtor_test_deletesData(void **state)
{
        (void) state;
        InputRedirToken tok;

        inputredir_token_ctor(&tok);
        inputredir_token_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super.private);
}

static void inputredir_token_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd < file1.txt";
        char sym = '<';
        char *slice;
        InputRedirToken tok;

        // iterate to symbol
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        inputredir_token_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->get_value((Token *) &tok);
        assert_string_equal("<", slice);

        // iter should point at end of token
        assert_int_equal(' ', *iter.vptr->next(&iter));
}
// endregion

// region OutputRedirToken
static void outputredir_token_ctor_test_initializesValues(void **state)
{
        (void) state;
        OutputRedirToken tok;

        outputredir_token_ctor(&tok);

        assert_int_equal(TOK_REDIR_OUTPUT, tok.super.vptr->get_type((Token *) &tok));
        assert_null(tok.super.vptr->get_value((Token *) &tok));
}

static void outputredir_token_dtor_test_deletesData(void **state)
{
        (void) state;
        OutputRedirToken tok;

        outputredir_token_ctor(&tok);
        outputredir_token_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super.private);
}

static void outputredir_token_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt";
        char sym = '>';
        char *slice;
        OutputRedirToken tok;

        // iterate to symbol
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        outputredir_token_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->get_value((Token *) &tok);
        assert_string_equal(">", slice);

        // iter should point at end of token
        assert_int_equal(' ', *iter.vptr->next(&iter));
}
// endregion

// region BackgroundCommandToken
static void bgctrl_token_ctor_test_initializesValues(void **state)
{
        (void) state;
        BGCtrlToken tok;

        bgctrl_token_ctor(&tok);

        assert_int_equal(TOK_CTRL_BG, tok.super.vptr->get_type((Token *) &tok));
        assert_null(tok.super.vptr->get_value((Token *) &tok));
}

static void bgctrl_token_dtor_test_deletesData(void **state)
{
        (void) state;
        BGCtrlToken tok;

        bgctrl_token_ctor(&tok);
        bgctrl_token_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super.private);
}

static void bgctrl_token_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd > file1.txt &";
        char sym = '&';
        char *slice;
        BGCtrlToken tok;

        // iterate to symbol
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        bgctrl_token_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->get_value((Token *) &tok);
        assert_string_equal("&", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region WordToken
static void word_token_ctor_test_initializesValues(void **state)
{
        (void) state;
        WordToken tok;

        word_token_ctor(&tok);

        assert_int_equal(TOK_WORD, tok.super.vptr->get_type((Token *) &tok));
        assert_null(tok.super.vptr->get_value((Token *) &tok));
}

static void word_token_dtor_test_deletesData(void **state)
{
        (void) state;
        WordToken tok;

        word_token_ctor(&tok);
        word_token_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super.private);
}

static void word_token_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "cmd";
        char *slice;
        WordToken tok;

        string_iterator_ctor(&iter, string);

        // take token
        word_token_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->get_value((Token *) &tok);
        assert_string_equal("cmd", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

// region NewlineToken
static void newline_token_ctor_test_initializesValues(void **state)
{
        (void) state;
        NewlineToken tok;

        newline_token_ctor(&tok);

        assert_int_equal(TOK_CTRL_NEWLINE, tok.super.vptr->get_type((Token *) &tok));
        assert_null(tok.super.vptr->get_value((Token *) &tok));
}

static void newline_token_dtor_test_deletesData(void **state)
{
        (void) state;
        NewlineToken tok;

        newline_token_ctor(&tok);
        newline_token_dtor(&tok);

        assert_null(tok.super.vptr);
        assert_null(tok.super.private);
}

static void newline_token_take_test_functionality(void **state)
{
        (void) state;
        StringIterator iter;
        char *string = "\n";
        char sym = '\n';
        char *slice;
        NewlineToken tok;

        // iterate to symbol
        string_iterator_ctor(&iter, string);
        while (iter.vptr->peek(&iter, 0) != sym) {
                iter.vptr->next(&iter);
        }

        // take token
        newline_token_ctor(&tok);
        tok.super.vptr->take((Token *) &tok, &iter);
        slice = tok.super.vptr->get_value((Token *) &tok);
        assert_string_equal("\n", slice);

        // iter should point at end of token
        assert_int_equal('\0', *iter.vptr->next(&iter));
}
// endregion

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        comment_token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        comment_token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        comment_token_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        inputredir_token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        inputredir_token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        inputredir_token_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        outputredir_token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        outputredir_token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        outputredir_token_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        bgctrl_token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        bgctrl_token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        bgctrl_token_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        word_token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        word_token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        word_token_take_test_functionality, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        newline_token_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        newline_token_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        newline_token_take_test_functionality, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
