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

/**
 * @file    hashtable.h
 * @brief   HASH TABLE
 *
 * Embed one or more @ref HashTableNode's into your struct to make it a potential node in one or more
 * hashtables. The @ref HashTable structure keeps track of a table of @ref HashTableNode's. A @ref HashTable
 * MUST be initialized before it is used. A @ref HashTableNode does NOT need to be initialized before it is
 * used. A @ref HashTableNode should belong to at most ONE @ref HashTable.
 *
 * The user is required to define a bucket array (which is an array of pointers to @ref HashTableNode's) that
 * the @ref HashTable will use over the course of its lifetime. The user is encouraged to create this bucket
 * array filled with NULL values in an optimized manner (such as using an initializer list, using calloc,
 * etc), so that they can call @ref hashtable_fast_init which skips the process of filling the bucket array
 * with NULL values manually.
 *
 * The user is required to define a hash function which returns the hashcode of a key. This function does NOT
 * need to convert the hashcode into a usuable bucket index. The returned hashcode can be ANY number.
 *
 * The user is required to define an equal function which determines if a key is equal to the key of a
 * @ref HashTableNode. When a @ref HashTableNode is inserted with a non-unique (an already existing) key, the
 * old @ref HashTableNode will be discarded and the new @ref HashTableNode will take its place.
 *
 * The user can OPTIONALLY define a collide function which is called after the old @ref HashTableNode is
 * replaced. The collide function is called with three arguments, the old @ref HashTableNode, the new
 * @ref HashTableNode, and the auxiliary data that was stored in the @ref HashTable during initialization. The
 * collide function can be used for conveniently freeing up resources held by the old @ref HashTableNode,
 * giving the @ref HashTable multi-key functionality, etc. Note that the auxiliary data is NEVER manipulated
 * by the @ref HashTable. This data is user-defined. This data, for example, could be a memory pool object
 * that is used for freeing up resources held by the old @ref HashTableNode in the collide function.
 *
 * Note the difference between a key collision and a bucket collision. The @ref HashTable handles bucket
 * collisions internally by using a singly linked list at each bucket. This allows the @ref HashTable to grow
 * in size indefinitely without having to resize (at the cost of poor insert/lookup/removal time complexities
 * when the size of the @ref HashTable is severely greater than the number buckets). If you wish to resize a
 * @ref HashTable, you are best off creating an entirely new @ref HashTable and iteratively removing each
 * @ref HashTableNode from the old @ref HashTable while immediately inserting it into the new @ref HashTable
 * afterwards.
 *
 * Example:
 *          struct Object {
 *              int key;
 *              int val;
 *              HashTableNode n;
 *          };
 *
 *          size_t hash(const void *key) {
 *              return *(const int*)key;
 *          }
 *
 *          int equal(const void *key, const HashTableNode *node) {
 *              return *(const int*)key == hashtable_entry(node, struct Object, n)->key;
 *          }
 *
 *          int main(void) {
 *              struct Object obj;
 *              HashTable hashtable;
 *              HashTableNode *bucket_array[50] = { NULL };
 *              int copy_val;
 *
 *              obj.key = 1;
 *
 *              hashtable_fast_init(&hashtable, bucket_array, 50, hash, equal, NULL, NULL);
 *              hashtable_insert(&hashtable, &obj.key, &obj.n);
 *
 *              obj.val = 1000;
 *              copy_val = hashtable_entry(hashtable_lookup_key(&hashtable, &obj.key), struct Object, n)->val;
 *              assert(obj.val == copy_val);
 *              return 0;
 *          }
 *
 * Dependencies:
 *      -   C89 assert.h
 *      -   C89 stddef.h
 *
 * API:
 *      ====  TYPES  ====
 *      -   typedef struct HashTable HashTable;
 *      -   typedef struct HashTableNode HashTableNode;
 *
 *      ====  FUNCTIONS  ====
 *      Initializers:
 *          -   hashtable_init
 *          -   hashtable_fast_init
 *      Properties:
 *          -   hashtable_bucket_array
 *          -   hashtable_num_buckets
 *          -   hashtable_size
 *          -   hashtable_empty
 *          -   hashtable_contains_key
 *      Insertion:
 *          -   hashtable_insert
 *      Lookup:
 *          -   hashtable_lookup_key
 *      Removal:
 *          -   hashtable_remove_key
 *          -   hashtable_remove_all
 *
 *      ====  MACROS  ====
 *      Constants:
 *          -   HASHTABLE_POISON_NEXT
 *      Convenient Node Initializer:
 *          -   HASHTABLE_NODE_INIT
 *      Properties:
 *          -   hashtable_entry
 *      Traversal:
 *          -   hashtable_for_each
 *          -   hashtable_for_each_safe
 *          -   hashtable_for_each_possible
 *          -   hashtable_for_each_possible_safe
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

/* ========================================================================================================
 *
 *                                                  TYPES
 *
 * ======================================================================================================== */

