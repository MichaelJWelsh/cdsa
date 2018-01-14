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
#include "../src/rbtree.h"
#include "../src/rbtree.h"

/* ========================================================================================================
 *
 *                                             TESTING UTILITIES
 *
 * ======================================================================================================== */

typedef struct TestStruct {
    int key;
    int num_similar_keys;
    RBTreeNode node;
} TestStruct;

TestStruct var1, var2, var3, var4, var5, var6, var7;
RBTree rbtree;
size_t counter;
void *aux_ptr;

#define ASSERT_RBTREE(rbtree, root_ptr, size_of_rbtree) \
    do { \
        assert(rbtree.root == (RBTreeNode*) (root_ptr)); \
        assert(rbtree.size == size_of_rbtree); \
    } while (0)

#define ASSERT_NODE(node, parent_ptr, left_child_ptr, right_child_ptr, node_color) \
    do { \
        assert((node).parent == (RBTreeNode*) (parent_ptr)); \
        assert((node).left_child == (RBTreeNode*) (left_child_ptr)); \
        assert((node).right_child == (RBTreeNode*) (right_child_ptr)); \
        assert((node).color == node_color); \
    } while (0)

#define ASSERT_INORDERNESS(rbtree) \
    do { \
        assert(rbtree_first(&rbtree) == &var1.node); \
        \
        assert(rbtree_last(&rbtree) == &var7.node); \
        \
        assert(rbtree_next(&var1.node) == &var2.node); \
        assert(rbtree_next(&var2.node) == &var3.node); \
        assert(rbtree_next(&var3.node) == &var4.node); \
        assert(rbtree_next(&var4.node) == &var5.node); \
        assert(rbtree_next(&var5.node) == &var6.node); \
        assert(rbtree_next(&var6.node) == &var7.node); \
        assert(rbtree_next(&var7.node) == NULL); \
        \
        assert(rbtree_prev(&var7.node) == &var6.node); \
        assert(rbtree_prev(&var6.node) == &var5.node); \
        assert(rbtree_prev(&var5.node) == &var4.node); \
        assert(rbtree_prev(&var4.node) == &var3.node); \
        assert(rbtree_prev(&var3.node) == &var2.node); \
        assert(rbtree_prev(&var2.node) == &var1.node); \
        assert(rbtree_prev(&var1.node) == NULL); \
    } while (0)

static RBTreeNodeColor color_(RBTreeNode *node) {
    return node ? node->color : RBTREE_NODE_BLACK;
}

static void p1_(RBTree *rbtree) {
    assert(color_(rbtree->root) == RBTREE_NODE_BLACK);
}

static void p2_(RBTreeNode *node) {
    if (color_(node) == RBTREE_NODE_RED) {
        assert(color_(node->parent) == RBTREE_NODE_BLACK);
        assert(color_(node->left_child) == RBTREE_NODE_BLACK);
        assert(color_(node->right_child) == RBTREE_NODE_BLACK);
    }

    if (!node) {
        return;
    }

    p2_(node->left_child);
    p2_(node->right_child);
}

static void p3_helper_(RBTreeNode *node, int black_count, int *black_count_path) {
    if (color_(node) == RBTREE_NODE_BLACK) {
        ++black_count;
    }

    if (!node) {
        if (*black_count_path == -1) {
            *black_count_path = black_count;
        } else {
            assert(black_count == *black_count_path);
        }

        return;
    }

    p3_helper_(node->left_child, black_count, black_count_path);
    p3_helper_(node->right_child, black_count, black_count_path);
}

static void p3_(RBTreeNode *node) {
    int black_count_path = -1;

    p3_helper_(node, 0, &black_count_path);
}

#define ASSERT_PROPERTIES(rbtree) \
    do { \
        p1_(&rbtree); \
        p2_(rbtree.root); \
        p3_(rbtree.root); \
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
            case 5: \
                assert(node_ptr == &var6.node); \
                break; \
            case 6: \
                assert(node_ptr == &var7.node); \
                break; \
            default: \
                assert(0); \
        } \
    } while (0)

#define FILL_SEQUENTIALLY(rbtree) \
    do { \
        rbtree_insert(&rbtree, &var1.key, &var1.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var2.key, &var2.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var3.key, &var3.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var4.key, &var4.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var5.key, &var5.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var6.key, &var6.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var7.key, &var7.node); \
        ASSERT_PROPERTIES(rbtree); \
    } while (0)

