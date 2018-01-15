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

#include <assert.h>
#include <stddef.h>

#include "hashtable.h"

void hashtable_init(
    HashTable *hashtable,
    HashTableNode **bucket_array,
    size_t num_buckets,
    size_t (*hash)(const void *key),
    int (*equal)(const void *key, const HashTableNode *node),
    void (*collide)(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data),
    void *auxiliary_data
) {
    size_t i;

    assert(hashtable && bucket_array && num_buckets > 0 && hash && equal);

    for (i = 0; i < num_buckets; ++i) {
        bucket_array[i] = NULL;
    }

    hashtable->hash = hash;
    hashtable->equal = equal;
    hashtable->collide = collide;
    hashtable->auxiliary_data = auxiliary_data;
    hashtable->bucket_array = bucket_array;
    hashtable->num_buckets = num_buckets;
    hashtable->size = 0;
}

void hashtable_fast_init(
    HashTable *hashtable,
    HashTableNode **bucket_array,
    size_t num_buckets,
    size_t (*hash)(const void *key),
    int (*equal)(const void *key, const HashTableNode *node),
    void (*collide)(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data),
    void *auxiliary_data
) {
    assert(hashtable && bucket_array && num_buckets > 0 && hash && equal);

    #ifndef NDEBUG
    {
        size_t i;
        for (i = 0; i < num_buckets; ++i) {
            assert(!bucket_array[i]);
        }
    }
    #endif /* NDEBUG */

    hashtable->hash = hash;
    hashtable->equal = equal;
    hashtable->collide = collide;
    hashtable->auxiliary_data = auxiliary_data;
    hashtable->bucket_array = bucket_array;
    hashtable->num_buckets = num_buckets;
    hashtable->size = 0;
}

HashTableNode** hashtable_bucket_array(const HashTable *hashtable) {
    assert(hashtable);

    return hashtable->bucket_array;
}

size_t hashtable_num_buckets(const HashTable *hashtable) {
    assert(hashtable);

    return hashtable->num_buckets;
}

size_t hashtable_size(const HashTable *hashtable) {
    assert(hashtable);

    return hashtable->size;
}

int hashtable_empty(const HashTable *hashtable) {
    assert(hashtable);

    return hashtable->size == 0;
}

int hashtable_contains_key(const HashTable *hashtable, const void *key) {
    assert(hashtable);

    return hashtable_lookup_key(hashtable, key) != NULL;
}

void hashtable_insert(HashTable *hashtable, const void *key, HashTableNode *node) {
    HashTableNode **bucket, *n, *prev;

    assert(hashtable && node);

    bucket = hashtable->bucket_array + hashtable->hash(key) % hashtable->num_buckets;

    for (n = *bucket, prev = NULL; n; prev = n, n = n->next) {
        if (hashtable->equal(key, n)) {
            if (prev) {
                prev->next = node;
            } else {
                *bucket = node;
            }

            node->next = n->next;
            n->next = HASHTABLE_POISON_NEXT;

            if (hashtable->collide) {
                hashtable->collide(n, node, hashtable->auxiliary_data);
            }

            return;
        }
    }

    node->next = *bucket;
    *bucket = node;

    ++hashtable->size;
}

HashTableNode* hashtable_lookup_key(const HashTable *hashtable, const void *key) {
    HashTableNode *n;

    assert(hashtable);

    n = hashtable->bucket_array[hashtable->hash(key) % hashtable->num_buckets];

    while (n && !hashtable->equal(key, n)) {
        n = n->next;
    }

    return n;
}

void hashtable_remove_key(HashTable *hashtable, const void *key) {
    HashTableNode **bucket, *n, *prev;

    assert(hashtable);

    bucket = hashtable->bucket_array + hashtable->hash(key) % hashtable->num_buckets;

    for (n = *bucket, prev = NULL; n; prev = n, n = n->next) {
        if (hashtable->equal(key, n)) {
            if (prev) {
                prev->next = n->next;
            } else {
                *bucket = n->next;
            }

            n->next = HASHTABLE_POISON_NEXT;

            --hashtable->size;

            return;
        }
    }
}

void hashtable_remove_all(HashTable *hashtable) {
    size_t i;

    assert(hashtable);

    if (hashtable->size == 0) {
        return;
    }

    for (i = 0; i < hashtable->num_buckets; ++i) {
        hashtable->bucket_array[i] = NULL;
    }

    hashtable->size = 0;
}