/* Struct type declarations. */
struct HashTable;
struct HashTableNode;

/* Struct typedef's. */
typedef struct HashTable HashTable;
typedef struct HashTableNode HashTableNode;

/**
 * Represents a hash table.
 */
struct HashTable {
    HashTableNode **bucket_array;
    size_t (*hash)(const void *key);
    int (*equal)(const void *key, const HashTableNode *node);
    void (*collide)(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data);
    void *auxiliary_data;
    size_t num_buckets;
    size_t size;
};

/**
 * Represents a node in a @ref HashTable. Embed this into your structure to make it a node.
 */
struct HashTableNode {
    HashTableNode *next;
};

/* ========================================================================================================
 *
 *                                               PROTOTYPES
 *
 * ======================================================================================================== */

/**
 * Initializes/resets the @ref hashtable. Unlike @ref hashtable_fast_init, this function fills the
 * @ref bucket_array with NULL values manually.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *      -   @ref bucket_array != NULL
 *      -   @ref num_buckets > 0
 *      -   @ref hash != NULL
 *      -   @ref equal != NULL
 *
 * Time complexity:
 *      -   O(m), where m == number of buckets in bucket array
 *
 * @param hashtable             The @ref HashTable to be initialized/reset.
 * @param bucket_array          The bucket array created by the user. It does NOT need to already be filled
 *                              with NULL values.
 * @param num_buckets           The number of buckets in the @ref bucket_array.
 * @param hash                  The callback function used to hash a key.
 * @param equal                 The callback function used to to determine if a key is equal to the key of a
 *                              @ref HashTableNode.
 * @param collide               The OPTIONAL (i.e. can be NULL) callback function used to handle key
 *                              collisions (note that key collisions and bucket collisions are two entirely
 *                              different things). If non-NULL, @ref collide will be called after the old
 *                              @ref HashTableNode is replaced by the new @ref HashTableNode.
 * @param auxiliary_data        The auxiliary data passed to the OPTIONAL @ref collide callback function if
 *                              the @ref collide callback function is non-NULL. This data is NEVER manipulated
 *                              by the @ref hashtable. This data is user-defined. For example, this data might
 *                              be a memory pool object that is used for freeing up resources held by the old
 *                              @ref HashTableNode in the @ref collide callback function.
 */
void hashtable_init(
    HashTable *hashtable,
    HashTableNode **bucket_array,
    size_t num_buckets,
    size_t (*hash)(const void *key),
    int (*equal)(const void *key, const HashTableNode *node),
    void (*collide)(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data),
    void *auxiliary_data
);

/**
 * Initializes/resets the @ref hashtable. Note that the @ref bucket_array MUST be filled with NULL values.
 * This function does NOT do this because there are more time efficient ways of doing this during creation of
 * the @ref bucket_array.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *      -   @ref bucket_array != NULL
 *      -   @ref num_buckets > 0
 *      -   @ref hash != NULL
 *      -   @ref equal != NULL
 *      -   @ref bucket_array is filled with NULL values
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param hashtable             The @ref HashTable to be initialized/reset.
 * @param bucket_array          The bucket array created by the user. It MUST be filled with NULL values.
 * @param num_buckets           The number of buckets in the @ref bucket_array.
 * @param hash                  The callback function used to hash a key.
 * @param equal                 The callback function used to to determine if a key is equal to the key of a
 *                              @ref HashTableNode.
 * @param collide               The OPTIONAL (i.e. can be NULL) callback function used to handle key
 *                              collisions (note that key collisions and bucket collisions are two entirely
 *                              different things). If non-NULL, @ref collide will be called after the old
 *                              @ref HashTableNode is replaced by the new @ref HashTableNode.
 * @param auxiliary_data        The auxiliary data passed to the OPTIONAL @ref collide callback function if
 *                              the @ref collide callback function is non-NULL. This data is NEVER manipulated
 *                              by the @ref hashtable. This data is user-defined. For example, this data might
 *                              be a memory pool object that is used for freeing up resources held by the old
 *                              @ref HashTableNode in the @ref collide callback function.
 */