#define FILL_SEQUENTIALLY_REVERSE(rbtree) \
    do { \
        rbtree_insert(&rbtree, &var7.key, &var7.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var6.key, &var6.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var5.key, &var5.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var4.key, &var4.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var3.key, &var3.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var2.key, &var2.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_insert(&rbtree, &var1.key, &var1.node); \
        ASSERT_PROPERTIES(rbtree); \
    } while (0)

#define FILL_RANDOMLY(rbtree) \
    { \
        int vars_used[7] = { 0, 0, 0, 0, 0, 0, 0 }; \
        \
        while ( \
            !vars_used[0] || \
            !vars_used[1] || \
            !vars_used[2] || \
            !vars_used[3] || \
            !vars_used[4] || \
            !vars_used[5] || \
            !vars_used[6] \
        ) { \
            int x = rand() % 7; \
            \
            while (vars_used[x]) { \
                x = rand() % 7; \
            } \
            \
            vars_used[x] = 1; \
            \
            switch (x + 1) { \
                case 1: \
                    rbtree_insert(&rbtree, &var1.key, &var1.node); \
                    break; \
                case 2: \
                    rbtree_insert(&rbtree, &var2.key, &var2.node); \
                    break; \
                case 3: \
                    rbtree_insert(&rbtree, &var3.key, &var3.node); \
                    break; \
                case 4: \
                    rbtree_insert(&rbtree, &var4.key, &var4.node); \
                    break; \
                case 5: \
                    rbtree_insert(&rbtree, &var5.key, &var5.node); \
                    break; \
                case 6: \
                    rbtree_insert(&rbtree, &var6.key, &var6.node); \
                    break; \
                case 7: \
                    rbtree_insert(&rbtree, &var7.key, &var7.node); \
                    break; \
                default: \
                    assert(0); \
            } \
            \
            ASSERT_PROPERTIES(rbtree); \
        } \
    }

#define DRAIN_SEQUENTIALLY(rbtree) \
    do { \
        rbtree_remove(&rbtree, &var1.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var2.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var3.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var4.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var5.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var6.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var7.node); \
        ASSERT_PROPERTIES(rbtree); \
    } while (0)

#define DRAIN_SEQUENTIALLY_REVERSE(rbtree) \
    do { \
        rbtree_remove(&rbtree, &var7.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var6.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var5.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var4.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var3.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var2.node); \
        ASSERT_PROPERTIES(rbtree); \
        rbtree_remove(&rbtree, &var1.node); \
        ASSERT_PROPERTIES(rbtree); \
    } while (0)

#define DRAIN_RANDOMLY(rbtree) \
    { \
        int vars_used[7] = { 0, 0, 0, 0, 0, 0, 0 }; \
        \
        while ( \
            !vars_used[0] || \
            !vars_used[1] || \
            !vars_used[2] || \
            !vars_used[3] || \
            !vars_used[4] || \
            !vars_used[5] || \
            !vars_used[6] \
        ) { \
            int x = rand() % 7; \
            \
            while (vars_used[x]) { \
                x = rand() % 7; \
            } \
            \
            vars_used[x] = 1; \
            \
            switch (x + 1) { \
                case 1: \
                    rbtree_remove(&rbtree, &var1.node); \
                    break; \
                case 2: \
                    rbtree_remove(&rbtree, &var2.node); \
                    break; \
                case 3: \
                    rbtree_remove(&rbtree, &var3.node); \
                    break; \
                case 4: \
                    rbtree_remove(&rbtree, &var4.node); \
                    break; \
                case 5: \
                    rbtree_remove(&rbtree, &var5.node); \
                    break; \
                case 6: \
                    rbtree_remove(&rbtree, &var6.node); \
                    break; \
                case 7: \
                    rbtree_remove(&rbtree, &var7.node); \
                    break; \
                default: \
                    assert(0); \
            } \
            \
            ASSERT_PROPERTIES(rbtree); \
        } \
    }

#define loop \
    for (counter = 0; counter < 5000; ++counter)

static int compare_func(const void *key, const RBTreeNode *node) {
    return *(const int*)key - rbtree_entry(node, TestStruct, node)->key;
}

static void collide_func(const RBTreeNode *old_node, const RBTreeNode *new_node, void *auxiliary_data) {
    ASSERT_NODE(*old_node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, old_node->color);
    assert((void**) auxiliary_data == &aux_ptr);

    rbtree_entry(new_node, TestStruct, node)->num_similar_keys += 1 + rbtree_entry(old_node, TestStruct, node)->num_similar_keys;
}

static void reset_globals(void) {
    rbtree_init(&rbtree, compare_func, collide_func, &aux_ptr);

    var1.key = 1;
    var1.num_similar_keys = 0;
    var1.node.parent = RBTREE_POISON_PARENT;
    var1.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var1.node.right_child = RBTREE_POISON_RIGHT_CHILD;

    var2.key = 2;
    var2.num_similar_keys = 0;
    var2.node.parent = RBTREE_POISON_PARENT;
    var2.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var2.node.right_child = RBTREE_POISON_RIGHT_CHILD;

    var3.key = 3;
    var3.num_similar_keys = 0;
    var3.node.parent = RBTREE_POISON_PARENT;
    var3.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var3.node.right_child = RBTREE_POISON_RIGHT_CHILD;

    var4.key = 4;
    var4.num_similar_keys = 0;
    var4.node.parent = RBTREE_POISON_PARENT;
    var4.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var4.node.right_child = RBTREE_POISON_RIGHT_CHILD;

    var5.key = 5;
    var5.num_similar_keys = 0;
    var5.node.parent = RBTREE_POISON_PARENT;
    var5.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var5.node.right_child = RBTREE_POISON_RIGHT_CHILD;

    var6.key = 6;
    var6.num_similar_keys = 0;
    var6.node.parent = RBTREE_POISON_PARENT;
    var6.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var6.node.right_child = RBTREE_POISON_RIGHT_CHILD;

    var7.key = 7;
    var7.num_similar_keys = 0;
    var7.node.parent = RBTREE_POISON_PARENT;
    var7.node.left_child = RBTREE_POISON_LEFT_CHILD;
    var7.node.right_child = RBTREE_POISON_RIGHT_CHILD;
}

/* ========================================================================================================
 *
 *                                             TESTING FUNCTIONS
 *
 * ======================================================================================================== */

void test_rbtree_init(void) {
    RBTreeNode node_init_with_macro = RBTREE_NODE_INIT;
    ASSERT_NODE(node_init_with_macro, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, RBTREE_NODE_RED);

    rbtree_init(&rbtree, compare_func, collide_func, &aux_ptr);
    ASSERT_RBTREE(rbtree, NULL, 0);
    assert(rbtree.compare == compare_func);
    assert(rbtree.collide == collide_func);
    assert((void**) rbtree.auxiliary_data == &aux_ptr);
    rbtree_init(&rbtree, compare_func, NULL, NULL);
    ASSERT_RBTREE(rbtree, NULL, 0);
    assert(rbtree.compare == compare_func);
    assert(rbtree.collide == NULL);
    assert(rbtree.auxiliary_data == NULL);
}

void test_rbtree_first(void) {
    assert(rbtree_first(&rbtree) == NULL);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_first(&rbtree) == &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_first(&rbtree) == &var1.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_first(&rbtree) == &var3.node);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_first(&rbtree) == &var1.node);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_first(&rbtree) == &var1.node);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_first(&rbtree) == &var1.node);
        reset_globals();
    }
}

