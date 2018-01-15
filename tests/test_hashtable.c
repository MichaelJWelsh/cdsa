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
#include "../src/hashtable.h"
#include "../src/hashtable.h"

/* ========================================================================================================
 *
 *                                             TESTING UTILITIES
 *
 * ======================================================================================================== */

typedef struct TestStruct {
    int key;
    int num_similar_keys;
    HashTableNode node;
} TestStruct;

TestStruct var1, var2, var3, var4, var5, var6;
HashTable hashtable;
HashTableNode *bkt_arr[3];
size_t counter;
void *aux_ptr;

#define ASSERT_HASHTABLE(hashtable, size_of_hashtable) \
    do { \
        size_t i; \
        \
        assert(hashtable.size == size_of_hashtable); \
        \
        for (i = 0; i < 3; ++i) { \
            HashTableNode *n; \
            for (n = hashtable.bucket_array[i]; n; n = n->next) { \
                assert(n->next || 1); \
            } \
        } \
    } while (0)

#define ASSERT_NODE(node, next_ptr) \
    do { \
        assert((node).next == (HashTableNode*) (next_ptr)); \
    } while (0)

#define ASSERT_BUCKET_ARRAY_NULLIFIED() \
    do { \
        size_t i; \
        for (i = 0; i < 3; ++i) { \
            assert(bkt_arr[i] == NULL); \
        } \
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
            default: \
                assert(0); \
        } \
    } while (0)

#define HASHTABLE_REMOVE_KEY_BY_NODE(hashtable_ptr, node_ptr) \
    hashtable_remove_key(hashtable_ptr, &hashtable_entry(node_ptr, TestStruct, node)->key)

#define POISON_BUCKET_ARRAY() \
    do { \
        size_t i; \
        for (i = 0; i < 3; ++i) { \
            bkt_arr[i] = (HashTableNode*) &aux_ptr; \
        } \
    } while (0)

#define NULLIFY_BUCKET_ARRAY() \
    do { \
        size_t i; \
        for (i = 0; i < 3; ++i) { \
            bkt_arr[i] = NULL; \
        } \
    } while (0)

#define FILL_FOR_TESTING_FOR_EACH(hashtable) \
    do { \
        hashtable_insert(&hashtable, &var2.key, &var2.node); \
        hashtable_insert(&hashtable, &var1.key, &var1.node); \
        hashtable_insert(&hashtable, &var4.key, &var4.node); \
        hashtable_insert(&hashtable, &var3.key, &var3.node); \
        hashtable_insert(&hashtable, &var6.key, &var6.node); \
        hashtable_insert(&hashtable, &var5.key, &var5.node); \
    } while (0)

#define FILL_RANDOMLY(hashtable) \
    { \
        int vars_used[6] = { 0, 0, 0, 0, 0, 0 }; \
        \
        while ( \
            !vars_used[0] || \
            !vars_used[1] || \
            !vars_used[2] || \
            !vars_used[3] || \
            !vars_used[4] || \
            !vars_used[5] \
        ) { \
            int x = rand() % 6; \
            \
            while (vars_used[x]) { \
                x = rand() % 6; \
            } \
            \
            vars_used[x] = 1; \
            \
            switch (x + 1) { \
                case 1: \
                    hashtable_insert(&hashtable, &var1.key, &var1.node); \
                    break; \
                case 2: \
                    hashtable_insert(&hashtable, &var2.key, &var2.node); \
                    break; \
                case 3: \
                    hashtable_insert(&hashtable, &var3.key, &var3.node); \
                    break; \
                case 4: \
                    hashtable_insert(&hashtable, &var4.key, &var4.node); \
                    break; \
                case 5: \
                    hashtable_insert(&hashtable, &var5.key, &var5.node); \
                    break; \
                case 6: \
                    hashtable_insert(&hashtable, &var6.key, &var6.node); \
                    break; \
                default: \
                    assert(0); \
            } \
        } \
    }

