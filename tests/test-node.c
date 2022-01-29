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
        Node n;
        NodeValue val;

        Node_ctor(&n, NODE_0);

        val.null_value = "hello world";
        n.vptr->setValue(&n, &val);

        assert_ptr_equal(&val, n._private->value);
        assert_string_equal("hello world", n._private->value->null_value);
}

static void Node_setValue_test_overwriteOrig(void **state)
{
        (void) state;
        Node n;
        NodeValue val1, val2;

        Node_ctor(&n, NODE_0);

        val1.null_value = "hello world";
        n.vptr->setValue(&n, &val1);
        val2.null_value = "goodbye world";
        n.vptr->setValue(&n, &val2);

        assert_ptr_equal(&val2, n._private->value);
        assert_string_equal("goodbye world", n._private->value->null_value);
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
        NodeValue val, out;

        Node_ctor(&n, NODE_0);

        val.null_value = "hello world";
        n.vptr->setValue(&n, &val);

        out = *n.vptr->getValue(&n);
        assert_ptr_not_equal(&val, &out);
        assert_ptr_not_equal(&out, n._private->value);
        assert_ptr_equal(&val, n._private->value);
        assert_string_equal("hello world", out.null_value);
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
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
