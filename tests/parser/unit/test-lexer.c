/**
 * @file test-lexer.c
 * @author Mohamed Al-Hussein
 * @date 26 Jan 2022
 * @brief Unit test module for lexer.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdlib.h>

#include "parser/lexer.h"

static void lexer_generate_tokens_test_emptyBuffer(void **state)
{
        (void) state;

        char *buf = "";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        assert_int_equal(0, count);
}

static void lexer_generate_tokens_test_onlyWhitespace(void **state)
{
        (void) state;

        char *buf = "  \t\t  \t  \t\t  ";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        assert_int_equal(0, count);
}

static void lexer_generate_tokens_test_singleToken(void **state)
{
        (void) state;

        // comment token
        char *buf1 = "#";
        Token *tokens1[MAX_TOKENS];
        size_t count1 = lexer_generate_tokens(buf1, MAX_TOKENS, tokens1);

        assert_int_equal(1, count1);
        assert_int_equal(
                TOK_CMT,
                ((CommentToken *) tokens1[0])->super.vptr->get_type(tokens1[0]));

        // background control token
        char *buf2 = "&";
        Token *tokens2[MAX_TOKENS];
        size_t count2 = lexer_generate_tokens(buf2, MAX_TOKENS, tokens2);

        assert_int_equal(1, count2);
        assert_int_equal(
                TOK_CTRL_BG,
                ((BGCtrlToken *) tokens2[0])->super.vptr->get_type(tokens2[0]));

        // input redirection token
        char *buf3 = "<";
        Token *tokens3[MAX_TOKENS];
        size_t count3 = lexer_generate_tokens(buf3, MAX_TOKENS, tokens3);

        assert_int_equal(1, count3);
        assert_int_equal(
                TOK_REDIR_INPUT,
                ((InputRedirToken *) tokens3[0])->super.vptr->get_type(tokens3[0]));

        // output redirection token
        char *buf4 = ">";
        Token *tokens4[MAX_TOKENS];
        size_t count4 = lexer_generate_tokens(buf4, MAX_TOKENS, tokens4);

        assert_int_equal(1, count4);
        assert_int_equal(
                TOK_REDIR_OUTPUT,
                ((OutputRedirToken *) tokens4[0])->super.vptr->get_type(tokens4[0]));

        // newline token
        char *buf5 = "\n";
        Token *tokens5[MAX_TOKENS];
        size_t count5 = lexer_generate_tokens(buf5, MAX_TOKENS, tokens5);

        assert_int_equal(1, count5);
        assert_int_equal(
                TOK_CTRL_NEWLINE,
                ((NewlineToken *) tokens5[0])->super.vptr->get_type(tokens5[0]));

        // word token (single character)
        char *buf6 = "a";
        Token *tokens6[MAX_TOKENS];
        size_t count6 = lexer_generate_tokens(buf6, MAX_TOKENS, tokens6);

        assert_int_equal(1, count6);
        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens6[0])->super.vptr->get_type(tokens6[0]));

        // word token (multi-character)
        char *buf7 = "cmd";
        Token *tokens7[MAX_TOKENS];
        size_t count7 = lexer_generate_tokens(buf7, MAX_TOKENS, tokens7);

        assert_int_equal(1, count7);
        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens7[0])->super.vptr->get_type(tokens7[0]));
}

static void lexer_generate_tokens_test_singleTokenBeforeNewline(void **state)
{
        (void) state;

        // comment token
        char *buf1 = "#\n";
        Token *tokens1[MAX_TOKENS];
        size_t count1 = lexer_generate_tokens(buf1, MAX_TOKENS, tokens1);

        assert_int_equal(2, count1);
        assert_int_equal(
                TOK_CMT,
                ((CommentToken *) tokens1[0])->super.vptr->get_type(tokens1[0]));

        // background control token
        char *buf2 = "&\n";
        Token *tokens2[MAX_TOKENS];
        size_t count2 = lexer_generate_tokens(buf2, MAX_TOKENS, tokens2);

        assert_int_equal(2, count2);
        assert_int_equal(
                TOK_CTRL_BG,
                ((BGCtrlToken *) tokens2[0])->super.vptr->get_type(tokens2[0]));

        // input redirection token
        char *buf3 = "<\n";
        Token *tokens3[MAX_TOKENS];
        size_t count3 = lexer_generate_tokens(buf3, MAX_TOKENS, tokens3);

        assert_int_equal(2, count3);
        assert_int_equal(
                TOK_REDIR_INPUT,
                ((InputRedirToken *) tokens3[0])->super.vptr->get_type(tokens3[0]));

        // output redirection token
        char *buf4 = ">\n";
        Token *tokens4[MAX_TOKENS];
        size_t count4 = lexer_generate_tokens(buf4, MAX_TOKENS, tokens4);

        assert_int_equal(2, count4);
        assert_int_equal(
                TOK_REDIR_OUTPUT,
                ((OutputRedirToken *) tokens4[0])->super.vptr->get_type(tokens4[0]));

        // newline token
        char *buf5 = "\n\n";
        Token *tokens5[MAX_TOKENS];
        size_t count5 = lexer_generate_tokens(buf5, MAX_TOKENS, tokens5);

        assert_int_equal(2, count5);
        assert_int_equal(
                TOK_CTRL_NEWLINE,
                ((NewlineToken *) tokens5[0])->super.vptr->get_type(tokens5[0]));

        // word token (single character)
        char *buf6 = "a\n";
        Token *tokens6[MAX_TOKENS];
        size_t count6 = lexer_generate_tokens(buf6, MAX_TOKENS, tokens6);

        assert_int_equal(2, count6);
        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens6[0])->super.vptr->get_type(tokens6[0]));

        // word token (multi-character)
        char *buf7 = "cmd\n";
        Token *tokens7[MAX_TOKENS];
        size_t count7 = lexer_generate_tokens(buf7, MAX_TOKENS, tokens7);

        assert_int_equal(2, count7);
        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens7[0])->super.vptr->get_type(tokens7[0]));
}

static void lexer_generate_tokens_test_singleTokenTrailingWhitespace(void **state)
{
        (void) state;

        // comment token
        char *buf1 = " \t#\t ";
        Token *tokens1[MAX_TOKENS];
        size_t count1 = lexer_generate_tokens(buf1, MAX_TOKENS, tokens1);

        assert_int_equal(1, count1);
        assert_int_equal(
                TOK_CMT,
                ((CommentToken *) tokens1[0])->super.vptr->get_type(tokens1[0]));

        // background control token
        char *buf2 = " \t&\t ";
        Token *tokens2[MAX_TOKENS];
        size_t count2 = lexer_generate_tokens(buf2, MAX_TOKENS, tokens2);

        assert_int_equal(1, count2);
        assert_int_equal(
                TOK_CTRL_BG,
                ((BGCtrlToken *) tokens2[0])->super.vptr->get_type(tokens2[0]));

        // input redirection token
        char *buf3 = " \t<\t ";
        Token *tokens3[MAX_TOKENS];
        size_t count3 = lexer_generate_tokens(buf3, MAX_TOKENS, tokens3);

        assert_int_equal(1, count3);
        assert_int_equal(
                TOK_REDIR_INPUT,
                ((InputRedirToken *) tokens3[0])->super.vptr->get_type(tokens3[0]));

        // output redirection token
        char *buf4 = " \t>\t ";
        Token *tokens4[MAX_TOKENS];
        size_t count4 = lexer_generate_tokens(buf4, MAX_TOKENS, tokens4);

        assert_int_equal(1, count4);
        assert_int_equal(
                TOK_REDIR_OUTPUT,
                ((OutputRedirToken *) tokens4[0])->super.vptr->get_type(tokens4[0]));

        // newline token
        char *buf5 = " \t\n\t ";
        Token *tokens5[MAX_TOKENS];
        size_t count5 = lexer_generate_tokens(buf5, MAX_TOKENS, tokens5);

        assert_int_equal(1, count5);
        assert_int_equal(
                TOK_CTRL_NEWLINE,
                ((NewlineToken *) tokens5[0])->super.vptr->get_type(tokens5[0]));

        // word token (single character)
        char *buf6 = " \ta\t ";
        Token *tokens6[MAX_TOKENS];
        size_t count6 = lexer_generate_tokens(buf6, MAX_TOKENS, tokens6);

        assert_int_equal(1, count6);
        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens6[0])->super.vptr->get_type(tokens6[0]));

        // word token (multi-character)
        char *buf7 = " \tcmd\t ";
        Token *tokens7[MAX_TOKENS];
        size_t count7 = lexer_generate_tokens(buf7, MAX_TOKENS, tokens7);

        assert_int_equal(1, count7);
        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens7[0])->super.vptr->get_type(tokens7[0]));
}

static void lexer_generate_tokens_test_allTokenTypes(void **state)
{
        (void) state;

        char *buf = "# cmd arg1 arg2 arg3$$ < file1.txt > file2.txt &\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        assert_int_equal(11, count);

        assert_int_equal(
                TOK_CMT,
                ((CommentToken *) tokens[0])->super.vptr->get_type(tokens[0]));
        assert_string_equal("#",
                            ((CommentToken *) tokens[0])->super.vptr->get_value(tokens[0]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[1])->super.vptr->get_type(tokens[1]));
        assert_string_equal("cmd",
                            ((WordToken *) tokens[1])->super.vptr->get_value(tokens[1]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[2])->super.vptr->get_type(tokens[2]));
        assert_string_equal("arg1",
                            ((WordToken *) tokens[2])->super.vptr->get_value(tokens[2]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[3])->super.vptr->get_type(tokens[3]));
        assert_string_equal("arg2",
                            ((WordToken *) tokens[3])->super.vptr->get_value(tokens[3]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[4])->super.vptr->get_type(tokens[4]));
        assert_string_equal("arg3$$",
                            ((WordToken *) tokens[4])->super.vptr->get_value(tokens[4]));

        assert_int_equal(
                TOK_REDIR_INPUT,
                ((InputRedirToken *) tokens[5])->super.vptr->get_type(tokens[5]));
        assert_string_equal("<",
                            ((InputRedirToken *) tokens[5])->super.vptr->get_value(tokens[5]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[6])->super.vptr->get_type(tokens[6]));
        assert_string_equal("file1.txt",
                            ((WordToken *) tokens[6])->super.vptr->get_value(tokens[6]));

        assert_int_equal(
                TOK_REDIR_OUTPUT,
                ((OutputRedirToken *) tokens[7])->super.vptr->get_type(tokens[7]));
        assert_string_equal(">",
                            ((OutputRedirToken *) tokens[7])->super.vptr->get_value(tokens[7]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[8])->super.vptr->get_type(tokens[8]));
        assert_string_equal("file2.txt",
                            ((WordToken *) tokens[8])->super.vptr->get_value(tokens[8]));

        assert_int_equal(
                TOK_CTRL_BG,
                ((BGCtrlToken *) tokens[9])->super.vptr->get_type(tokens[9]));
        assert_string_equal("&",
                            ((BGCtrlToken *) tokens[9])->super.vptr->get_value(tokens[9]));

        assert_int_equal(
                TOK_CTRL_NEWLINE,
                ((NewlineToken *) tokens[10])->super.vptr->get_type(tokens[10]));
        assert_string_equal("\n",
                            ((NewlineToken *) tokens[10])->super.vptr->get_value(tokens[10]));
}

static void lexer_generate_tokens_test_mangledTokens(void **state)
{
        (void) state;

        char *buf = "## #cmd# <arg1> >arg2< &arg3$$& < <file1.txt> > >file2.txt< && \\n";
        Token *tokens[MAX_TOKENS];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        assert_int_equal(11, count);

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[0])->super.vptr->get_type(tokens[0]));
        assert_string_equal("##",
                            ((WordToken *) tokens[0])->super.vptr->get_value(tokens[0]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[1])->super.vptr->get_type(tokens[1]));
        assert_string_equal("#cmd#",
                            ((WordToken *) tokens[1])->super.vptr->get_value(tokens[1]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[2])->super.vptr->get_type(tokens[2]));
        assert_string_equal("<arg1>",
                            ((WordToken *) tokens[2])->super.vptr->get_value(tokens[2]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[3])->super.vptr->get_type(tokens[3]));
        assert_string_equal(">arg2<",
                            ((WordToken *) tokens[3])->super.vptr->get_value(tokens[3]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[4])->super.vptr->get_type(tokens[4]));
        assert_string_equal("&arg3$$&",
                            ((WordToken *) tokens[4])->super.vptr->get_value(tokens[4]));

        assert_int_equal(
                TOK_REDIR_INPUT,
                ((InputRedirToken *) tokens[5])->super.vptr->get_type(tokens[5]));
        assert_string_equal("<",
                            ((InputRedirToken *) tokens[5])->super.vptr->get_value(tokens[5]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[6])->super.vptr->get_type(tokens[6]));
        assert_string_equal("<file1.txt>",
                            ((WordToken *) tokens[6])->super.vptr->get_value(tokens[6]));

        assert_int_equal(
                TOK_REDIR_OUTPUT,
                ((OutputRedirToken *) tokens[7])->super.vptr->get_type(tokens[7]));
        assert_string_equal(">",
                            ((OutputRedirToken *) tokens[7])->super.vptr->get_value(tokens[7]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[8])->super.vptr->get_type(tokens[8]));
        assert_string_equal(">file2.txt<",
                            ((WordToken *) tokens[8])->super.vptr->get_value(tokens[8]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[9])->super.vptr->get_type(tokens[9]));
        assert_string_equal("&&",
                            ((WordToken *) tokens[9])->super.vptr->get_value(tokens[9]));

        assert_int_equal(
                TOK_WORD,
                ((WordToken *) tokens[10])->super.vptr->get_type(tokens[10]));
        assert_string_equal("\\n",
                            ((WordToken *) tokens[10])->super.vptr->get_value(tokens[10]));
}

static void lexer_generate_tokens_test_tokensFull(void **state)
{
        (void) state;

        char *buf = "# cmd \n";
        Token *tokens[3];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        assert_int_equal(3, count);
}

static void lexer_generate_tokens_test_tokensExhausted(void **state)
{
        (void) state;

        char *buf = "# cmd \n";
        Token *tokens[2];
        size_t count = lexer_generate_tokens(buf, MAX_TOKENS, tokens);

        assert_int_equal(3, count);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_emptyBuffer, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_onlyWhitespace, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_singleToken, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_singleTokenBeforeNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_singleTokenTrailingWhitespace, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_allTokenTypes, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_mangledTokens, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_tokensFull, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        lexer_generate_tokens_test_tokensExhausted, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