#define DRAIN_RANDOMLY(hashtable) \
    { \
        int vars_used[6] = { 0, 0, 0, 0, 0, 0 }; \
        \
        while ( \
            !vars_used[0] || \
            !vars_used[1] || \
            !vars_used[2] || \
            !vars_used[3] || \
            !vars_used[4] || \
            !vars_used[5] \
        ) { \
            int x = rand() % 6; \
            \
            while (vars_used[x]) { \
                x = rand() % 6; \
            } \
            \
            vars_used[x] = 1; \
            \
            switch (x + 1) { \
                case 1: \
                    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var1.node); \
                    break; \
                case 2: \
                    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var2.node); \
                    break; \
                case 3: \
                    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var3.node); \
                    break; \
                case 4: \
                    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var4.node); \
                    break; \
                case 5: \
                    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var5.node); \
                    break; \
                case 6: \
                    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var6.node); \
                    break; \
                default: \
                    assert(0); \
            } \
        } \
    }

#define loop \
    for (counter = 0; counter < 700; ++counter)

static size_t hash_func(const void *key) {
    int k = *(const int*) key;

    if (k == 1 || k == 2) {
        k = 3;
    } else if (k == 3 || k == 4) {
        k = 1;
    } else if (k == 5 || k == 6) {
        k = 2;
    }

    return 81 + k;
}

static int equal_func(const void *key, const HashTableNode *node) {
    return *(const int*)key == hashtable_entry(node, TestStruct, node)->key;
}

static void collide_func(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data) {
    ASSERT_NODE(*old_node, HASHTABLE_POISON_NEXT);
    assert((void**) auxiliary_data == &aux_ptr);

    hashtable_entry(new_node, TestStruct, node)->num_similar_keys += 1 + hashtable_entry(old_node, TestStruct, node)->num_similar_keys;
}

static void reset_globals(void) {
    hashtable_init(&hashtable, bkt_arr, 3, hash_func, equal_func, collide_func, &aux_ptr);

    var1.key = 1;
    var1.num_similar_keys = 0;
    var1.node.next = HASHTABLE_POISON_NEXT;

    var2.key = 2;
    var2.num_similar_keys = 0;
    var2.node.next = HASHTABLE_POISON_NEXT;

    var3.key = 3;
    var3.num_similar_keys = 0;
    var3.node.next = HASHTABLE_POISON_NEXT;

    var4.key = 4;
    var4.num_similar_keys = 0;
    var4.node.next = HASHTABLE_POISON_NEXT;

    var5.key = 5;
    var5.num_similar_keys = 0;
    var5.node.next = HASHTABLE_POISON_NEXT;

    var6.key = 6;
    var6.num_similar_keys = 0;
    var6.node.next = HASHTABLE_POISON_NEXT;
}

/* ========================================================================================================
 *
 *                                             TESTING FUNCTIONS
 *
 * ======================================================================================================== */

void test_hashtable_init(void) {
    HashTableNode node_init_with_macro = HASHTABLE_NODE_INIT;
    ASSERT_NODE(node_init_with_macro, HASHTABLE_POISON_NEXT);

    POISON_BUCKET_ARRAY();
    hashtable_init(&hashtable, bkt_arr, 3, hash_func, equal_func, collide_func, &aux_ptr);
    ASSERT_HASHTABLE(hashtable, 0);
    ASSERT_BUCKET_ARRAY_NULLIFIED();
    assert(hashtable.bucket_array == bkt_arr);
    assert(hashtable.num_buckets == 3);
    assert(hashtable.hash == hash_func);
    assert(hashtable.equal == equal_func);
    assert(hashtable.collide == collide_func);
    assert((void**) hashtable.auxiliary_data == &aux_ptr);
    POISON_BUCKET_ARRAY();
    hashtable_init(&hashtable, bkt_arr, 3, hash_func, equal_func, NULL, NULL);
    ASSERT_HASHTABLE(hashtable, 0);
    ASSERT_BUCKET_ARRAY_NULLIFIED();
    assert(hashtable.bucket_array == bkt_arr);
    assert(hashtable.num_buckets == 3);
    assert(hashtable.hash == hash_func);
    assert(hashtable.equal == equal_func);
    assert(hashtable.collide == NULL);
    assert(hashtable.auxiliary_data == NULL);
}

