/*
Copyright (c) 2017, Michael J Welsh

Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided
that the above copyright notice and this permission notice appear
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "testing_framework.h"

/* Test header guard. */
#include "../src/stack.h"
#include "../src/stack.h"

/* ========================================================================================================
 *
 *                                             TESTING UTILITIES
 *
 * ======================================================================================================== */

typedef struct TestStruct {
    int val;
    StackNode node;
} TestStruct;

TestStruct var1, var2, var3;
Stack stack;

#define ASSERT_STACK(stack, tail_ptr, size_of_stack) \
    do { \
        assert(stack.tail == (StackNode*) (tail_ptr)); \
        assert(stack.size == size_of_stack); \
    } while (0)

#define ASSERT_NODE(node, prev_ptr) \
    do { \
        assert(node.prev == (StackNode*) (prev_ptr)); \
    } while (0)

#define ASSERT_FOR_EACH(node_ptr, index) \
    do { \
        switch (index) { \
            case 0: \
                assert(node_ptr == &var1.node); \
                break; \
            case 1: \
                assert(node_ptr == &var2.node); \
                break; \
            case 2: \
                assert(node_ptr == &var3.node); \
                break; \
            default: \
                assert(0); \
        } \
    } while (0)

static void reset_globals(void) {
    stack_init(&stack);

    var1.val = 1;
    var1.node.prev = STACK_POISON_PREV;

    var2.val = 2;
    var2.node.prev = STACK_POISON_PREV;

    var3.val = 3;
    var3.node.prev = STACK_POISON_PREV;
}

/* ========================================================================================================
 *
 *                                             TESTING FUNCTIONS
 *
 * ======================================================================================================== */

void test_stack_init(void) {
    StackNode node_init_with_macro = STACK_NODE_INIT;
    ASSERT_NODE(node_init_with_macro, STACK_POISON_PREV);

    stack_init(&stack);
    ASSERT_STACK(stack, NULL, 0);
}

void test_stack_peek(void) {
    assert(stack_peek(&stack) == NULL);

    stack_push(&stack, &var1.node);
    assert(stack_peek(&stack) == &var1.node);
    stack_push(&stack, &var2.node);
    assert(stack_peek(&stack) == &var2.node);
    stack_push(&stack, &var3.node);
    assert(stack_peek(&stack) == &var3.node);
}

void test_stack_size(void) {
    assert(stack_size(&stack) == 0);

    stack_push(&stack, &var1.node);
    assert(stack_size(&stack) == 1);
    stack_push(&stack, &var2.node);
    assert(stack_size(&stack) == 2);
    stack_push(&stack, &var3.node);
    assert(stack_size(&stack) == 3);
}

void test_stack_empty(void) {
    assert(stack_empty(&stack) == 1);

    stack_push(&stack, &var1.node);
    assert(stack_empty(&stack) == 0);
    stack_push(&stack, &var2.node);
    assert(stack_empty(&stack) == 0);
    stack_push(&stack, &var3.node);
    assert(stack_empty(&stack) == 0);
}

void test_stack_push(void) {
    stack_push(&stack, &var1.node);
    ASSERT_STACK(stack, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL);
    stack_push(&stack, &var2.node);
    ASSERT_STACK(stack, &var2.node, 2);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    stack_push(&stack, &var3.node);
    ASSERT_STACK(stack, &var3.node, 3);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, &var2.node);
}

void test_stack_pop(void) {
    stack_push(&stack, &var1.node);
    stack_push(&stack, &var2.node);
    stack_push(&stack, &var3.node);
    assert(stack_pop(&stack) == &var3.node);
    ASSERT_STACK(stack, &var2.node, 2);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, STACK_POISON_PREV);
    assert(stack_pop(&stack) == &var2.node);
    ASSERT_STACK(stack, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, STACK_POISON_PREV);
    assert(stack_pop(&stack) == &var1.node);
    ASSERT_STACK(stack, NULL, 0);
    ASSERT_NODE(var1.node, STACK_POISON_PREV);
    assert(stack_pop(&stack) == NULL);
    ASSERT_STACK(stack, NULL, 0);
}

void test_stack_remove_all(void) {
    stack_push(&stack, &var1.node);
    stack_push(&stack, &var2.node);
    stack_push(&stack, &var3.node);
    stack_remove_all(&stack);
    ASSERT_STACK(stack, NULL, 0);
    ASSERT_NODE(var3.node, STACK_POISON_PREV);
    stack_remove_all(&stack);
    ASSERT_STACK(stack, NULL, 0);
}

void test_stack_entry(void) {
    assert(stack_entry(&var1.node, TestStruct, node)->val == 1);
    assert(stack_entry(&var1.node, TestStruct, node)->node.prev == STACK_POISON_PREV);
}

void test_stack_for_each(void) {
    StackNode *n;
    size_t i = 0;

    stack_for_each(n, &stack) {
        assert(0);
    }

    stack_push(&stack, &var3.node);
    stack_push(&stack, &var2.node);
    stack_push(&stack, &var1.node);

    stack_for_each(n, &stack) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 3);
}

void test_stack_for_each_safe(void) {
    StackNode *n, *backup;
    size_t i = 0;

    stack_for_each_safe(n, backup, &stack) {
        assert(0);
    }

    stack_push(&stack, &var3.node);
    stack_push(&stack, &var2.node);
    stack_push(&stack, &var1.node);

    stack_for_each_safe(n, backup, &stack) {
        ASSERT_FOR_EACH(n, i);
        assert(stack_pop(&stack) == n);
        n = NULL;
        ++i;
    }
    assert(i == 3);
}

TestFunc test_funcs[] = {
    test_stack_init,
    test_stack_peek,
    test_stack_size,
    test_stack_empty,
    test_stack_push,
    test_stack_pop,
    test_stack_remove_all,
    test_stack_entry,
    test_stack_for_each,
    test_stack_for_each_safe
};

int main(int argc, char *argv[]) {
    char msg[100] = "Stack ";
    assert(argc == 2);
    strcat(msg, argv[1]);

    assert(sizeof(test_funcs) / sizeof(TestFunc) == 10);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc), msg, reset_globals);

    return 0;
}
