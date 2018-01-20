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
#include "../src/queue.h"
#include "../src/queue.h"

/* ========================================================================================================
 *
 *                                             TESTING UTILITIES
 *
 * ======================================================================================================== */

typedef struct TestStruct {
    int val;
    QueueNode node;
} TestStruct;

TestStruct var1, var2, var3;
Queue queue;

#define ASSERT_QUEUE(queue, head_ptr, tail_ptr, size_of_queue) \
    do { \
        assert(queue.head == (QueueNode*) (head_ptr)); \
        assert(queue.tail == (QueueNode*) (tail_ptr)); \
        assert(queue.size == size_of_queue); \
    } while (0)

#define ASSERT_NODE(node, next_ptr) \
    do { \
        assert(node.next == (QueueNode*) (next_ptr)); \
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
    queue_init(&queue);

    var1.val = 1;
    var1.node.next = QUEUE_POISON_NEXT;

    var2.val = 2;
    var2.node.next = QUEUE_POISON_NEXT;

    var3.val = 3;
    var3.node.next = QUEUE_POISON_NEXT;
}

/* ========================================================================================================
 *
 *                                             TESTING FUNCTIONS
 *
 * ======================================================================================================== */

void test_queue_init(void) {
    QueueNode node_init_with_macro = QUEUE_NODE_INIT;
    ASSERT_NODE(node_init_with_macro, QUEUE_POISON_NEXT);

    queue_init(&queue);
    ASSERT_QUEUE(queue, NULL, NULL, 0);
}

void test_queue_peek(void) {
    assert(queue_peek(&queue) == NULL);

    queue_push(&queue, &var1.node);
    assert(queue_peek(&queue) == &var1.node);
    queue_push(&queue, &var2.node);
    assert(queue_peek(&queue) == &var1.node);
    queue_push(&queue, &var3.node);
    assert(queue_peek(&queue) == &var1.node);
}

void test_queue_size(void) {
    assert(queue_size(&queue) == 0);

    queue_push(&queue, &var1.node);
    assert(queue_size(&queue) == 1);
    queue_push(&queue, &var2.node);
    assert(queue_size(&queue) == 2);
    queue_push(&queue, &var3.node);
    assert(queue_size(&queue) == 3);
}

void test_queue_empty(void) {
    assert(queue_empty(&queue) == 1);

    queue_push(&queue, &var1.node);
    assert(queue_empty(&queue) == 0);
    queue_push(&queue, &var2.node);
    assert(queue_empty(&queue) == 0);
    queue_push(&queue, &var3.node);
    assert(queue_empty(&queue) == 0);
}

void test_queue_push(void) {
    queue_push(&queue, &var1.node);
    ASSERT_QUEUE(queue, &var1.node, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL);
    queue_push(&queue, &var2.node);
    ASSERT_QUEUE(queue, &var1.node, &var2.node, 2);
    ASSERT_NODE(var1.node, &var2.node);
    ASSERT_NODE(var2.node, NULL);
    queue_push(&queue, &var3.node);
    ASSERT_QUEUE(queue, &var1.node, &var3.node, 3);
    ASSERT_NODE(var1.node, &var2.node);
    ASSERT_NODE(var2.node, &var3.node);
    ASSERT_NODE(var3.node, NULL);
}

void test_queue_pop(void) {
    queue_push(&queue, &var1.node);
    queue_push(&queue, &var2.node);
    queue_push(&queue, &var3.node);
    assert(queue_pop(&queue) == &var1.node);
    ASSERT_QUEUE(queue, &var2.node, &var3.node, 2);
    ASSERT_NODE(var1.node, QUEUE_POISON_NEXT);
    ASSERT_NODE(var2.node, &var3.node);
    ASSERT_NODE(var3.node, NULL);
    assert(queue_pop(&queue) == &var2.node);
    ASSERT_QUEUE(queue, &var3.node, &var3.node, 1);
    ASSERT_NODE(var2.node, QUEUE_POISON_NEXT);
    ASSERT_NODE(var3.node, NULL);
    assert(queue_pop(&queue) == &var3.node);
    ASSERT_QUEUE(queue, NULL, NULL, 0);
    ASSERT_NODE(var3.node, QUEUE_POISON_NEXT);
    assert(queue_pop(&queue) == NULL);
    ASSERT_QUEUE(queue, NULL, NULL, 0);
}

void test_queue_remove_all(void) {
    queue_push(&queue, &var1.node);
    queue_push(&queue, &var2.node);
    queue_push(&queue, &var3.node);
    queue_remove_all(&queue);
    ASSERT_QUEUE(queue, NULL, NULL, 0);
    ASSERT_NODE(var1.node, QUEUE_POISON_NEXT);
    ASSERT_NODE(var3.node, QUEUE_POISON_NEXT);
    queue_remove_all(&queue);
    ASSERT_QUEUE(queue, NULL, NULL, 0);
}

void test_queue_entry(void) {
    assert(queue_entry(&var1.node, TestStruct, node)->val == 1);
    assert(queue_entry(&var1.node, TestStruct, node)->node.next == QUEUE_POISON_NEXT);
}

void test_queue_for_each(void) {
    QueueNode *n;
    size_t i = 0;

    queue_for_each(n, &queue) {
        assert(0);
    }

    queue_push(&queue, &var1.node);
    queue_push(&queue, &var2.node);
    queue_push(&queue, &var3.node);

    queue_for_each(n, &queue) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 3);
}

void test_queue_for_each_safe(void) {
    QueueNode *n, *backup;
    size_t i = 0;

    queue_for_each_safe(n, backup, &queue) {
        assert(0);
    }

    queue_push(&queue, &var1.node);
    queue_push(&queue, &var2.node);
    queue_push(&queue, &var3.node);

    queue_for_each_safe(n, backup, &queue) {
        ASSERT_FOR_EACH(n, i);
        assert(queue_pop(&queue) == n);
        n = NULL;
        ++i;
    }
    assert(i == 3);
}

TestFunc test_funcs[] = {
    test_queue_init,
    test_queue_peek,
    test_queue_size,
    test_queue_empty,
    test_queue_push,
    test_queue_pop,
    test_queue_remove_all,
    test_queue_entry,
    test_queue_for_each,
    test_queue_for_each_safe
};

int main(int argc, char *argv[]) {
    char msg[100] = "Queue ";
    assert(argc == 2);
    strcat(msg, argv[1]);

    assert(sizeof(test_funcs) / sizeof(TestFunc) == 10);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc), msg, reset_globals);

    return 0;
}