void hashtable_fast_init(
    HashTable *hashtable,
    HashTableNode **bucket_array,
    size_t num_buckets,
    size_t (*hash)(const void *key),
    int (*equal)(const void *key, const HashTableNode *node),
    void (*collide)(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data),
    void *auxiliary_data
);

/**
 * Returns the bucket array used by the @ref hashtable.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param hashtable             The @ref HashTable whose "bucket_array" member will be returned.
 * @return                      @ref hashtable->bucket_array.
 */
HashTableNode** hashtable_bucket_array(const HashTable *hashtable);

/**
 * Returns the number of buckets in the bucket array used by the @ref hashtable.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param hashtable             The @ref HashTable whose "num_buckets" member will be returned.
 * @return                      @ref hashtable->num_buckets.
 */
size_t hashtable_num_buckets(const HashTable *hashtable);

/**
 * Returns the size of the @ref hashtable.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param hashtable             The @ref HashTable whose "size" member will be returned.
 * @return                      @ref hashtable->size.
 */
size_t hashtable_size(const HashTable *hashtable);

/**
 * Returns whether or not the @ref hashtable is empty (i.e. @ref hashtable->size == 0).
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param hashtable             The @ref HashTable whose "size" member will be used to determine if it is
 *                              empty.
 * @return                      Whether or not the @ref hashtable is empty (i.e. @ref hashtable->size == 0).
 */
int hashtable_empty(const HashTable *hashtable);

/**
 * Returns whether or not the @ref hashtable contains the @ref HashTableNode associated with the @ref key.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(n/m), where m == number of buckets in bucket array
 *
 * @param hashtable             The @ref HashTable that may potentially contain the @ref HashTableNode
 *                              associated with the @ref key.
 * @param key                   The key used for lookup.
 * @return                      Whether or not the @ref hashtable contains the @ref HashTableNode associated
 *                              with the @ref key.
 */
int hashtable_contains_key(const HashTable *hashtable, const void *key);

/**
 * Inserts the @ref node with associated @ref key into the @ref hashtable. If a @ref HashTableNode already
 * exists with the same @ref key, the already existing @ref HashTableNode will be replaced by the new
 * @ref HashTableNode, and then the @ref hashtable->collide function will be called (if non-NULL).
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *      -   @ref node != NULL
 *
 * Time complexity:
 *      -   O(n/m), where m == number of buckets in bucket array
 *
 * @param hashtable             The @ref HashTable to be operated on.
 * @param key                   The key associated with the @ref node.
 * @param node                  The @ref HashTableNode to be inserted.
 */
void hashtable_insert(HashTable *hashtable, const void *key, HashTableNode *node);

/**
 * Returns the @ref HashTableNode associated with the @ref key in the @ref hashtable. NULL if a match for the
 * @ref key is not found.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(n/m), where m == number of buckets in bucket array
 *
 * @param hashtable             The @ref HashTable containing nodes.
 * @param key                   The key used for lookup.
 * @return                      NULL if a match for the @ref key is not found; otherwise, the
 *                              @ref HashTableNode associated with the @ref key.
 */
HashTableNode* hashtable_lookup_key(const HashTable *hashtable, const void *key);

/**
 * Removes the @ref HashTableNode associated with the @ref key from the @ref hashtable. If a match for the
 * @ref key is not found, this function simply returns.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(n/m), where m == number of buckets in bucket array
 *
 * @param hashtable             The @ref HashTable to be operated on.
 * @param key                   The key used for lookup.
 */
void hashtable_remove_key(HashTable *hashtable, const void *key);

/**
 * Removes all the @ref HashTableNode's from the @ref hashtable. If the @ref hashtable is empty, this function
 * simply returns.
 *
 * Requirements:
 *      -   @ref hashtable != NULL
 *
 * Time complexity:
 *      -   O(m), where m == number of buckets in bucket array
 *
 * @param hashtable             The @ref HashTable to be operated on.
 */
void hashtable_remove_all(HashTable *hashtable);

/* ========================================================================================================
 *
 *                                                 MACROS
 *
 * ======================================================================================================== */

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "next" member of
 * a single removed @ref HashTableNode. Useful for identifying bugs.
 */
#define HASHTABLE_POISON_NEXT ((HashTableNode*) 0x100)

/**
 * Initializing a @ref HashTableNode before it is used is NOT required. This macro is simply for allowing you
 * to initialize a struct (containing one or more @ref HashTableNode's) with an initializer-list conveniently.
 */
