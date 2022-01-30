/**
 * @file test-parser.c
 * @author Mohamed Al-Hussein
 * @date 29 Jan 2022
 * @brief Unit test module for parser.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "parser.c"

// forward declare to stop ide warnings even though it's included in file
void generate_tokens(char *buf, size_t n, Token *tokens[n]);

static void parse_bgctrl_test_returnsNode(void **state)
{
        (void) state;

        // define character stream
        char *buf = " & \n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for bg control node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_bgctrl(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
}

static void parse_cmt_test_returnsNode(void **state)
{
        (void) state;

        // define character stream
        char *buf = " # \n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmt(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
}

static void parse_cmd_test_oneWordTermIsNewline(void **state)
{
        (void) state;

        // define character stream
        char *buf = " cmd \n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmd(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(1, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);
        WordToken *word = cmd_val.argv[0];
        assert_non_null(word);
        char *word_str = word->super.vptr->getValue((Token *) word);
        assert_string_equal("cmd", word_str);
}

static void parse_cmd_test_oneWordTermIsBG(void **state)
{
        (void) state;

        // define character stream
        char *buf = " cmd &\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmd(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(1, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);
        WordToken *word = cmd_val.argv[0];
        assert_non_null(word);
        char *word_str = word->super.vptr->getValue((Token *) word);
        assert_string_equal("cmd", word_str);
}

static void parse_cmd_test_oneWordTermIsInputRedir(void **state)
{
        (void) state;

        // define character stream
        char *buf = " cmd < file1.txt\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmd(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(1, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);
        WordToken *word = cmd_val.argv[0];
        assert_non_null(word);
        char *word_str = word->super.vptr->getValue((Token *) word);
        assert_string_equal("cmd", word_str);
}

static void parse_cmd_test_oneWordTermIsOutputRedir(void **state)
{
        (void) state;

        // define character stream
        char *buf = " cmd > file1.txt\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmd(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(1, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);
        WordToken *word = cmd_val.argv[0];
        assert_non_null(word);
        char *word_str = word->super.vptr->getValue((Token *) word);
        assert_string_equal("cmd", word_str);
}

static void parse_cmd_test_oneWordTermIsComment(void **state)
{
        (void) state;

        // define character stream
        char *buf = " cmd # arg1\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmd(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(1, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);
        WordToken *word = cmd_val.argv[0];
        assert_non_null(word);
        char *word_str = word->super.vptr->getValue((Token *) word);
        assert_string_equal("cmd", word_str);
}

static void parse_cmd_test_multiWord(void **state)
{
        (void) state;

        // define character stream
        char *buf = "cmd arg1 arg2 arg3 arg4 < file1.txt > file2.txt &\n";
        char *words[5] = { "cmd", "arg1", "arg2", "arg3", "arg4" };

        // create token stream
        size_t n = 20;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for comment node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_cmd(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(5, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);

        for (size_t i = 0; i < 5; i++) {
                WordToken *word = cmd_val.argv[i];
                assert_non_null(word);
                assert_string_equal(words[i], word->super.vptr->getValue((Token *) word));
        }
}

static void parse_ioredir_test_validInputRedir(void **state)
{
        (void) state;

        // define character stream
        char *buf = "< file1.txt \n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for io node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_ioredir(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        IORedirValue io_val = val->ioredir_value;
        assert_non_null(&io_val);

        // fields must be valid
        assert_int_equal(TOK_REDIR_INPUT, io_val.type);
        WordToken *word = io_val.stream;
        assert_non_null(word);
        assert_string_equal("file1.txt", word->super.vptr->getValue((Token *) word));
}

static void parse_ioredir_test_validOutputRedir(void **state)
{
        (void) state;

        // define character stream
        char *buf = "> file1.txt \n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // parse token stream for io node
        TokenIterator iter;
        TokenIterator_ctor(&iter, n, tokens);
        Node *node = parse_ioredir(&iter);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        IORedirValue io_val = val->ioredir_value;
        assert_non_null(&io_val);

        // fields must be valid
        assert_int_equal(TOK_REDIR_OUTPUT, io_val.type);
        WordToken *word = io_val.stream;
        assert_non_null(word);
        assert_string_equal("file1.txt", word->super.vptr->getValue((Token *) word));
}

static void parse_test_onlyComment(void **state)
{
        (void) state;

        // define character stream
        char *buf = "#\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly one value
        assert_int_equal(n_parse, 1);

        // stack head should be parsed comment node
        Node *cmt = (Node *) stack.pop(&stack);
        assert_non_null(cmt);

        // node data should be valid
        CommentValue *val = (CommentValue *) cmt->vptr->getValue(cmt);
        assert_non_null(cmt);
}

static void parse_test_onlyBGControl(void **state)
{
        (void) state;

        // define character stream
        char *buf = "&\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly one value
        assert_int_equal(n_parse, 1);

        // stack head should be parsed bg control node
        Node *bg = (Node *) stack.pop(&stack);
        assert_non_null(bg);

        // node data should be valid
        BGControlValue *val = (BGControlValue *) bg->vptr->getValue(bg);
        assert_non_null(bg);
}

static void parse_test_onlyIORedir01(void **state)
{
        (void) state;

        // define character stream
        char *buf = "< file.txt\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly one value
        assert_int_equal(n_parse, 1);

        // stack head should be parsed bg control node
        Node *node = (Node *) stack.pop(&stack);
        assert_non_null(node);

        // node should be valid
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        IORedirValue io_val = val->ioredir_value;
        assert_non_null(&io_val);

        // fields must be valid
        assert_int_equal(TOK_REDIR_INPUT, io_val.type);
        WordToken *word = io_val.stream;
        assert_non_null(word);
        char *filename = word->super.vptr->getValue((const Token *const) word);
        assert_string_equal("file.txt", filename);
}

static void parse_test_onlyIORedir02(void **state)
{
        (void) state;

        // define character stream
        char *buf = "> file.txt\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly one value
        assert_int_equal(n_parse, 1);

        // stack head should be parsed bg control node
        Node *node = (Node *) stack.pop(&stack);
        assert_non_null(node);

        // node should be valid
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        IORedirValue io_val = val->ioredir_value;
        assert_non_null(&io_val);

        // fields must be valid
        assert_int_equal(TOK_REDIR_OUTPUT, io_val.type);
        WordToken *word = io_val.stream;
        assert_non_null(word);
        char *filename = word->super.vptr->getValue((const Token *const) word);
        assert_string_equal("file.txt", filename);
}

static void parse_test_onlyCommand01(void **state)
{
        (void) state;

        // define character stream
        char *buf = "cmd\n";

        // create token stream
        size_t n = 10;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly one value
        assert_int_equal(n_parse, 1);

        // stack head should be parsed bg control node
        Node *node = (Node *) stack.pop(&stack);
        assert_non_null(node);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(1, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);
        WordToken *word = cmd_val.argv[0];
        assert_non_null(word);
        char *word_str = word->super.vptr->getValue((Token *) word);
        assert_string_equal("cmd", word_str);
}

static void parse_test_onlyCommand02(void **state)
{
        (void) state;

        // define character stream
        char *buf = "cmd arg1 arg2 arg3 arg4\n";
        char *words[5] = { "cmd", "arg1", "arg2", "arg3", "arg4" };

        // create token stream
        size_t n = 20;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly one value
        assert_int_equal(n_parse, 1);

        // stack head should be parsed command node
        Node *node = (Node *) stack.pop(&stack);
        assert_non_null(node);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(5, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);

        for (size_t i = 0; i < 5; i++) {
                WordToken *word = cmd_val.argv[i];
                assert_non_null(word);
                assert_string_equal(words[i], word->super.vptr->getValue((Token *) word));
        }
}

static void parse_test_allTokens(void **state)
{
        (void) state;

        // define character stream
        char *buf = "cmd arg1 arg2 arg3 arg4 > output.txt < input.txt & #\n";
        char *words[5] = { "cmd", "arg1", "arg2", "arg3", "arg4" };

        // create token stream
        size_t n = 20;
        Token *tokens[n];
        (void) generate_tokens(buf, n, tokens);

        // create node stack
        Stack stack;
        size_t stack_size = 10;
        Stack_ctor(&stack, stack_size);

        // parse token stream into stack
        size_t n_parse = parse(n, tokens, &stack);

        // should parse exactly five values
        assert_int_equal(n_parse, 5);

        // [0]: comment node
        Node *cmt = (Node *) stack.pop(&stack);
        assert_non_null(cmt);

        // node data should be valid
        CommentValue *cmt_val = (CommentValue *) cmt->vptr->getValue(cmt);
        assert_non_null(cmt_val);

        // [1]: bg control node
        // stack head should be parsed bg control node
        Node *bg = (Node *) stack.pop(&stack);
        assert_non_null(bg);

        // node data should be valid
        BGControlValue *bg_val = (BGControlValue *) bg->vptr->getValue(bg);
        assert_non_null(bg_val);

        // [2]: input redirection node
        Node *input = (Node *) stack.pop(&stack);
        assert_non_null(input);

        // node should be valid
        NodeValue *input_val = input->vptr->getValue(input);
        assert_non_null(input_val);
        IORedirValue input_io_val = input_val->ioredir_value;
        assert_non_null(&input_io_val);

        // fields must be valid
        assert_int_equal(TOK_REDIR_INPUT, input_io_val.type);
        WordToken *word = input_io_val.stream;
        assert_non_null(word);
        char *filename = word->super.vptr->getValue((const Token *const) word);
        assert_string_equal("input.txt", filename);

        // [3]: output redirection node
        Node *output = (Node *) stack.pop(&stack);
        assert_non_null(output);

        // node should be valid
        assert_non_null(output);
        NodeValue *output_val = output->vptr->getValue(output);
        assert_non_null(output_val);
        IORedirValue output_io_val = output_val->ioredir_value;
        assert_non_null(&output_io_val);

        // fields must be valid
        assert_int_equal(TOK_REDIR_OUTPUT, output_io_val.type);
        WordToken *word1 = output_io_val.stream;
        assert_non_null(word1);
        char *filename1 = word1->super.vptr->getValue((const Token *const) word1);
        assert_string_equal("output.txt", filename1);

        // [4]: command node
        Node *node = (Node *) stack.pop(&stack);
        assert_non_null(node);

        // node should be valid
        assert_non_null(node);
        NodeValue *val = node->vptr->getValue(node);
        assert_non_null(val);
        CommandValue cmd_val = val->cmd_value;
        assert_non_null(&cmd_val);

        // should have parsed exactly one word
        assert_int_equal(5, cmd_val.argc);

        // parsed word should be valid
        assert_non_null(cmd_val.argv);

        for (size_t i = 0; i < 5; i++) {
                WordToken *w = cmd_val.argv[i];
                assert_non_null(w);
                assert_string_equal(words[i], w->super.vptr->getValue((Token *) w));
        }
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        parse_bgctrl_test_returnsNode, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmt_test_returnsNode, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmd_test_oneWordTermIsNewline, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmd_test_oneWordTermIsBG, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmd_test_oneWordTermIsInputRedir, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmd_test_oneWordTermIsOutputRedir, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmd_test_oneWordTermIsComment, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_cmd_test_multiWord, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_ioredir_test_validInputRedir, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_ioredir_test_validOutputRedir, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_onlyComment, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_onlyBGControl, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_onlyIORedir01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_onlyIORedir02, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_onlyCommand01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_onlyCommand02, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parse_test_allTokens, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