void test_rbtree_last(void) {
    assert(rbtree_last(&rbtree) == NULL);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_last(&rbtree) == &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_last(&rbtree) == &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_last(&rbtree) == &var2.node);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_last(&rbtree) == &var7.node);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_last(&rbtree) == &var7.node);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_last(&rbtree) == &var7.node);
        reset_globals();
    }
}

void test_rbtree_prev(void) {
    assert(rbtree_prev(NULL) == NULL);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_prev(&var1.node) == NULL);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_prev(&var1.node) == NULL);
    assert(rbtree_prev(&var2.node) == &var1.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_prev(&var3.node) == NULL);
    assert(rbtree_prev(&var2.node) == &var3.node);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_prev(&var7.node) == &var6.node);
    assert(rbtree_prev(&var6.node) == &var5.node);
    assert(rbtree_prev(&var5.node) == &var4.node);
    assert(rbtree_prev(&var4.node) == &var3.node);
    assert(rbtree_prev(&var3.node) == &var2.node);
    assert(rbtree_prev(&var2.node) == &var1.node);
    assert(rbtree_prev(&var1.node) == NULL);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_prev(&var7.node) == &var6.node);
    assert(rbtree_prev(&var6.node) == &var5.node);
    assert(rbtree_prev(&var5.node) == &var4.node);
    assert(rbtree_prev(&var4.node) == &var3.node);
    assert(rbtree_prev(&var3.node) == &var2.node);
    assert(rbtree_prev(&var2.node) == &var1.node);
    assert(rbtree_prev(&var1.node) == NULL);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_prev(&var7.node) == &var6.node);
        assert(rbtree_prev(&var6.node) == &var5.node);
        assert(rbtree_prev(&var5.node) == &var4.node);
        assert(rbtree_prev(&var4.node) == &var3.node);
        assert(rbtree_prev(&var3.node) == &var2.node);
        assert(rbtree_prev(&var2.node) == &var1.node);
        assert(rbtree_prev(&var1.node) == NULL);
        reset_globals();
    }
}

void test_rbtree_next(void) {
    assert(rbtree_next(NULL) == NULL);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_next(&var1.node) == NULL);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_next(&var1.node) == &var2.node);
    assert(rbtree_next(&var2.node) == NULL);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_next(&var3.node) == &var2.node);
    assert(rbtree_next(&var2.node) == NULL);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_next(&var1.node) == &var2.node);
    assert(rbtree_next(&var2.node) == &var3.node);
    assert(rbtree_next(&var3.node) == &var4.node);
    assert(rbtree_next(&var4.node) == &var5.node);
    assert(rbtree_next(&var5.node) == &var6.node);
    assert(rbtree_next(&var6.node) == &var7.node);
    assert(rbtree_next(&var7.node) == NULL);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_next(&var1.node) == &var2.node);
    assert(rbtree_next(&var2.node) == &var3.node);
    assert(rbtree_next(&var3.node) == &var4.node);
    assert(rbtree_next(&var4.node) == &var5.node);
    assert(rbtree_next(&var5.node) == &var6.node);
    assert(rbtree_next(&var6.node) == &var7.node);
    assert(rbtree_next(&var7.node) == NULL);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_next(&var1.node) == &var2.node);
        assert(rbtree_next(&var2.node) == &var3.node);
        assert(rbtree_next(&var3.node) == &var4.node);
        assert(rbtree_next(&var4.node) == &var5.node);
        assert(rbtree_next(&var5.node) == &var6.node);
        assert(rbtree_next(&var6.node) == &var7.node);
        assert(rbtree_next(&var7.node) == NULL);
        reset_globals();
    }
}

void test_rbtree_size(void) {
    assert(rbtree_size(&rbtree) == 0);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_size(&rbtree) == 1);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_size(&rbtree) == 2);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_size(&rbtree) == 2);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_size(&rbtree) == 7);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_size(&rbtree) == 7);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_size(&rbtree) == 7);
        reset_globals();
    }
}

void test_rbtree_empty(void) {
    assert(rbtree_empty(&rbtree) == 1);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_empty(&rbtree) == 0);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_empty(&rbtree) == 0);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_empty(&rbtree) == 0);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_empty(&rbtree) == 0);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_empty(&rbtree) == 0);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_empty(&rbtree) == 0);
        reset_globals();
    }
}