void test_hashtable_fast_init(void) {
    NULLIFY_BUCKET_ARRAY();
    hashtable_fast_init(&hashtable, bkt_arr, 3, hash_func, equal_func, collide_func, &aux_ptr);
    ASSERT_HASHTABLE(hashtable, 0);
    ASSERT_BUCKET_ARRAY_NULLIFIED();
    assert(hashtable.bucket_array == bkt_arr);
    assert(hashtable.num_buckets == 3);
    assert(hashtable.hash == hash_func);
    assert(hashtable.equal == equal_func);
    assert(hashtable.collide == collide_func);
    assert((void**) hashtable.auxiliary_data == &aux_ptr);
    NULLIFY_BUCKET_ARRAY();
    hashtable_fast_init(&hashtable, bkt_arr, 3, hash_func, equal_func, NULL, NULL);
    ASSERT_HASHTABLE(hashtable, 0);
    ASSERT_BUCKET_ARRAY_NULLIFIED();
    assert(hashtable.bucket_array == bkt_arr);
    assert(hashtable.num_buckets == 3);
    assert(hashtable.hash == hash_func);
    assert(hashtable.equal == equal_func);
    assert(hashtable.collide == NULL);
    assert(hashtable.auxiliary_data == NULL);
}

void test_hashtable_bucket_array(void) {
    assert(hashtable_bucket_array(&hashtable) == bkt_arr);
    hashtable.bucket_array = NULL;
    assert(hashtable_bucket_array(&hashtable) == NULL);
}

void test_hashtable_num_buckets(void) {
    assert(hashtable_num_buckets(&hashtable) == 3);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    assert(hashtable_num_buckets(&hashtable) == 3);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var1.node);
    assert(hashtable_num_buckets(&hashtable) == 3);
    hashtable.num_buckets = 0;
    assert(hashtable_num_buckets(&hashtable) == 0);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        DRAIN_RANDOMLY(hashtable);
        assert(hashtable_num_buckets(&hashtable) == 3);
        reset_globals();
    }
}

void test_hashtable_size(void) {
    assert(hashtable_size(&hashtable) == 0);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    assert(hashtable_size(&hashtable) == 1);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    assert(hashtable_size(&hashtable) == 2);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    assert(hashtable_size(&hashtable) == 3);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    assert(hashtable_size(&hashtable) == 3);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var3.node);
    assert(hashtable_size(&hashtable) == 2);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var2.node);
    assert(hashtable_size(&hashtable) == 1);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var4.node);
    assert(hashtable_size(&hashtable) == 0);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        DRAIN_RANDOMLY(hashtable);
        assert(hashtable_size(&hashtable) == 0);
        reset_globals();
    }
}

void test_hashtable_empty(void) {
    assert(hashtable_empty(&hashtable) == 1);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    assert(hashtable_empty(&hashtable) == 0);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    assert(hashtable_empty(&hashtable) == 0);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    assert(hashtable_empty(&hashtable) == 0);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    assert(hashtable_empty(&hashtable) == 0);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var3.node);
    assert(hashtable_empty(&hashtable) == 0);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var2.node);
    assert(hashtable_empty(&hashtable) == 0);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var4.node);
    assert(hashtable_empty(&hashtable) == 1);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        DRAIN_RANDOMLY(hashtable);
        assert(hashtable_empty(&hashtable) == 1);
        reset_globals();
    }
}

void test_hashtable_contains_key(void) {
    assert(hashtable_contains_key(&hashtable, &var1.key) == 0);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    assert(hashtable_contains_key(&hashtable, &var1.key) == 1);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    assert(hashtable_contains_key(&hashtable, &var2.key) == 1);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    assert(hashtable_contains_key(&hashtable, &var3.key) == 1);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    assert(hashtable_contains_key(&hashtable, &var4.key) == 1);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var3.node);
    assert(hashtable_contains_key(&hashtable, &var3.key) == 0);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var2.node);
    assert(hashtable_contains_key(&hashtable, &var2.key) == 0);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var4.node);
    assert(hashtable_contains_key(&hashtable, &var4.key) == 0);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        assert(hashtable_contains_key(&hashtable, &var1.key) == 1);
        assert(hashtable_contains_key(&hashtable, &var2.key) == 1);
        assert(hashtable_contains_key(&hashtable, &var3.key) == 1);
        assert(hashtable_contains_key(&hashtable, &var4.key) == 1);
        assert(hashtable_contains_key(&hashtable, &var5.key) == 1);
        assert(hashtable_contains_key(&hashtable, &var6.key) == 1);
        DRAIN_RANDOMLY(hashtable);
        assert(hashtable_contains_key(&hashtable, &var1.key) == 0);
        assert(hashtable_contains_key(&hashtable, &var2.key) == 0);
        assert(hashtable_contains_key(&hashtable, &var3.key) == 0);
        assert(hashtable_contains_key(&hashtable, &var4.key) == 0);
        assert(hashtable_contains_key(&hashtable, &var5.key) == 0);
        assert(hashtable_contains_key(&hashtable, &var6.key) == 0);
        reset_globals();
    }
}

