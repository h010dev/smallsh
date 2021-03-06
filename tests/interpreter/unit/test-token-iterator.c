/**
 * @file test-token-iterator.c
 * @author Mohamed Al-Hussein
 * @date 28 Jan 2022
 * @brief Unit test module for token-iterator.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "interpreter/token-iterator.h"
#include "interpreter/lexer.h"

struct TokenIteratorPrivate {
        size_t ti_len;
        Token **ti_tok;
        size_t ti_cur;
};

static void token_iterator_ctor_test_initializesValues(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        assert_int_equal(count, iter.private->ti_len);
        assert_int_equal(0, iter.private->ti_cur);
        for (size_t i = 0; i < count; i++) {
                assert_memory_equal(tokens[i], iter.private->ti_tok[i], sizeof(Token));
        }
}

static void token_iterator_dtor_test_deletesData(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);
        token_iterator_dtor(&iter);

        assert_null(iter.vptr);
        assert_null(iter.private);
}

static void token_iterator_next_test_advancesCurrent(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        for (size_t i = 0; i < count; i++) {
                Token *cur = iter.vptr->next(&iter);
                assert_memory_equal(tokens[i], cur, sizeof(Token));
        }
}

static void token_iterator_peek_test_returnsCurrent(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        for (size_t i = 0; i < count; i++) {
                Token cur = iter.vptr->peek(&iter, 0);
                assert_memory_equal(tokens[i], &cur, sizeof(Token));
                iter.vptr->next(&iter);
        }
}

static void token_iterator_peek_test_returnsOffset(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        // skip last token since newline token is end of iterable
        for (size_t i = 0; i < count - 1; i++) {
                Token cur = iter.vptr->peek(&iter, i);
                assert_memory_equal(tokens[i], &cur, sizeof(Token));
        }

        // cursor should not change
        assert_int_equal(0, iter.private->ti_cur);
}

static void token_iterator_peek_test_Exhausted(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        // go to last valid token
        for (size_t i = 0; i < count - 2; i++) {
                iter.vptr->next(&iter);
        }

        // cursor should be valid
        Token cur = iter.vptr->peek(&iter, 0);
        assert_memory_equal(tokens[9], &cur, sizeof(Token));

        // next should not be valid
        for (size_t i = 1; i < 5; i++) {
                cur = iter.vptr->peek(&iter, i);
        }

        // cursor should remain at original position
        cur = iter.vptr->peek(&iter, 0);
        assert_memory_equal(tokens[9], &cur, sizeof(Token));
}

static void token_iterator_has_next_test_currentAtEnd(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        // go to last token
        for (size_t i = 0; i < count; i++) {
                assert_true(iter.vptr->has_next(&iter));
                iter.vptr->next(&iter);
        }

        assert_false(iter.vptr->has_next(&iter));
}

static void token_iterator_has_next_test_detectsNewlineToken(void **state)
{
        (void) state;

        // create token array
        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        TokenIterator iter;
        token_iterator_ctor(&iter, count, tokens);

        // go to last valid token
        for (size_t i = 0; i < count - 1; i++) {
                assert_true(iter.vptr->has_next(&iter));
                iter.vptr->next(&iter);
        }

        assert_false(iter.vptr->has_next(&iter));
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        token_iterator_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_next_test_advancesCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_peek_test_returnsCurrent, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_peek_test_returnsOffset, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_peek_test_Exhausted, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_has_next_test_currentAtEnd, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        token_iterator_has_next_test_detectsNewlineToken, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