void test_rbtree_contains_key(void) {
    assert(rbtree_contains_key(&rbtree, &var1.key) == 0);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_contains_key(&rbtree, &var1.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var2.key) == 0);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_contains_key(&rbtree, &var2.key) == 1);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_contains_key(&rbtree, &var3.key) == 1);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_contains_key(&rbtree, &var1.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var2.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var3.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var4.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var5.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var6.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var7.key) == 1);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_contains_key(&rbtree, &var1.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var2.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var3.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var4.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var5.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var6.key) == 1);
    assert(rbtree_contains_key(&rbtree, &var7.key) == 1);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_contains_key(&rbtree, &var1.key) == 1);
        assert(rbtree_contains_key(&rbtree, &var2.key) == 1);
        assert(rbtree_contains_key(&rbtree, &var3.key) == 1);
        assert(rbtree_contains_key(&rbtree, &var4.key) == 1);
        assert(rbtree_contains_key(&rbtree, &var5.key) == 1);
        assert(rbtree_contains_key(&rbtree, &var6.key) == 1);
        assert(rbtree_contains_key(&rbtree, &var7.key) == 1);
        reset_globals();
    }
}

void test_rbtree_index_of(void) {
    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_index_of(&rbtree, &var1.node) == 0);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_index_of(&rbtree, &var2.node) == 1);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_index_of(&rbtree, &var3.node) == 0);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_index_of(&rbtree, &var1.node) == 0);
    assert(rbtree_index_of(&rbtree, &var2.node) == 1);
    assert(rbtree_index_of(&rbtree, &var3.node) == 2);
    assert(rbtree_index_of(&rbtree, &var4.node) == 3);
    assert(rbtree_index_of(&rbtree, &var5.node) == 4);
    assert(rbtree_index_of(&rbtree, &var6.node) == 5);
    assert(rbtree_index_of(&rbtree, &var7.node) == 6);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_index_of(&rbtree, &var1.node) == 0);
    assert(rbtree_index_of(&rbtree, &var2.node) == 1);
    assert(rbtree_index_of(&rbtree, &var3.node) == 2);
    assert(rbtree_index_of(&rbtree, &var4.node) == 3);
    assert(rbtree_index_of(&rbtree, &var5.node) == 4);
    assert(rbtree_index_of(&rbtree, &var6.node) == 5);
    assert(rbtree_index_of(&rbtree, &var7.node) == 6);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_index_of(&rbtree, &var1.node) == 0);
        assert(rbtree_index_of(&rbtree, &var2.node) == 1);
        assert(rbtree_index_of(&rbtree, &var3.node) == 2);
        assert(rbtree_index_of(&rbtree, &var4.node) == 3);
        assert(rbtree_index_of(&rbtree, &var5.node) == 4);
        assert(rbtree_index_of(&rbtree, &var6.node) == 5);
        assert(rbtree_index_of(&rbtree, &var7.node) == 6);
        reset_globals();
    }
}

void test_rbtree_at(void) {
    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_at(&rbtree, 0) == &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_at(&rbtree, 1) == &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_at(&rbtree, 0) == &var3.node);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_at(&rbtree, 0) == &var1.node);
    assert(rbtree_at(&rbtree, 1) == &var2.node);
    assert(rbtree_at(&rbtree, 2) == &var3.node);
    assert(rbtree_at(&rbtree, 3) == &var4.node);
    assert(rbtree_at(&rbtree, 4) == &var5.node);
    assert(rbtree_at(&rbtree, 5) == &var6.node);
    assert(rbtree_at(&rbtree, 6) == &var7.node);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_at(&rbtree, 0) == &var1.node);
    assert(rbtree_at(&rbtree, 1) == &var2.node);
    assert(rbtree_at(&rbtree, 2) == &var3.node);
    assert(rbtree_at(&rbtree, 3) == &var4.node);
    assert(rbtree_at(&rbtree, 4) == &var5.node);
    assert(rbtree_at(&rbtree, 5) == &var6.node);
    assert(rbtree_at(&rbtree, 6) == &var7.node);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_at(&rbtree, 0) == &var1.node);
        assert(rbtree_at(&rbtree, 1) == &var2.node);
        assert(rbtree_at(&rbtree, 2) == &var3.node);
        assert(rbtree_at(&rbtree, 3) == &var4.node);
        assert(rbtree_at(&rbtree, 4) == &var5.node);
        assert(rbtree_at(&rbtree, 5) == &var6.node);
        assert(rbtree_at(&rbtree, 6) == &var7.node);
        reset_globals();
    }
}

