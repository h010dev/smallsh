/**
 * @file test-node.c
 * @author Mohamed Al-Hussein
 * @date 29 Jan 2022
 * @brief Unit test module for node.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "node.c"

static void Node_ctor_test_initializesValues(void **state)
{
        (void) state;
        Node n;

        Node_ctor(&n, NODE_0);

        assert_int_equal(NODE_0, n._private->type);
        assert_null(n._private->value);
}

static void Node_dtor_test_deletesData(void **state)
{
        (void) state;
        Node n;

        Node_ctor(&n, NODE_0);
        Node_dtor(&n);

        assert_null(n.vptr);
        assert_null(n._private);
}

static void Node_setValue_test_valueAssignment(void **state)
{
        (void) state;

        // bg control node
        Node n1;
        NodeValue v1 = { .bgctrl_value = 9 };
        Node_ctor(&n1, NODE_CTRL_BG);
        n1.vptr->setValue(&n1, &v1);
        assert_ptr_equal(&v1, n1._private->value);
        assert_int_equal(9, n1._private->value->bgctrl_value);

        // comment node
        Node n2;
        NodeValue v2 = { .cmt_value = 10 };
        Node_ctor(&n2, NODE_CMT);
        n2.vptr->setValue(&n2, &v2);
        assert_ptr_equal(&v2, n2._private->value);
        assert_int_equal(10, n2._private->value->cmt_value);

        // command node
        Node n3;
        WordToken t1;
        WordToken_ctor(&t1);
        WordToken **argv = malloc(sizeof(WordToken));
        argv[0] = &t1;
        t1.super.vptr->setValue((Token *) &t1, "cmd");
        NodeValue v3 = { .cmd_value = {
                .argc = 1,
                .argv = argv,
        } };
        Node_ctor(&n3, NODE_CMD);
        n3.vptr->setValue(&n3, &v3);
        assert_ptr_equal(&v3, n3._private->value);
        assert_int_equal(1, n3._private->value->cmd_value.argc);
        CommandValue c1 = n3._private->value->cmd_value;
        char *cmd = c1.argv[0]->super.vptr->getValue((Token *) c1.argv[0]);
        assert_string_equal("cmd", cmd);

        // input redir node
        Node n4;
        InputRedirToken t2;
        WordToken f1;
        WordToken_ctor(&f1);
        f1.super.vptr->setValue((Token *) &f1, "file1.txt");
        InputRedirToken_ctor(&t2);
        t2.super.vptr->setValue((Token *) &t2, "<");
        IORedirValue v4 = { .type = TOK_REDIR_INPUT, .stream = &f1 };
        NodeValue x1 = { .ioredir_value = v4 };
        Node_ctor(&n4, NODE_REDIR);
        n4.vptr->setValue(&n4, &x1);
        NodeValue *res1 = n4.vptr->getValue(&n4);
        assert_memory_equal(res1, &x1, sizeof(NodeValue));

        // output redir node
        Node n5;
        OutputRedirToken t3;
        WordToken f2;
        WordToken_ctor(&f2);
        f2.super.vptr->setValue((Token *) &f2, "file2.txt");
        OutputRedirToken_ctor(&t3);
        t3.super.vptr->setValue((Token *) &t3, ">");
        IORedirValue v5 = { .type = TOK_REDIR_OUTPUT, .stream = &f2 };
        NodeValue x2 = { .ioredir_value = v5 };
        Node_ctor(&n5, NODE_REDIR);
        n4.vptr->setValue(&n5, &x2);
        NodeValue *res2 = n5.vptr->getValue(&n5);
        assert_memory_equal(res2, &x2, sizeof(NodeValue));
}

static void Node_setValue_test_overwriteOrig(void **state)
{
        (void) state;

        // bg control node
        Node n1;
        NodeValue v1 = { .bgctrl_value = 9 };
        NodeValue v2 = { .bgctrl_value = 12 };
        Node_ctor(&n1, NODE_CTRL_BG);
        n1.vptr->setValue(&n1, &v1);
        n1.vptr->setValue(&n1, &v2);
        assert_ptr_equal(&v2, n1._private->value);
        assert_int_equal(12, n1._private->value->bgctrl_value);

        // comment node
        Node n2;
        NodeValue v3 = { .cmt_value = 10 };
        NodeValue v4 = { .cmt_value = 5 };
        Node_ctor(&n2, NODE_CMT);
        n2.vptr->setValue(&n2, &v3);
        n2.vptr->setValue(&n2, &v4);
        assert_ptr_equal(&v4, n2._private->value);
        assert_int_equal(5, n2._private->value->cmt_value);

        // command node
        Node n3;
        WordToken t1;
        WordToken_ctor(&t1);
        WordToken **argv = malloc(sizeof(WordToken));
        argv[0] = &t1;
        t1.super.vptr->setValue((Token *) &t1, "cmd");
        NodeValue v5 = { .cmd_value = {
                .argc = 1,
                .argv = argv,
        } };
        Node_ctor(&n3, NODE_CMD);
        n3.vptr->setValue(&n3, &v5);
        WordToken t2;
        WordToken_ctor(&t2);
        WordToken **argv2 = malloc(sizeof(WordToken));
        argv2[0] = &t2;
        t2.super.vptr->setValue((Token *) &t2, "hello");
        NodeValue v6 = { .cmd_value = {
                        .argc = 1,
                        .argv = argv2,
                } };
        n3.vptr->setValue(&n3, &v6);
        assert_ptr_equal(&v6, n3._private->value);
        assert_int_equal(1, n3._private->value->cmd_value.argc);
        CommandValue c1 = n3._private->value->cmd_value;
        char *cmd = c1.argv[0]->super.vptr->getValue((Token *) c1.argv[0]);
        assert_string_equal("hello", cmd);

        // input redir node
        Node n4;
        Node_ctor(&n4, NODE_REDIR);

        WordToken f1;
        WordToken_ctor(&f1);
        f1.super.vptr->setValue((Token *) &f1, "file1.txt");

        InputRedirToken t3;
        InputRedirToken_ctor(&t3);
        t3.super.vptr->setValue((Token *) &t3, "<");

        IORedirValue v7 = { .type = TOK_REDIR_INPUT, .stream = &f1 };
        NodeValue x1 = { .ioredir_value = v7 };
        n4.vptr->setValue(&n4, &x1);

        WordToken f2;
        WordToken_ctor(&f2);
        f2.super.vptr->setValue((Token *) &f2, "file2.txt");

        InputRedirToken t4;
        InputRedirToken_ctor(&t4);
        t4.super.vptr->setValue((Token *) &t4, "<");

        IORedirValue v8 = { .type = TOK_REDIR_INPUT, .stream = &f2 };
        NodeValue x2 = { .ioredir_value = v8 };
        n4.vptr->setValue(&n4, &x2);

        NodeValue *res1 = n4.vptr->getValue(&n4);
        assert_memory_equal(res1, &x2, sizeof(NodeValue));

        // output redir node
        Node n5;
        Node_ctor(&n5, NODE_REDIR);

        WordToken f3;
        WordToken_ctor(&f3);
        f3.super.vptr->setValue((Token *) &f3, "file2.txt");

        OutputRedirToken t5;
        OutputRedirToken_ctor(&t5);
        t5.super.vptr->setValue((Token *) &t5, ">");

        IORedirValue v9 = { .type = TOK_REDIR_OUTPUT, .stream = &f3 };
        NodeValue x3 = { .ioredir_value = v9 };
        n5.vptr->setValue(&n5, &x3);

        WordToken f4;
        WordToken_ctor(&f4);
        f4.super.vptr->setValue((Token *) &f4, "file5.txt");

        OutputRedirToken t6;
        OutputRedirToken_ctor(&t6);
        t6.super.vptr->setValue((Token *) &t6, ">");

        IORedirValue v10 = { .type = TOK_REDIR_OUTPUT, .stream = &f4 };
        NodeValue x4 = { .ioredir_value = v10 };
        n5.vptr->setValue(&n5, &x4);

        NodeValue *res2 = n5.vptr->getValue(&n5);
        assert_memory_equal(res2, &x4, sizeof(NodeValue));
}

static void Node_getType_test_valid(void **state)
{
        (void) state;
        Node n;

        Node_ctor(&n, NODE_0);

        NodeType value = n.vptr->getType(&n);
        assert_int_equal(NODE_0, value);
}

static void Node_getValue_test_nullValue(void **state)
{
        (void) state;
        Node n;

        Node_ctor(&n, NODE_0);

        assert_null(n.vptr->getValue(&n));
}

static void Node_getValue_test_valueCopy(void **state)
{
        (void) state;
        Node n;

        Node_ctor(&n, NODE_0);

        // bg control node
        NodeValue v1 = { .bgctrl_value = 12 };
        Node_ctor(&n, NODE_CTRL_BG);
        n.vptr->setValue(&n, &v1);

        NodeValue out = *n.vptr->getValue(&n);
        assert_ptr_not_equal(&v1, &out);
        assert_ptr_not_equal(&out, n._private->value);
        assert_ptr_equal(&v1, n._private->value);
        assert_int_equal(12, out.bgctrl_value);
}

static void Node_print_testDisplay(void **state)
{
        (void) state;

        // bg control node
        Node n1;
        NodeValue v1 = { .bgctrl_value = 9 };
        Node_ctor(&n1, NODE_CTRL_BG);
        n1.vptr->setValue(&n1, &v1);

        n1.vptr->print(&n1);

        // comment node
        Node n2;
        NodeValue v2 = { .cmt_value = 10 };
        Node_ctor(&n2, NODE_CMT);
        n2.vptr->setValue(&n2, &v2);

        n2.vptr->print(&n2);

        // command node
        Node n3;
        WordToken t1;
        WordToken_ctor(&t1);
        WordToken w2;
        WordToken_ctor(&w2);
        WordToken **argv = malloc(sizeof(WordToken) * 2);
        argv[0] = &t1;
        argv[1] = &w2;
        t1.super.vptr->setValue((Token *) &t1, "cmd");
        w2.super.vptr->setValue((Token *) &w2, "arg1");
        NodeValue v3 = { .cmd_value = {
                .argc = 2,
                .argv = argv,
        } };
        Node_ctor(&n3, NODE_CMD);
        n3.vptr->setValue(&n3, &v3);

        n3.vptr->print(&n3);

        // input redir node
        Node n4;
        InputRedirToken t2;
        WordToken f1;
        WordToken_ctor(&f1);
        f1.super.vptr->setValue((Token *) &f1, "file1.txt");
        InputRedirToken_ctor(&t2);
        t2.super.vptr->setValue((Token *) &t2, "<");
        IORedirValue v4 = { .type = TOK_REDIR_INPUT, .stream = &f1 };
        NodeValue x1 = { .ioredir_value = v4 };
        Node_ctor(&n4, NODE_REDIR);
        n4.vptr->setValue(&n4, &x1);

        n4.vptr->print(&n4);

        // output redir node
        Node n5;
        OutputRedirToken t3;
        WordToken f2;
        WordToken_ctor(&f2);
        f2.super.vptr->setValue((Token *) &f2, "file2.txt");
        OutputRedirToken_ctor(&t3);
        t3.super.vptr->setValue((Token *) &t3, ">");
        IORedirValue v5 = { .type = TOK_REDIR_OUTPUT, .stream = &f2 };
        NodeValue x2 = { .ioredir_value = v5 };
        Node_ctor(&n5, NODE_REDIR);
        n4.vptr->setValue(&n5, &x2);

        n5.vptr->print(&n5);

        assert_true(true);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        Node_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_dtor_test_deletesData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_setValue_test_valueAssignment, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_setValue_test_overwriteOrig, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_getType_test_valid, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_getValue_test_nullValue, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_getValue_test_valueCopy, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        Node_print_testDisplay, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
