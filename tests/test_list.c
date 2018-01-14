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
#include "../src/list.h"
#include "../src/list.h"

/* ========================================================================================================
 *
 *                                             TESTING UTILITIES
 *
 * ======================================================================================================== */

typedef struct TestStruct {
    int val;
    ListNode node;
} TestStruct;

TestStruct var1, var2, var3, var4, var5;
List list, other_list;

#define ASSERT_LIST(list, head_ptr, tail_ptr, size_of_list) \
    do { \
        assert(list.head == (ListNode*) (head_ptr)); \
        assert(list.tail == (ListNode*) (tail_ptr)); \
        assert(list.size == size_of_list); \
    } while (0)

#define ASSERT_NODE(node, prev_ptr, next_ptr) \
    do { \
        assert(node.prev == (ListNode*) (prev_ptr)); \
        assert(node.next == (ListNode*) (next_ptr)); \
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
            case 3: \
                assert(node_ptr == &var4.node); \
                break; \
            case 4: \
                assert(node_ptr == &var5.node); \
                break; \
            default: \
                assert(0); \
        } \
    } while (0)

static void reset_globals(void) {
    list_init(&list);
    list_init(&other_list);

    var1.val = 1;
    var1.node.prev = LIST_POISON_PREV;
    var1.node.next = LIST_POISON_NEXT;

    var2.val = 2;
    var2.node.prev = LIST_POISON_PREV;
    var2.node.next = LIST_POISON_NEXT;

    var3.val = 3;
    var3.node.prev = LIST_POISON_PREV;
    var3.node.next = LIST_POISON_NEXT;

    var4.val = 4;
    var4.node.prev = LIST_POISON_PREV;
    var4.node.next = LIST_POISON_NEXT;

    var5.val = 5;
    var5.node.prev = LIST_POISON_PREV;
    var5.node.next = LIST_POISON_NEXT;
}

/* ========================================================================================================
 *
 *                                             TESTING FUNCTIONS
 *
 * ======================================================================================================== */

void test_list_init(void) {
    ListNode node_init_with_macro = LIST_NODE_INIT;
    ASSERT_NODE(node_init_with_macro, LIST_POISON_PREV, LIST_POISON_NEXT);

    list_init(&list);
    ASSERT_LIST(list, NULL, NULL, 0);
}

void test_list_front(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    assert(list_front(&list) == &var1.node);
}

void test_list_back(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    assert(list_back(&list) == &var2.node);
}

void test_list_prev(void) {
    assert(list_prev(NULL) == NULL);

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    assert(list_prev(&var1.node) == NULL);
    assert(list_prev(&var2.node) == &var1.node);
}

void test_list_next(void) {
    assert(list_next(NULL) == NULL);

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    assert(list_next(&var1.node) == &var2.node);
    assert(list_next(&var2.node) == NULL);
}

void test_list_size(void) {
    assert(list_size(&list) == 0);
    list_insert_back(&list, &var1.node);
    assert(list_size(&list) == 1);
}

void test_list_empty(void) {
    assert(list_empty(&list) == 1);
    list_insert_back(&list, &var1.node);
    assert(list_empty(&list) == 0);
}

void test_list_index_of(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    assert(list_index_of(&list, &var1.node) == 0);
    assert(list_index_of(&list, &var2.node) == 1);
    assert(list_index_of(&list, &var3.node) == 2);
    assert(list_index_of(&list, &var4.node) == 3);
    assert(list_index_of(&list, &var5.node) == 4);
}

void test_list_at(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    assert(list_at(&list, 0) == &var1.node);
    assert(list_at(&list, 1) == &var2.node);
    assert(list_at(&list, 2) == &var3.node);
    assert(list_at(&list, 3) == &var4.node);
    assert(list_at(&list, 4) == &var5.node);
}

void test_list_insert_left(void) {
    list_insert_left(&list, &var5.node, list.tail);
    list_insert_left(&list, &var1.node, list.tail);
    list_insert_left(&list, &var2.node, list.tail);
    list_insert_left(&list, &var3.node, list.tail);
    list_insert_left(&list, &var4.node, list.tail);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
}

void test_list_insert_right(void) {
    list_insert_right(&list, &var1.node, list.head);
    list_insert_right(&list, &var5.node, list.head);
    list_insert_right(&list, &var4.node, list.head);
    list_insert_right(&list, &var3.node, list.head);
    list_insert_right(&list, &var2.node, list.head);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
}

void test_list_insert_front(void) {
    list_insert_front(&list, &var5.node);
    list_insert_front(&list, &var4.node);
    list_insert_front(&list, &var3.node);
    list_insert_front(&list, &var2.node);
    list_insert_front(&list, &var1.node);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
}

void test_list_insert_back(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
}

void test_list_splice_left(void) {
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    list_insert_back(&other_list, &var1.node);
    list_insert_back(&other_list, &var2.node);
    list_insert_back(&other_list, &var3.node);
    list_splice_left(&list, &other_list, list.head);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
    ASSERT_LIST(other_list, NULL, NULL, 0);
}

