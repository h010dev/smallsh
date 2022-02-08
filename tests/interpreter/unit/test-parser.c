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

#include "interpreter/parser.h"

// case 1: empty string
static void parser_insert_pid_test_retModifiedString01(void **state)
{
        (void) state;

        // setup string
        char *str = "";
        char *ptr = &str[0];
        size_t len = 0;

        // try inserting pid into string
        char *res = parser_insert_pid(str, &ptr, &len);

        // should end up with this:
        char *exp = "123456";
        size_t e_len = 4;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // ptr should point at EOL
        assert_int_equal('\0', *ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// case 2: single char string, ptr at start
static void parser_insert_pid_test_retModifiedString02(void **state)
{
        (void) state;

        // setup string
        char *str = "a";
        char *ptr = &str[0];
        size_t len = 1;

        // try inserting pid into string
        char *res = parser_insert_pid(str, &ptr, &len);

        // should end up with this:
        char *exp = "123456";
        size_t e_len = 5;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // ptr should point at EOL
        assert_int_equal('\0', *ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// case 3: single char string, ptr at end
static void parser_insert_pid_test_retModifiedString03(void **state)
{
        (void) state;

        // setup string
        char *str = "a";
        char *ptr = &str[1];
        size_t len = 1;

        // try inserting pid into string
        char *res = parser_insert_pid(str, &ptr, &len);

        // should end up with this:
        char *exp = "a123456";
        size_t e_len = 5;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // ptr should point at EOL
        assert_int_equal('\0', *ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// case 4: multi-char string, ptr at end
static void parser_insert_pid_test_retModifiedString04(void **state)
{
        (void) state;

        // setup string
        char *str = "abc";
        char *ptr = &str[3];
        size_t len = 3;

        // try inserting pid into string
        char *res = parser_insert_pid(str, &ptr, &len);

        // should end up with this:
        char *exp = "abc123456";
        size_t e_len = 7;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // ptr should point at EOL
        assert_int_equal('\0', *ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// word only contains pid expansion
static void parser_substitute_variable_test_subPID01(void **state)
{
        (void) state;

        // setup word
        char *word = "$$";
        size_t len = 3;
        char *new_word = calloc(len, sizeof(char));
        char *old_ptr = &word[0];
        char *new_ptr = &new_word[0];

        // try expanding word with pid
        char *res = parser_substitute_variable(new_word, &old_ptr, &new_ptr,
                                               &len);

        // should end up with this:
        char *exp = "123456";
        size_t e_len = 7;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // old_ptr should point at second '$'
        assert_int_equal('$', *old_ptr);

        // new_ptr should point at EOL
        assert_int_equal('\0', *new_ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// word only contains single '$'
static void parser_substitute_variable_test_subPID02(void **state)
{
        (void) state;

        // setup word
        char *word = "$";
        size_t len = 2;
        char *new_word = calloc(len, sizeof(char));
        char *old_ptr = &word[0];
        char *new_ptr = &new_word[0];

        // try expanding word with pid
        char *res = parser_substitute_variable(new_word, &old_ptr, &new_ptr,
                                               &len);

        // should end up with this:
        char *exp = "$";
        size_t e_len = 2;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // old_ptr should point at first '$'
        assert_int_equal('$', *old_ptr);

        // new_ptr should point at EOL
        assert_int_equal('\0', *new_ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// word contains multiple-'$' and chars
static void parser_substitute_variable_test_subPID03(void **state)
{
        (void) state;

        // setup word
        char *word = "abcd$$$$efghi";
        size_t len = 14;
        char *new_word = calloc(len, sizeof(char));
        new_word[0] = 'a';
        new_word[1] = 'b';
        new_word[2] = 'c';
        new_word[3] = 'd';
        char *old_ptr = &word[4];
        char *new_ptr = &new_word[4];

        // try expanding word with pid
        char *res = parser_substitute_variable(new_word, &old_ptr, &new_ptr,
                                               &len);

        // should end up with this:
        char *exp = "abcd123456";
        size_t e_len = 18;
        assert_non_null(res);
        assert_string_equal(exp, res);

        // old_ptr should point at first '$'
        assert_int_equal('$', *old_ptr);

        // new_ptr should point at EOL
        assert_int_equal('\0', *new_ptr);

        // len should be updated to original len + pid_len delta
        assert_int_equal(e_len, len);
}

// case 1: no expansions
static void parser_expand_word_test_expandStr01(void **state)
{
        (void) state;

        // setup word
        char *word = "abcdefg";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "abcdefg";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 2: single '$' string
static void parser_expand_word_test_expandStr02(void **state)
{
        (void) state;

        // setup word
        char *word = "$";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "$";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 3: single expansion only
static void parser_expand_word_test_expandStr03(void **state)
{
        (void) state;

        // setup word
        char *word = "$$";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "123456";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 4: single expansion at string beginning
static void parser_expand_word_test_expandStr04(void **state)
{
        (void) state;

        // setup word
        char *word = "$$abcdefg";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "123456abcdefg";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 5: single expansion at string middle
static void parser_expand_word_test_expandStr05(void **state)
{
        (void) state;

        // setup word
        char *word = "abc$$def";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "abc123456def";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 6: single expansion at string end
static void parser_expand_word_test_expandStr06(void **state)
{
        (void) state;

        // setup word
        char *word = "abcdefg$$";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "abcdefg123456";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 7: multiple solo '$' chars
static void parser_expand_word_test_expandStr07(void **state)
{
        (void) state;

        // setup word
        char *word = "$abc$d$ef$ghij$k$";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "$abc$d$ef$ghij$k$";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 8: multiple consecutive expansions 01
static void parser_expand_word_test_expandStr08(void **state)
{
        (void) state;

        // setup word
        char *word = "$$$$$$$$";
        char *exp = "123456123456123456123456";

        // try expanding word
        char *res = parser_expand_word(word);
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 9: multiple consecutive expansions 02
static void parser_expand_word_test_expandStr09(void **state)
{
        (void) state;

        // setup word
        char *word = calloc(2053, sizeof(char));
        for (size_t i = 0; i < 2052; i++) {
                word[i] = '$';
        }
        word[2052] = '\0';

        char *pid = "123456";
        char *exp = calloc(1026 * 6 + 1, sizeof(char));
        for (size_t i = 0; i < 1026; i++) {
                for (size_t j = 0; j < 6; j++) {
                        exp[i * 6 + j] = pid[j];
                }
        }
        exp[1026 * 6] = '\0';

        // try expanding word
        char *res = parser_expand_word(word);
        assert_non_null(res);
        assert_string_equal(exp, res);
}

// case 10: multiple expansions
static void parser_expand_word_test_expandStr10(void **state)
{
        (void) state;

        // setup word
        char *word = "$$a$$bc$$def$$ghij$$$$$";

        // try expanding word
        char *res = parser_expand_word(word);

        // should end up with this:
        char *exp = "123456a123456bc123456def123456ghij123456123456$";
        assert_non_null(res);
        assert_string_equal(exp, res);
}

static void parser_parse_test_emptyString01(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(0, n_stmts);

        // statements should not be null, even though empty
        assert_non_null(parser.get_statements(&parser));
}

static void parser_parse_test_emptyString02(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "     \t\t\t   \t   \n       \n            \t\t";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(0, n_stmts);

        // statements should not be null, even though empty
        assert_non_null(parser.get_statements(&parser));
}

static void parser_parse_test_emptyString03(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "#\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(0, n_stmts);

        // statements should not be null, even though empty
        assert_non_null(parser.get_statements(&parser));
}

static void parser_parse_test_emptyString04(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "# cmd arg1 arg2 < f1 > f2 &\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(0, n_stmts);

        // statements should not be null, even though empty
        assert_non_null(parser.get_statements(&parser));
}

static void parser_parse_test_singleStatementCmd(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "cmd\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only single command should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // cmd should match input
        assert_string_equal("cmd", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to zero
        assert_int_equal(FLAGS_NONE, flags);
}

static void parser_parse_test_singleStatementBuiltinCD(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "cd\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only single command should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // cmd should match input
        assert_string_equal("cd", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to builtins
        assert_int_equal(FLAGS_BUILTIN, flags);
}

static void parser_parse_test_singleStatementBuiltinExit(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "exit\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only single command should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // cmd should match input
        assert_string_equal("exit", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to builtins
        assert_int_equal(FLAGS_BUILTIN, flags);
}

static void parser_parse_test_singleStatementBuiltinStatus(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "status\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only single command should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // cmd should match input
        assert_string_equal("status", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to builtins
        assert_int_equal(FLAGS_BUILTIN, flags);
}

static void parser_parse_test_singleStatementBuiltinCDPath(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "cd ./parent/child/ \n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only two args should be parsed
        assert_int_equal(2, cmd->cmd_argc);

        // args should match input
        assert_string_equal("cd", cmd->cmd_argv[0]);
        assert_string_equal("./parent/child/", cmd->cmd_argv[1]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to builtins
        assert_int_equal(FLAGS_BUILTIN, flags);
}

static void parser_parse_test_singleStatementBuiltinCDPathBG(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "cd ./parent/child/ &\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only two args should be parsed
        assert_int_equal(2, cmd->cmd_argc);

        // args should match input
        assert_string_equal("cd", cmd->cmd_argv[0]);
        assert_string_equal("./parent/child/", cmd->cmd_argv[1]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to builtins
        int exp_flags = FLAGS_BGCTRL | FLAGS_BUILTIN;
        assert_int_equal(exp_flags, flags);
}

static void parser_parse_test_singleStatementCmdArg(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "echo hello \n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only two args should be parsed
        assert_int_equal(2, cmd->cmd_argc);

        // args should match input
        assert_string_equal("echo", cmd->cmd_argv[0]);
        assert_string_equal("hello", cmd->cmd_argv[1]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should not be set
        assert_int_equal(FLAGS_NONE, flags);
}

static void parser_parse_test_singleStatementCmd2Args(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "echo hello world\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only three args should be parsed
        assert_int_equal(3, cmd->cmd_argc);

        // args should match input
        assert_string_equal("echo", cmd->cmd_argv[0]);
        assert_string_equal("hello", cmd->cmd_argv[1]);
        assert_string_equal("world", cmd->cmd_argv[2]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should not be set
        assert_int_equal(FLAGS_NONE, flags);
}

static void parser_parse_test_singleStatementCmdStdin(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "echo < file1.txt \n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only 1 arg should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // args should match input
        assert_string_equal("echo", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // only one stdin streams should be parsed
        assert_int_equal(1, stdin_->stdin_num_streams);

        // stream should match
        assert_string_equal("file1.txt", stdin_->stdin_streams[0]);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should not be set
        assert_int_equal(FLAGS_NONE, flags);
}

static void parser_parse_test_singleStatementCmdStdout(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "echo > file1.txt \n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only 1 arg should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // args should match input
        assert_string_equal("echo", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // only one stdout streams should be parsed
        assert_int_equal(1, stdout_->stdout_num_streams);

        // stream should match
        assert_string_equal("file1.txt", stdout_->stdout_streams[0]);

        // flags should not be set
        assert_int_equal(FLAGS_NONE, flags);
}

static void parser_parse_test_singleStatementCmdBG(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "echo & \n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // no statements should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only 1 arg should be parsed
        assert_int_equal(1, cmd->cmd_argc);

        // args should match input
        assert_string_equal("echo", cmd->cmd_argv[0]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // no stdin streams should be parsed
        assert_int_equal(0, stdin_->stdin_num_streams);

        // no stdout streams should be parsed
        assert_int_equal(0, stdout_->stdout_num_streams);

        // flags should be set to bg
        assert_int_equal(FLAGS_BGCTRL, flags);
}

static void parser_parse_test_singleStatementAllTypes01(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "cd ~/Documents . > f1.txt < f2.txt & #\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // only one statement should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only 3 arg should be parsed
        assert_int_equal(3, cmd->cmd_argc);

        // args should match input
        assert_string_equal("cd", cmd->cmd_argv[0]);
        assert_string_equal("~/Documents", cmd->cmd_argv[1]);
        assert_string_equal(".", cmd->cmd_argv[2]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // one stdin stream should open
        assert_int_equal(1, stdin_->stdin_num_streams);

        // stream name should match
        assert_string_equal("f2.txt", stdin_->stdin_streams[0]);

        // one stdout streams should open
        assert_int_equal(1, stdout_->stdout_num_streams);

        // stream name should match
        assert_string_equal("f1.txt", stdout_->stdout_streams[0]);

        // flags should be set to bg and builtin
        assert_int_equal(FLAGS_BGCTRL | FLAGS_BUILTIN, flags);
}

static void parser_parse_test_singleStatementAllTypes02(void **state)
{
        (void) state;

        // setup parser
        Parser parser;
        parser_ctor(&parser);

        // setup line
        char *line = "ls ~/Documents . > f1.txt < f2.txt &\n";

        // parse statement
        ssize_t n_stmts = parser.parse(&parser, line);

        // only one statement should have been created
        assert_int_equal(1, n_stmts);

        // statements should not be null
        Statement **stmts = parser.get_statements(&parser);
        assert_non_null(stmts);

        // all fields in statement should be valid
        StmtCmd *cmd = stmts[0]->stmt_cmd;
        StmtStdin *stdin_ = stmts[0]->stmt_stdin;
        StmtStdout *stdout_ = stmts[0]->stmt_stdout;
        StmtFlags flags = stmts[0]->stmt_flags;

        // only 3 arg should be parsed
        assert_int_equal(3, cmd->cmd_argc);

        // args should match input
        assert_string_equal("ls", cmd->cmd_argv[0]);
        assert_string_equal("~/Documents", cmd->cmd_argv[1]);
        assert_string_equal(".", cmd->cmd_argv[2]);

        // trailing arg should be null
        assert_null(cmd->cmd_argv[cmd->cmd_argc]);

        // one stdin stream should open
        assert_int_equal(1, stdin_->stdin_num_streams);

        // stream name should match
        assert_string_equal("f2.txt", stdin_->stdin_streams[0]);

        // one stdout streams should open
        assert_int_equal(1, stdout_->stdout_num_streams);

        // stream name should match
        assert_string_equal("f1.txt", stdout_->stdout_streams[0]);

        // flags should be set to bg
        assert_int_equal(FLAGS_BGCTRL, flags);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        parser_insert_pid_test_retModifiedString01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_insert_pid_test_retModifiedString02, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_insert_pid_test_retModifiedString03, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_insert_pid_test_retModifiedString04, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_substitute_variable_test_subPID01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_substitute_variable_test_subPID02, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_substitute_variable_test_subPID03, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr02, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr03, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr04, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr05, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr06, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr07, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr08, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr09, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_expand_word_test_expandStr10, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_emptyString01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_emptyString02, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_emptyString03, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_emptyString04, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementCmd, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementBuiltinCD, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementBuiltinExit, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementBuiltinStatus, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementBuiltinCDPath, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementBuiltinCDPathBG, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementCmdArg, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementCmd2Args, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementCmdStdin, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementCmdStdout, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementCmdBG, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementAllTypes01, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        parser_parse_test_singleStatementAllTypes02, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