void test_rbtree_insert(void) {
    rbtree.collide = NULL;
    rbtree_insert(&rbtree, &var1.key, &var1.node);
    ASSERT_RBTREE(rbtree, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    ASSERT_RBTREE(rbtree, &var1.node, 2);
    ASSERT_NODE(var1.node, NULL, NULL, &var2.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, &var1.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    ASSERT_RBTREE(rbtree, &var3.node, 2);
    ASSERT_NODE(var1.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var1.node.color);
    ASSERT_NODE(var2.node, &var3.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var3.node, NULL, NULL, &var2.node, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var1.key, &var1.node);
    ASSERT_RBTREE(rbtree, &var1.node, 2);
    ASSERT_NODE(var1.node, NULL, NULL, &var2.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, &var1.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var3.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var3.node.color);
    ASSERT_PROPERTIES(rbtree);
    var3.key = 3;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    ASSERT_RBTREE(rbtree, &var2.node, 3);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var3.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var4.key, &var4.node);
    ASSERT_RBTREE(rbtree, &var2.node, 4);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var3.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var2.node, NULL, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var3.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var5.key, &var5.node);
    ASSERT_RBTREE(rbtree, &var2.node, 5);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, &var5.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var6.key, &var6.node);
    ASSERT_RBTREE(rbtree, &var2.node, 6);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, &var5.node, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, &var4.node, NULL, &var6.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, &var5.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var7.key, &var7.node);
    ASSERT_RBTREE(rbtree, &var2.node, 7);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, &var6.node, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, &var4.node, &var5.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    ASSERT_INORDERNESS(rbtree);
    reset_globals();

    rbtree_insert(&rbtree, &var7.key, &var7.node);
    ASSERT_RBTREE(rbtree, &var7.node, 1);
    ASSERT_NODE(var7.node, NULL, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var6.key, &var6.node);
    ASSERT_RBTREE(rbtree, &var7.node, 2);
    ASSERT_NODE(var7.node, NULL, &var6.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, &var7.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    var5.key = var7.key;
    rbtree_insert(&rbtree, &var5.key, &var5.node);
    assert(var5.num_similar_keys == 1);
    ASSERT_RBTREE(rbtree, &var5.node, 2);
    ASSERT_NODE(var7.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var7.node.color);
    ASSERT_NODE(var6.node, &var5.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, NULL, &var6.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var7.key, &var7.node);
    assert(var7.num_similar_keys == 2);
    ASSERT_RBTREE(rbtree, &var7.node, 2);
    ASSERT_NODE(var7.node, NULL, &var6.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, &var7.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var5.node.color);
    ASSERT_PROPERTIES(rbtree);
    var5.key = 5;
    rbtree_insert(&rbtree, &var5.key, &var5.node);
    ASSERT_RBTREE(rbtree, &var6.node, 3);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, NULL, &var5.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var4.key, &var4.node);
    ASSERT_RBTREE(rbtree, &var6.node, 4);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, NULL, &var5.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var6.node, &var4.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var5.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    ASSERT_RBTREE(rbtree, &var6.node, 5);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, NULL, &var4.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, &var6.node, &var3.node, &var5.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    ASSERT_RBTREE(rbtree, &var6.node, 6);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, NULL, &var4.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var6.node, &var3.node, &var5.node, RBTREE_NODE_RED);
    ASSERT_NODE(var3.node, &var4.node, &var2.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, &var3.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_insert(&rbtree, &var1.key, &var1.node);
    ASSERT_RBTREE(rbtree, &var6.node, 7);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, NULL, &var4.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var6.node, &var2.node, &var5.node, RBTREE_NODE_RED);
    ASSERT_NODE(var3.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var2.node, &var4.node, &var1.node, &var3.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    ASSERT_INORDERNESS(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    ASSERT_RBTREE(rbtree, &var2.node, 7);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, &var6.node, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, &var4.node, &var5.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_INORDERNESS(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    ASSERT_RBTREE(rbtree, &var6.node, 7);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var2.node, &var4.node, &var1.node, &var3.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, &var6.node, &var2.node, &var5.node, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var6.node, NULL, &var4.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_INORDERNESS(rbtree);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        ASSERT_INORDERNESS(rbtree);
        reset_globals();
    }
}

void test_rbtree_lookup_key(void) {
    assert(rbtree_lookup_key(&rbtree, &var1.key) == NULL);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    assert(rbtree_lookup_key(&rbtree, &var1.key) == &var1.node);
    assert(rbtree_lookup_key(&rbtree, &var2.key) == NULL);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    assert(rbtree_lookup_key(&rbtree, &var2.key) == &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    assert(rbtree_lookup_key(&rbtree, &var3.key) == &var3.node);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    assert(rbtree_lookup_key(&rbtree, &var1.key) == &var1.node);
    assert(rbtree_lookup_key(&rbtree, &var2.key) == &var2.node);
    assert(rbtree_lookup_key(&rbtree, &var3.key) == &var3.node);
    assert(rbtree_lookup_key(&rbtree, &var4.key) == &var4.node);
    assert(rbtree_lookup_key(&rbtree, &var5.key) == &var5.node);
    assert(rbtree_lookup_key(&rbtree, &var6.key) == &var6.node);
    assert(rbtree_lookup_key(&rbtree, &var7.key) == &var7.node);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    assert(rbtree_lookup_key(&rbtree, &var1.key) == &var1.node);
    assert(rbtree_lookup_key(&rbtree, &var2.key) == &var2.node);
    assert(rbtree_lookup_key(&rbtree, &var3.key) == &var3.node);
    assert(rbtree_lookup_key(&rbtree, &var4.key) == &var4.node);
    assert(rbtree_lookup_key(&rbtree, &var5.key) == &var5.node);
    assert(rbtree_lookup_key(&rbtree, &var6.key) == &var6.node);
    assert(rbtree_lookup_key(&rbtree, &var7.key) == &var7.node);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        assert(rbtree_lookup_key(&rbtree, &var1.key) == &var1.node);
        assert(rbtree_lookup_key(&rbtree, &var2.key) == &var2.node);
        assert(rbtree_lookup_key(&rbtree, &var3.key) == &var3.node);
        assert(rbtree_lookup_key(&rbtree, &var4.key) == &var4.node);
        assert(rbtree_lookup_key(&rbtree, &var5.key) == &var5.node);
        assert(rbtree_lookup_key(&rbtree, &var6.key) == &var6.node);
        assert(rbtree_lookup_key(&rbtree, &var7.key) == &var7.node);
        reset_globals();
    }
}

void test_rbtree_remove(void) {
    rbtree_remove(&rbtree, NULL);
    ASSERT_RBTREE(rbtree, NULL, 0);

    FILL_SEQUENTIALLY(rbtree);
    rbtree_remove(&rbtree, &var1.node);
    ASSERT_RBTREE(rbtree, &var4.node, 6);
    ASSERT_NODE(var1.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var1.node.color);
    ASSERT_NODE(var2.node, &var4.node, NULL, &var3.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, NULL, &var2.node, &var6.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, &var4.node, &var5.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var2.node);
    ASSERT_RBTREE(rbtree, &var6.node, 5);
    ASSERT_NODE(var2.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var2.node.color);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, &var6.node, &var3.node, &var5.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, NULL, &var4.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var3.node);
    ASSERT_RBTREE(rbtree, &var6.node, 4);
    ASSERT_NODE(var3.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var3.node.color);
    ASSERT_NODE(var4.node, &var6.node, NULL, &var5.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, NULL, &var4.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var4.node);
    ASSERT_RBTREE(rbtree, &var6.node, 3);
    ASSERT_NODE(var4.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var4.node.color);
    ASSERT_NODE(var5.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, NULL, &var5.node, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var5.node);
    ASSERT_RBTREE(rbtree, &var6.node, 2);
    ASSERT_NODE(var5.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var5.node.color);
    ASSERT_NODE(var6.node, NULL, NULL, &var7.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var6.node);
    ASSERT_RBTREE(rbtree, &var7.node, 1);
    ASSERT_NODE(var6.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var6.node.color);
    ASSERT_NODE(var7.node, NULL, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var7.node);
    ASSERT_RBTREE(rbtree, NULL, 0);
    ASSERT_NODE(var7.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var7.node.color);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    rbtree_remove(&rbtree, &var7.node);
    ASSERT_RBTREE(rbtree, &var2.node, 6);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, &var6.node, RBTREE_NODE_RED);
    ASSERT_NODE(var5.node, &var6.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, &var4.node, &var5.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var7.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var7.node.color);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var6.node);
    ASSERT_RBTREE(rbtree, &var2.node, 5);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, &var5.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var6.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var6.node.color);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var5.node);
    ASSERT_RBTREE(rbtree, &var2.node, 4);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var4.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var4.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, &var2.node, &var3.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var5.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var5.node.color);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var4.node);
    ASSERT_RBTREE(rbtree, &var2.node, 3);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var2.node, NULL, &var1.node, &var3.node, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var4.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var4.node.color);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var3.node);
    ASSERT_RBTREE(rbtree, &var2.node, 2);
    ASSERT_NODE(var1.node, &var2.node, NULL, NULL, RBTREE_NODE_RED);
    ASSERT_NODE(var2.node, NULL, &var1.node, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var3.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var3.node.color);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var2.node);
    ASSERT_RBTREE(rbtree, &var1.node, 1);
    ASSERT_NODE(var1.node, NULL, NULL, NULL, RBTREE_NODE_BLACK);
    ASSERT_NODE(var2.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var2.node.color);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove(&rbtree, &var1.node);
    ASSERT_RBTREE(rbtree, NULL, 0);
    ASSERT_NODE(var1.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var1.node.color);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    DRAIN_SEQUENTIALLY(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    DRAIN_SEQUENTIALLY_REVERSE(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    DRAIN_SEQUENTIALLY(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    DRAIN_SEQUENTIALLY_REVERSE(rbtree);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        DRAIN_RANDOMLY(rbtree);
        reset_globals();
    }
}

void test_rbtree_remove_key(void) {
    rbtree_remove_key(&rbtree, &var1.node);
    ASSERT_RBTREE(rbtree, NULL, 0);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    rbtree_remove_key(&rbtree, &var3.key);
    assert(rbtree_contains_key(&rbtree, &var3.key) == 0);
    ASSERT_RBTREE(rbtree, &var2.node, 1);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    rbtree_remove_key(&rbtree, &var1.key);
    assert(rbtree_contains_key(&rbtree, &var1.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var2.key);
    assert(rbtree_contains_key(&rbtree, &var2.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var3.key);
    assert(rbtree_contains_key(&rbtree, &var3.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var4.key);
    assert(rbtree_contains_key(&rbtree, &var4.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var5.key);
    assert(rbtree_contains_key(&rbtree, &var5.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var6.key);
    assert(rbtree_contains_key(&rbtree, &var6.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var7.key);
    assert(rbtree_contains_key(&rbtree, &var7.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    rbtree_remove_key(&rbtree, &var1.key);
    assert(rbtree_contains_key(&rbtree, &var1.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var2.key);
    assert(rbtree_contains_key(&rbtree, &var2.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var3.key);
    assert(rbtree_contains_key(&rbtree, &var3.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var4.key);
    assert(rbtree_contains_key(&rbtree, &var4.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var5.key);
    assert(rbtree_contains_key(&rbtree, &var5.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var6.key);
    assert(rbtree_contains_key(&rbtree, &var6.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_key(&rbtree, &var7.key);
    assert(rbtree_contains_key(&rbtree, &var7.key) == 0);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        rbtree_remove_key(&rbtree, &var1.key);
        assert(rbtree_contains_key(&rbtree, &var1.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_key(&rbtree, &var2.key);
        assert(rbtree_contains_key(&rbtree, &var2.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_key(&rbtree, &var3.key);
        assert(rbtree_contains_key(&rbtree, &var3.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_key(&rbtree, &var4.key);
        assert(rbtree_contains_key(&rbtree, &var4.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_key(&rbtree, &var5.key);
        assert(rbtree_contains_key(&rbtree, &var5.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_key(&rbtree, &var6.key);
        assert(rbtree_contains_key(&rbtree, &var6.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_key(&rbtree, &var7.key);
        assert(rbtree_contains_key(&rbtree, &var7.key) == 0);
        ASSERT_PROPERTIES(rbtree);
        reset_globals();
    }
}

void test_rbtree_remove_first(void) {
    rbtree_remove_first(&rbtree);
    ASSERT_RBTREE(rbtree, NULL, 0);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var2.node);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var2.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var3.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var4.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var5.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var6.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var7.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == NULL);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var2.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var3.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var4.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var5.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var6.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == &var7.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_first(&rbtree);
    assert(rbtree_first(&rbtree) == NULL);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == &var2.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == &var3.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == &var4.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == &var5.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == &var6.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == &var7.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_first(&rbtree);
        assert(rbtree_first(&rbtree) == NULL);
        ASSERT_PROPERTIES(rbtree);
        reset_globals();
    }
}

void test_rbtree_remove_last(void) {
    rbtree_remove_last(&rbtree);
    ASSERT_RBTREE(rbtree, NULL, 0);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var3.node);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var6.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var5.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var4.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var3.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var2.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var1.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == NULL);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var6.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var5.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var4.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var3.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var2.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == &var1.node);
    ASSERT_PROPERTIES(rbtree);
    rbtree_remove_last(&rbtree);
    assert(rbtree_last(&rbtree) == NULL);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    loop {
        FILL_RANDOMLY(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == &var6.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == &var5.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == &var4.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == &var3.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == &var2.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == &var1.node);
        ASSERT_PROPERTIES(rbtree);
        rbtree_remove_last(&rbtree);
        assert(rbtree_last(&rbtree) == NULL);
        ASSERT_PROPERTIES(rbtree);
        reset_globals();
    }
}

void test_rbtree_remove_all(void) {
    rbtree_remove_all(&rbtree);
    ASSERT_RBTREE(rbtree, NULL, 0);

    rbtree_insert(&rbtree, &var1.key, &var1.node);
    rbtree_insert(&rbtree, &var2.key, &var2.node);
    var3.key = var1.key;
    rbtree_insert(&rbtree, &var3.key, &var3.node);
    rbtree_remove_all(&rbtree);
    ASSERT_RBTREE(rbtree, NULL, 0);
    ASSERT_NODE(var3.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var3.node.color);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY(rbtree);
    rbtree_remove_all(&rbtree);
    ASSERT_RBTREE(rbtree, NULL, 0);
    ASSERT_NODE(var2.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var2.node.color);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    FILL_SEQUENTIALLY_REVERSE(rbtree);
    rbtree_remove_all(&rbtree);
    ASSERT_RBTREE(rbtree, NULL, 0);
    ASSERT_NODE(var6.node, RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, var6.node.color);
    ASSERT_PROPERTIES(rbtree);
    reset_globals();

    loop {
        RBTreeNode *root;
        FILL_RANDOMLY(rbtree);
        root = rbtree.root;
        rbtree_remove_all(&rbtree);
        ASSERT_RBTREE(rbtree, NULL, 0);
        ASSERT_NODE((*root), RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, root->color);
        ASSERT_PROPERTIES(rbtree);
        reset_globals();
    }
}

void test_rbtree_entry(void) {
    assert(rbtree_entry(&var1.node, TestStruct, node)->key == 1);
    assert(rbtree_entry(&var1.node, TestStruct, node)->node.parent == RBTREE_POISON_PARENT);
    assert(rbtree_entry(&var1.node, TestStruct, node)->node.left_child == RBTREE_POISON_LEFT_CHILD);
    assert(rbtree_entry(&var1.node, TestStruct, node)->node.right_child == RBTREE_POISON_RIGHT_CHILD);
}

void test_rbtree_for_each(void) {
    RBTreeNode *n;

    rbtree_for_each(n, &rbtree) {
        assert(0);
    }

    loop {
        size_t i = 0;

        FILL_RANDOMLY(rbtree);

        rbtree_for_each(n, &rbtree) {
            ASSERT_FOR_EACH(n, i);
            ++i;
        }
        assert(i == 7);

        reset_globals();
    }
}

void test_rbtree_for_each_reverse(void) {
    RBTreeNode *n;

    rbtree_for_each_reverse(n, &rbtree) {
        assert(0);
    }

    loop {
        size_t i = 7;

        FILL_RANDOMLY(rbtree);

        rbtree_for_each_reverse(n, &rbtree) {
            --i;
            ASSERT_FOR_EACH(n, i);
        }
        assert(i == 0);

        reset_globals();
    }
}

void test_rbtree_for_each_safe(void) {
    RBTreeNode *n, *backup;

    rbtree_for_each_safe(n, backup, &rbtree) {
        assert(0);
    }

    loop {
        size_t i = 0;

        FILL_RANDOMLY(rbtree);

        rbtree_for_each_safe(n, backup, &rbtree) {
            ASSERT_FOR_EACH(n, i);
            rbtree_remove(&rbtree, n);
            n = NULL;
            ++i;
        }
        assert(i == 7);

        reset_globals();
    }
}

void test_rbtree_for_each_safe_reverse(void) {
    RBTreeNode *n, *backup;

    rbtree_for_each_safe_reverse(n, backup, &rbtree) {
        assert(0);
    }

    loop {
        size_t i = 7;

        FILL_RANDOMLY(rbtree);

        rbtree_for_each_safe_reverse(n, backup, &rbtree) {
            --i;
            ASSERT_FOR_EACH(n, i);
            rbtree_remove(&rbtree, n);
            n = NULL;
        }
        assert(i == 0);

        reset_globals();
    }
}

void test_rbtree_for_each_after(void) {
    RBTreeNode *n = NULL;

    rbtree_for_each_after(n) {
        assert(0);
    }

    loop {
        size_t i = 1;

        FILL_RANDOMLY(rbtree);

        n = rbtree_first(&rbtree);
        rbtree_for_each_after(n) {
            ASSERT_FOR_EACH(n, i);
            ++i;
        }
        assert(i == 7);

        reset_globals();
    }
}

void test_rbtree_for_each_after_reverse(void) {
    RBTreeNode *n = NULL;

    rbtree_for_each_after_reverse(n) {
        assert(0);
    }

    loop {
        size_t i = 6;

        FILL_RANDOMLY(rbtree);

        n = rbtree_last(&rbtree);
        rbtree_for_each_after_reverse(n) {
            --i;
            ASSERT_FOR_EACH(n, i);
        }
        assert(i == 0);

        reset_globals();
    }
}

void test_rbtree_for_each_safe_after(void) {
    RBTreeNode *n = NULL, *backup;

    rbtree_for_each_safe_after(n, backup) {
        assert(0);
    }

    loop {
        size_t i = 1;

        FILL_RANDOMLY(rbtree);

        n = rbtree_first(&rbtree);
        rbtree_for_each_safe_after(n, backup) {
            ASSERT_FOR_EACH(n, i);
            rbtree_remove(&rbtree, n);
            n = NULL;
            ++i;
        }
        assert(i == 7);

        reset_globals();
    }
}

void test_rbtree_for_each_safe_after_reverse(void) {
    RBTreeNode *n = NULL, *backup;

    rbtree_for_each_safe_after_reverse(n, backup) {
        assert(0);
    }

    loop {
        size_t i = 6;

        FILL_RANDOMLY(rbtree);

        n = rbtree_last(&rbtree);
        rbtree_for_each_safe_after_reverse(n, backup) {
            --i;
            ASSERT_FOR_EACH(n, i);
            rbtree_remove(&rbtree, n);
            n = NULL;
        }
        assert(i == 0);

        reset_globals();
    }
}

void test_rbtree_for_each_from(void) {
    RBTreeNode *n = NULL;

    rbtree_for_each_from(n) {
        assert(0);
    }

    loop {
        size_t i = 0;

        FILL_RANDOMLY(rbtree);

        n = rbtree_first(&rbtree);
        rbtree_for_each_from(n) {
            ASSERT_FOR_EACH(n, i);
            ++i;
        }
        assert(i == 7);

        reset_globals();
    }
}

void test_rbtree_for_each_from_reverse(void) {
    RBTreeNode *n = NULL;

    rbtree_for_each_from_reverse(n) {
        assert(0);
    }

    loop {
        size_t i = 7;

        FILL_RANDOMLY(rbtree);

        n = rbtree_last(&rbtree);
        rbtree_for_each_from_reverse(n) {
            --i;
            ASSERT_FOR_EACH(n, i);
        }
        assert(i == 0);

        reset_globals();
    }
}

void test_rbtree_for_each_safe_from(void) {
    RBTreeNode *n = NULL, *backup;

    rbtree_for_each_safe_from(n, backup) {
        assert(0);
    }

    loop {
        size_t i = 0;

        FILL_RANDOMLY(rbtree);

        n = rbtree_first(&rbtree);
        rbtree_for_each_safe_from(n, backup) {
            ASSERT_FOR_EACH(n, i);
            rbtree_remove(&rbtree, n);
            n = NULL;
            ++i;
        }
        assert(i == 7);

        reset_globals();
    }
}

void test_rbtree_for_each_safe_from_reverse(void) {
    RBTreeNode *n = NULL, *backup;

    rbtree_for_each_safe_from_reverse(n, backup) {
        assert(0);
    }

    loop {
        size_t i = 7;

        FILL_RANDOMLY(rbtree);

        n = rbtree_last(&rbtree);
        rbtree_for_each_safe_from_reverse(n, backup) {
            --i;
            ASSERT_FOR_EACH(n, i);
            rbtree_remove(&rbtree, n);
            n = NULL;
        }
        assert(i == 0);

        reset_globals();
    }
}

TestFunc test_funcs[] = {
    test_rbtree_init,
    test_rbtree_first,
    test_rbtree_last,
    test_rbtree_prev,
    test_rbtree_next,
    test_rbtree_size,
    test_rbtree_empty,
    test_rbtree_contains_key,
    test_rbtree_index_of,
    test_rbtree_at,
    test_rbtree_insert,
    test_rbtree_lookup_key,
    test_rbtree_remove,
    test_rbtree_remove_key,
    test_rbtree_remove_first,
    test_rbtree_remove_last,
    test_rbtree_remove_all,
    test_rbtree_entry,
    test_rbtree_for_each,
    test_rbtree_for_each_reverse,
    test_rbtree_for_each_safe,
    test_rbtree_for_each_safe_reverse,
    test_rbtree_for_each_after,
    test_rbtree_for_each_after_reverse,
    test_rbtree_for_each_safe_after,
    test_rbtree_for_each_safe_after_reverse,
    test_rbtree_for_each_from,
    test_rbtree_for_each_from_reverse,
    test_rbtree_for_each_safe_from,
    test_rbtree_for_each_safe_from_reverse
};

int main(int argc, char *argv[]) {
    char msg[100] = "RBTree ";
    assert(argc == 2);
    strcat(msg, argv[1]);

    assert(sizeof(test_funcs) / sizeof(TestFunc) == 30);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc), msg, reset_globals);

    return 0;
}