#define HASHTABLE_NODE_INIT { HASHTABLE_POISON_NEXT }

/**
 * Obtains the pointer to the struct for this entry.
 *
 * Requirements:
 *      -   @ref node_ptr != NULL
 *
 * @param node_ptr              The pointer to the @ref HashTableNode in the struct.
 * @param type                  The type of the struct the @ref HashTableNode is embedded in.
 * @param member                The name of the @ref HashTableNode in the struct.
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define hashtable_entry(node_ptr, type, member) \
        ({ \
            const typeof(((type*)0)->member) *__mptr = (node_ptr); \
            (type*) ((char*)__mptr - offsetof(type, member)); \
        })
#else
    #define hashtable_entry(node_ptr, type, member) \
        ( \
            (type*) ((char*)(node_ptr) - offsetof(type, member)) \
        )
#endif

/**
 * Iterates over the @ref HashTable.
 *
 * Requirements:
 *      -   @ref hashtable_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref HashTable in
 *          the loop's body.
 *      -   The @ref bucket_index is not reassigned.
 *
 * @param cursor_node_ptr       The @ref HashTableNode to use as a loop cursor.
 * @param bucket_index          The integer to use to keep track of the current bucket index.
 * @param hashtable_ptr         The pointer to a @ref HashTable that will be iterated over.
 */
#define hashtable_for_each(cursor_node_ptr, bucket_index, hashtable_ptr) \
    for ( \
        bucket_index = 0; \
        bucket_index < (hashtable_ptr)->num_buckets; \
        ++bucket_index \
    ) \
        for ( \
            cursor_node_ptr = (hashtable_ptr)->bucket_array[bucket_index]; \
            cursor_node_ptr; \
            cursor_node_ptr = cursor_node_ptr->next \
        )

/**
 * Iterates over the @ref HashTable, and is safe against reassignment and/or removal of the
 * @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref hashtable_ptr != NULL
 *      -   The @ref backup_node_ptr is neither reassigned nor removed from its associated @ref HashTable in
 *          the loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *      -   The @ref bucket_index is not reassigned.
 *
 * @param cursor_node_ptr       The @ref HashTableNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref HashTableNode to use as a temporary storage.
 * @param bucket_index          The integer to use to keep track of the current bucket index.
 * @param hashtable_ptr         The pointer to a @ref HashTable that will be iterated over.
 */
#define hashtable_for_each_safe(cursor_node_ptr, backup_node_ptr, bucket_index, hashtable_ptr) \
    for ( \
        bucket_index = 0; \
        bucket_index < (hashtable_ptr)->num_buckets; \
        ++bucket_index \
    ) \
        for ( \
            cursor_node_ptr = (hashtable_ptr)->bucket_array[bucket_index], \
            backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
            \
            cursor_node_ptr; \
            \
            cursor_node_ptr = backup_node_ptr, \
            backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL \
        )

/**
 * Iterates over all possible @ref HashTableNode's hashing to the same bucket in the @ref HashTable.
 *
 * Requirements:
 *      -   @ref hashtable_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref HashTable in
 *          the loop's body.
 *
 * @param cursor_node_ptr       The @ref HashTableNode to use as a loop cursor.
 * @param key_ptr               The key used to determine which bucket will be iterated over.
 * @param hashtable_ptr         The pointer to a @ref HashTable that will be iterated over.
 */
#define hashtable_for_each_possible(cursor_node_ptr, key_ptr, hashtable_ptr) \
    for ( \
        cursor_node_ptr = (hashtable_ptr)->bucket_array[(hashtable_ptr)->hash((key_ptr)) % (hashtable_ptr)->num_buckets]; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->next \
    )

/**
 * Iterates over all possible @ref HashTableNode's hashing to the same bucket in the @ref HashTable, and is
 * safe against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref hashtable_ptr != NULL
 *      -   The @ref backup_node_ptr is neither reassigned nor removed from its associated @ref HashTable in
 *          the loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref HashTableNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref HashTableNode to use as a temporary storage.
 * @param key_ptr               The key used to determine which bucket will be iterated over.
 * @param hashtable_ptr         The pointer to a @ref HashTable that will be iterated over.
 */
#define hashtable_for_each_possible_safe(cursor_node_ptr, backup_node_ptr, key_ptr, hashtable_ptr) \
    for ( \
        cursor_node_ptr = (hashtable_ptr)->bucket_array[(hashtable_ptr)->hash((key_ptr)) % (hashtable_ptr)->num_buckets], \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL \
    )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HASHTABLE_H */