void test_hashtable_insert(void) {
    hashtable.collide = NULL;
    hashtable_insert(&hashtable, &var1.key, &var1.node);
    ASSERT_HASHTABLE(hashtable, 1);
    ASSERT_NODE(var1.node, NULL);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    ASSERT_HASHTABLE(hashtable, 2);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, NULL);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var1.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var2.node, &var4.node);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, NULL);
    hashtable_insert(&hashtable, &var1.key, &var1.node);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, HASHTABLE_POISON_NEXT);
    var4.key = 4;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    ASSERT_HASHTABLE(hashtable, 4);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, &var3.node);
    hashtable_insert(&hashtable, &var5.key, &var5.node);
    ASSERT_HASHTABLE(hashtable, 5);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, &var3.node);
    ASSERT_NODE(var5.node, NULL);
    hashtable_insert(&hashtable, &var6.key, &var6.node);
    ASSERT_HASHTABLE(hashtable, 6);
    ASSERT_NODE(var1.node, NULL);
    ASSERT_NODE(var2.node, &var1.node);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, &var3.node);
    ASSERT_NODE(var5.node, NULL);
    ASSERT_NODE(var6.node, &var5.node);
    reset_globals();

    hashtable_insert(&hashtable, &var6.key, &var6.node);
    ASSERT_HASHTABLE(hashtable, 1);
    ASSERT_NODE(var6.node, NULL);
    hashtable_insert(&hashtable, &var5.key, &var5.node);
    ASSERT_HASHTABLE(hashtable, 2);
    ASSERT_NODE(var6.node, NULL);
    ASSERT_NODE(var5.node, &var6.node);
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var6.node, NULL);
    ASSERT_NODE(var5.node, &var6.node);
    ASSERT_NODE(var4.node, NULL);
    var3.key = var6.key;
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    assert(var3.num_similar_keys == 1);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var6.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var5.node, &var3.node);
    ASSERT_NODE(var4.node, NULL);
    ASSERT_NODE(var3.node, NULL);
    hashtable_insert(&hashtable, &var6.key, &var6.node);
    assert(var6.num_similar_keys == 2);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var6.node, NULL);
    ASSERT_NODE(var5.node, &var6.node);
    ASSERT_NODE(var4.node, NULL);
    ASSERT_NODE(var3.node, HASHTABLE_POISON_NEXT);
    var3.key = 3;
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    ASSERT_HASHTABLE(hashtable, 4);
    ASSERT_NODE(var6.node, NULL);
    ASSERT_NODE(var5.node, &var6.node);
    ASSERT_NODE(var4.node, NULL);
    ASSERT_NODE(var3.node, &var4.node);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    ASSERT_HASHTABLE(hashtable, 5);
    ASSERT_NODE(var6.node, NULL);
    ASSERT_NODE(var5.node, &var6.node);
    ASSERT_NODE(var4.node, NULL);
    ASSERT_NODE(var3.node, &var4.node);
    ASSERT_NODE(var2.node, NULL);
    hashtable_insert(&hashtable, &var1.key, &var1.node);
    ASSERT_HASHTABLE(hashtable, 6);
    ASSERT_NODE(var6.node, NULL);
    ASSERT_NODE(var5.node, &var6.node);
    ASSERT_NODE(var4.node, NULL);
    ASSERT_NODE(var3.node, &var4.node);
    ASSERT_NODE(var2.node, NULL);
    ASSERT_NODE(var1.node, &var2.node);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        ASSERT_HASHTABLE(hashtable, 6);
        reset_globals();
    }
}