void test_list_splice_right(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&other_list, &var3.node);
    list_insert_back(&other_list, &var4.node);
    list_insert_back(&other_list, &var5.node);
    list_splice_right(&list, &other_list, list.tail);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
    ASSERT_LIST(other_list, NULL, NULL, 0);
}

void test_list_splice_front(void) {
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    list_insert_back(&other_list, &var1.node);
    list_insert_back(&other_list, &var2.node);
    list_insert_back(&other_list, &var3.node);
    list_splice_front(&list, &other_list);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
    ASSERT_LIST(other_list, NULL, NULL, 0);
}

void test_list_splice_back(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&other_list, &var3.node);
    list_insert_back(&other_list, &var4.node);
    list_insert_back(&other_list, &var5.node);
    list_splice_back(&list, &other_list);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
    ASSERT_LIST(other_list, NULL, NULL, 0);
}

void test_list_remove(void) {
    list_insert_back(&list, &var5.node);
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_remove(&list, &var5.node);
    list_remove(&list, &var4.node);
    list_remove(&list, &var3.node);
    list_remove(&list, NULL);
    ASSERT_NODE(var3.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_NODE(var4.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_NODE(var5.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_LIST(list, &var1.node, &var2.node, 2);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, NULL);
}

void test_list_remove_front(void) {
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var1.node);
    list_remove_front(&list);
    ASSERT_NODE(var2.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_LIST(list, &var1.node, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL, NULL);
    list_remove_front(&list);
    ASSERT_NODE(var1.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_LIST(list, NULL, NULL, 0);
    list_remove_front(&list);
}

void test_list_remove_back(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_remove_back(&list);
    ASSERT_NODE(var2.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_LIST(list, &var1.node, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL, NULL);
    list_remove_back(&list);
    ASSERT_NODE(var1.node, LIST_POISON_PREV, LIST_POISON_NEXT);
    ASSERT_LIST(list, NULL, NULL, 0);
    list_remove_back(&list);
}

void test_list_remove_all(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    list_remove_all(&list);
    ASSERT_NODE(var1.node, LIST_POISON_PREV, &var2.node);
    ASSERT_NODE(var5.node, &var4.node, LIST_POISON_NEXT);
    ASSERT_LIST(list, NULL, NULL, 0);
    list_remove_all(&list);
}

void test_list_cut(void) {
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);
    list_cut(&list, &var3.node, &var5.node, 3);
    ASSERT_NODE(var3.node, LIST_POISON_PREV, &var4.node);
    ASSERT_NODE(var5.node, &var4.node, LIST_POISON_NEXT);
    ASSERT_LIST(list, &var1.node, &var2.node, 2);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, NULL);
}

void test_list_paste(void) {
    list_insert_back(&other_list, &var1.node);
    list_insert_back(&other_list, &var2.node);
    list_insert_back(&other_list, &var3.node);
    list_insert_back(&other_list, &var4.node);
    list_insert_back(&list, &var5.node);
    list_cut(&other_list, other_list.head, other_list.tail, other_list.size);
    list_paste(&list, NULL, &var1.node, &var4.node, &var5.node, 4);
    ASSERT_LIST(other_list, NULL, NULL, 0);
    ASSERT_LIST(list, &var1.node, &var5.node, 5);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var5.node);
    ASSERT_NODE(var5.node, &var4.node, NULL);
}

static int cmp(const ListNode *a, const ListNode *b) {
    return list_entry(a, TestStruct, node)->val - list_entry(b, TestStruct, node)->val;
}

void test_list_sort(void) {
    TestStruct var4cpy = var4;

    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var5.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var4cpy.node);
    list_insert_back(&list, &var3.node);
    list_sort(&list, cmp);
    ASSERT_LIST(list, &var1.node, &var5.node, 6);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var4cpy.node);
    ASSERT_NODE(var4cpy.node, &var4.node, &var5.node);
    ASSERT_NODE(var5.node, &var4cpy.node, NULL);
    list_sort(&list, cmp);
    ASSERT_LIST(list, &var1.node, &var5.node, 6);
    ASSERT_NODE(var1.node, NULL, &var2.node);
    ASSERT_NODE(var2.node, &var1.node, &var3.node);
    ASSERT_NODE(var3.node, &var2.node, &var4.node);
    ASSERT_NODE(var4.node, &var3.node, &var4cpy.node);
    ASSERT_NODE(var4cpy.node, &var4.node, &var5.node);
    ASSERT_NODE(var5.node, &var4cpy.node, NULL);
}

void test_list_entry(void) {
    assert(list_entry(&var1.node, TestStruct, node)->val == 1);
    assert(list_entry(&var1.node, TestStruct, node)->node.prev == LIST_POISON_PREV);
    assert(list_entry(&var1.node, TestStruct, node)->node.next == LIST_POISON_NEXT);
}

void test_list_for_each(void) {
    ListNode *n;
    size_t i = 0;

    list_for_each(n, &list) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    list_for_each(n, &list) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 5);
}

