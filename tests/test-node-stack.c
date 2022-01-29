/**
 * @file test-node-stack.c
 * @author Mohamed Al-Hussein
 * @date 29 Jan 2022
 * @brief Unit test module for node-stack.c.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "node-stack.c"

static void NodeStack_ctor_test_initializesValues(void **state)
{
        (void) state;

        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        assert_int_equal(size, s._private->size);
        assert_int_equal(0, s._private->top);
}

static void NodeStack_dtor_test_destroysData(void **state)
{
        (void) state;

        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);
        Stack_dtor(&s);

        assert_null(s.isEmpty);
        assert_null(s.isFull);
        assert_null(s.peek);
        assert_null(s.pop);
        assert_null(s.push);
        assert_null(s._private);
}

static void NodeStack_push_test_addsItem(void **state)
{
        (void) state;

        // init stack
        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        // fill stack with nodes passed by value
        for (size_t i = 0; i < size; i++) {
                // add node
                Node n;
                NodeValue val;
                val.cmt_value = 1;
                Node_ctor(&n, NODE_CTRL_BG);
                n.vptr->setValue(&n, &val);
                s.push(&s, (void **) &n);

                // node should be inserted at first available index on stack array
                assert_memory_equal(&n, s._private->items[i], sizeof(Node *));

                // stack top should be updated
                assert_int_equal(i + 1, s._private->top);
        }

        // rewind stack
        s._private->top = 0;

        // fill stack with nodes passed by reference
        for (size_t i = 0; i < size; i++) {
                // add node
                Node *n = malloc(sizeof(Node));
                NodeValue val;
                val.cmt_value = 1;
                Node_ctor(n, NODE_CTRL_BG);
                n->vptr->setValue(n, &val);
                s.push(&s, (void **) &n);

                // node should be inserted at first available index on stack array
                assert_ptr_equal(&n, s._private->items[i]);

                // stack top should be updated
                assert_int_equal(i + 1, s._private->top);
        }

}

static void NodeStack_pop_test_removesItem(void **state)
{
        (void) state;

        // init stack
        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        // fill stack with nodes
        for (size_t i = 0; i < size; i++) {
                // add node
                Node n;
                NodeValue val;
                val.cmt_value = 1;
                Node_ctor(&n, NODE_CTRL_BG);
                n.vptr->setValue(&n, &val);
                s.push(&s, (void **) &n);
        }

        // empty stack
        for (size_t i = 0; i < size; i++) {
                // remove node
                Node *n = (Node *) s.pop(&s);

                // should not be null
                assert_non_null(n);

                // stack top should be updated
                assert_int_equal(size - i - 1, s._private->top);
        }

        // next pop should be null
        assert_null((Node *) s.pop(&s));

        // stack top should not change this time
        assert_int_equal(0, s._private->top);
}

static void NodeStack_peek_test_returnsCopy(void **state)
{
        (void) state;

        // init stack
        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        // fill stack with nodes
        for (size_t i = 0; i < size; i++) {
                // add node
                Node n;
                NodeValue val;
                val.cmt_value = 1;
                Node_ctor(&n, NODE_CTRL_BG);
                n.vptr->setValue(&n, &val);
                s.push(&s, (void **) &n);
        }

        // peek in stack
        for (size_t i = 0; i < size; i++) {
                // remove node
                Node *n = (Node *) s.peek(&s);

                // should not be null
                assert_non_null(n);

                // stack top should not be updated
                assert_int_equal(size, s._private->top);
        }
}

static void NodeStack_peek_tests_emptyStack(void **state)
{
        (void) state;

        // init stack
        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        // try to peek
        assert_null(s.peek(&s));
}

static void NodeStack_isEmpty_test_valid(void **state)
{
        (void) state;

        // init stack
        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        assert_true(s.isEmpty(&s));
}

static void NodeStack_isFull_test_valid(void **state)
{
        (void) state;

        // init stack
        Stack s;
        size_t size = 10;
        Stack_ctor(&s, size);

        // move top to end of stack
        s._private->top = size;

        assert_true(s.isFull(&s));
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        NodeStack_ctor_test_initializesValues, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_dtor_test_destroysData, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_push_test_addsItem, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_pop_test_removesItem, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_peek_test_returnsCopy, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_peek_tests_emptyStack, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_isEmpty_test_valid, NULL, NULL),
                cmocka_unit_test_setup_teardown(
                        NodeStack_isFull_test_valid, NULL, NULL),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