void test_hashtable_lookup_key(void) {
    assert(hashtable_lookup_key(&hashtable, &var1.key) == NULL);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    assert(hashtable_lookup_key(&hashtable, &var1.key) == &var1.node);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    assert(hashtable_lookup_key(&hashtable, &var2.key) == &var2.node);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    assert(hashtable_lookup_key(&hashtable, &var3.key) == &var3.node);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    assert(hashtable_lookup_key(&hashtable, &var4.key) == &var4.node);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var3.node);
    assert(hashtable_lookup_key(&hashtable, &var3.key) == NULL);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var2.node);
    assert(hashtable_lookup_key(&hashtable, &var2.key) == NULL);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var4.node);
    assert(hashtable_lookup_key(&hashtable, &var4.key) == NULL);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        assert(hashtable_lookup_key(&hashtable, &var1.key) == &var1.node);
        assert(hashtable_lookup_key(&hashtable, &var2.key) == &var2.node);
        assert(hashtable_lookup_key(&hashtable, &var3.key) == &var3.node);
        assert(hashtable_lookup_key(&hashtable, &var4.key) == &var4.node);
        assert(hashtable_lookup_key(&hashtable, &var5.key) == &var5.node);
        assert(hashtable_lookup_key(&hashtable, &var6.key) == &var6.node);
        DRAIN_RANDOMLY(hashtable);
        assert(hashtable_lookup_key(&hashtable, &var1.key) == NULL);
        assert(hashtable_lookup_key(&hashtable, &var2.key) == NULL);
        assert(hashtable_lookup_key(&hashtable, &var3.key) == NULL);
        assert(hashtable_lookup_key(&hashtable, &var4.key) == NULL);
        assert(hashtable_lookup_key(&hashtable, &var5.key) == NULL);
        assert(hashtable_lookup_key(&hashtable, &var6.key) == NULL);
        reset_globals();
    }
}

void test_hashtable_remove_key(void) {
    hashtable_remove_key(&hashtable, &var1.key);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    hashtable_remove_key(&hashtable, &var4.key);
    assert(hashtable_contains_key(&hashtable, &var4.key) == 0);
    ASSERT_HASHTABLE(hashtable, 2);
    reset_globals();

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    hashtable_insert(&hashtable, &var5.key, &var5.node);
    hashtable_insert(&hashtable, &var6.key, &var6.node);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var1.node);
    ASSERT_HASHTABLE(hashtable, 5);
    ASSERT_NODE(var1.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var2.node, NULL);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, &var3.node);
    ASSERT_NODE(var5.node, NULL);
    ASSERT_NODE(var6.node, &var5.node);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var2.node);
    ASSERT_HASHTABLE(hashtable, 4);
    ASSERT_NODE(var2.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var3.node, NULL);
    ASSERT_NODE(var4.node, &var3.node);
    ASSERT_NODE(var5.node, NULL);
    ASSERT_NODE(var6.node, &var5.node);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var3.node);
    ASSERT_HASHTABLE(hashtable, 3);
    ASSERT_NODE(var3.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var4.node, NULL);
    ASSERT_NODE(var5.node, NULL);
    ASSERT_NODE(var6.node, &var5.node);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var4.node);
    ASSERT_HASHTABLE(hashtable, 2);
    ASSERT_NODE(var4.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var5.node, NULL);
    ASSERT_NODE(var6.node, &var5.node);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var5.node);
    ASSERT_HASHTABLE(hashtable, 1);
    ASSERT_NODE(var5.node, HASHTABLE_POISON_NEXT);
    ASSERT_NODE(var6.node, NULL);
    HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, &var6.node);
    ASSERT_HASHTABLE(hashtable, 0);
    ASSERT_NODE(var6.node, HASHTABLE_POISON_NEXT);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        DRAIN_RANDOMLY(hashtable);
        ASSERT_HASHTABLE(hashtable, 0);
        reset_globals();
    }
}