void test_list_for_each_reverse(void) {
    ListNode *n;
    size_t i = 5;

    list_for_each_reverse(n, &list) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    list_for_each_reverse(n, &list) {
        --i;
        ASSERT_FOR_EACH(n, i);
    }
    assert(i == 0);
}

void test_list_for_each_safe(void) {
    ListNode *n, *backup;
    size_t i = 0;

    list_for_each_safe(n, backup, &list) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    list_for_each_safe(n, backup, &list) {
        ASSERT_FOR_EACH(n, i);
        list_remove(&list, n);
        n = NULL;
        ++i;
    }
    assert(i == 5);
}

void test_list_for_each_safe_reverse(void) {
    ListNode *n, *backup;
    size_t i = 5;

    list_for_each_safe_reverse(n, backup, &list) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    list_for_each_safe_reverse(n, backup, &list) {
        --i;
        ASSERT_FOR_EACH(n, i);
        list_remove(&list, n);
        n = NULL;
    }
    assert(i == 0);
}

void test_list_for_each_after(void) {
    ListNode *n = NULL;
    size_t i = 1;

    list_for_each_after(n) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.head;
    list_for_each_after(n) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 5);
}

void test_list_for_each_after_reverse(void) {
    ListNode *n = NULL;
    size_t i = 4;

    list_for_each_after_reverse(n) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.tail;
    list_for_each_after_reverse(n) {
        --i;
        ASSERT_FOR_EACH(n, i);
    }
    assert(i == 0);
}

void test_list_for_each_safe_after(void) {
    ListNode *n = NULL, *backup;
    size_t i = 1;

    list_for_each_safe_after(n, backup) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.head;
    list_for_each_safe_after(n, backup) {
        ASSERT_FOR_EACH(n, i);
        list_remove(&list, n);
        n = NULL;
        ++i;
    }
    assert(i == 5);
}

void test_list_for_each_safe_after_reverse(void) {
    ListNode *n = NULL, *backup;
    size_t i = 4;

    list_for_each_safe_after_reverse(n, backup) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.tail;
    list_for_each_safe_after_reverse(n, backup) {
        --i;
        ASSERT_FOR_EACH(n, i);
        list_remove(&list, n);
        n = NULL;
    }
    assert(i == 0);
}

void test_list_for_each_from(void) {
    ListNode *n = NULL;
    size_t i = 0;

    list_for_each_from(n) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.head;
    list_for_each_from(n) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 5);
}

void test_list_for_each_from_reverse(void) {
    ListNode *n = NULL;
    size_t i = 5;

    list_for_each_from_reverse(n) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.tail;
    list_for_each_from_reverse(n) {
        --i;
        ASSERT_FOR_EACH(n, i);
    }
    assert(i == 0);
}

void test_list_for_each_safe_from(void) {
    ListNode *n = NULL, *backup;
    size_t i = 0;

    list_for_each_safe_from(n, backup) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.head;
    list_for_each_safe_from(n, backup) {
        ASSERT_FOR_EACH(n, i);
        list_remove(&list, n);
        n = NULL;
        ++i;
    }
    assert(i == 5);
}

void test_list_for_each_safe_from_reverse(void) {
    ListNode *n = NULL, *backup;
    size_t i = 5;

    list_for_each_safe_from_reverse(n, backup) {
        assert(0);
    }

    list_insert_back(&list, &var1.node);
    list_insert_back(&list, &var2.node);
    list_insert_back(&list, &var3.node);
    list_insert_back(&list, &var4.node);
    list_insert_back(&list, &var5.node);

    n = list.tail;
    list_for_each_safe_from_reverse(n, backup) {
        --i;
        ASSERT_FOR_EACH(n, i);
        list_remove(&list, n);
        n = NULL;
    }
    assert(i == 0);
}

TestFunc test_funcs[] = {
    test_list_init,
    test_list_front,
    test_list_back,
    test_list_prev,
    test_list_next,
    test_list_size,
    test_list_empty,
    test_list_index_of,
    test_list_at,
    test_list_insert_left,
    test_list_insert_right,
    test_list_insert_front,
    test_list_insert_back,
    test_list_splice_left,
    test_list_splice_right,
    test_list_splice_front,
    test_list_splice_back,
    test_list_remove,
    test_list_remove_front,
    test_list_remove_back,
    test_list_remove_all,
    test_list_cut,
    test_list_paste,
    test_list_sort,
    test_list_entry,
    test_list_for_each,
    test_list_for_each_reverse,
    test_list_for_each_safe,
    test_list_for_each_safe_reverse,
    test_list_for_each_after,
    test_list_for_each_after_reverse,
    test_list_for_each_safe_after,
    test_list_for_each_safe_after_reverse,
    test_list_for_each_from,
    test_list_for_each_from_reverse,
    test_list_for_each_safe_from,
    test_list_for_each_safe_from_reverse
};

int main(int argc, char *argv[]) {
    char msg[100] = "List ";
    assert(argc == 2);
    strcat(msg, argv[1]);

    assert(sizeof(test_funcs) / sizeof(TestFunc) == 37);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc), msg, reset_globals);

    return 0;
}