void test_hashtable_remove_all(void) {
    hashtable_remove_all(&hashtable);
    ASSERT_HASHTABLE(hashtable, 0);

    hashtable_insert(&hashtable, &var1.key, &var1.node);
    hashtable_insert(&hashtable, &var2.key, &var2.node);
    hashtable_insert(&hashtable, &var3.key, &var3.node);
    var4.key = var1.key;
    hashtable_insert(&hashtable, &var4.key, &var4.node);
    hashtable_remove_all(&hashtable);
    ASSERT_HASHTABLE(hashtable, 0);
    reset_globals();

    POISON_BUCKET_ARRAY();
    hashtable.size = 1;
    hashtable_remove_all(&hashtable);
    ASSERT_BUCKET_ARRAY_NULLIFIED();
    ASSERT_HASHTABLE(hashtable, 0);
    reset_globals();

    loop {
        FILL_RANDOMLY(hashtable);
        hashtable_remove_all(&hashtable);
        ASSERT_HASHTABLE(hashtable, 0);
        reset_globals();
    }
}

void test_hashtable_entry(void) {
    assert(hashtable_entry(&var1.node, TestStruct, node)->key == 1);
    assert(hashtable_entry(&var1.node, TestStruct, node)->node.next == HASHTABLE_POISON_NEXT);
}

void test_hashtable_for_each(void) {
    HashTableNode *n;
    size_t i, bkt;

    hashtable_for_each(n, bkt, &hashtable) {
        assert(0);
    }

    FILL_FOR_TESTING_FOR_EACH(hashtable);

    i = 0;
    hashtable_for_each(n, bkt, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 6);
}

void test_hashtable_for_each_safe(void) {
    HashTableNode *n, *backup;
    size_t i, bkt;

    hashtable_for_each_safe(n, backup, bkt, &hashtable) {
        assert(0);
    }

    FILL_FOR_TESTING_FOR_EACH(hashtable);

    i = 0;
    hashtable_for_each_safe(n, backup, bkt, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, n);
        n = NULL;
        ++i;
    }
    assert(i == 6);
}

void test_hashtable_for_each_possible(void) {
    HashTableNode *n;
    size_t i;

    hashtable_for_each_possible(n, &var1.key, &hashtable) {
        assert(0);
    }

    FILL_FOR_TESTING_FOR_EACH(hashtable);

    i = 0;
    hashtable_for_each_possible(n, &var1.key, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    hashtable_for_each_possible(n, &var4.key, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    hashtable_for_each_possible(n, &var6.key, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        ++i;
    }
    assert(i == 6);
}

void test_hashtable_for_each_possible_safe(void) {
    HashTableNode *n, *backup;
    size_t i;

    hashtable_for_each_possible_safe(n, backup, &var1.key, &hashtable) {
        assert(0);
    }

    FILL_FOR_TESTING_FOR_EACH(hashtable);

    i = 0;
    hashtable_for_each_possible_safe(n, backup, &var1.key, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, n);
        n = NULL;
        ++i;
    }
    hashtable_for_each_possible_safe(n, backup, &var4.key, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, n);
        n = NULL;
        ++i;
    }
    hashtable_for_each_possible_safe(n, backup, &var6.key, &hashtable) {
        ASSERT_FOR_EACH(n, i);
        HASHTABLE_REMOVE_KEY_BY_NODE(&hashtable, n);
        n = NULL;
        ++i;
    }
    assert(i == 6);
}

TestFunc test_funcs[] = {
    test_hashtable_init,
    test_hashtable_fast_init,
    test_hashtable_bucket_array,
    test_hashtable_num_buckets,
    test_hashtable_size,
    test_hashtable_empty,
    test_hashtable_contains_key,
    test_hashtable_insert,
    test_hashtable_lookup_key,
    test_hashtable_remove_key,
    test_hashtable_remove_all,
    test_hashtable_entry,
    test_hashtable_for_each,
    test_hashtable_for_each_safe,
    test_hashtable_for_each_possible,
    test_hashtable_for_each_possible_safe
};

int main(int argc, char *argv[]) {
    char msg[100] = "HashTable ";
    assert(argc == 2);
    strcat(msg, argv[1]);

    assert(sizeof(test_funcs) / sizeof(TestFunc) == 16);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc), msg, reset_globals);

    return 0;
}
