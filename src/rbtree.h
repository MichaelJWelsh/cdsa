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
 * @file    rbtree.h
 * @brief   RED-BLACK TREE
 *
 * Embed one or more @ref RBTreeNode's into your struct to make it a potential node in one or more red-black
 * trees. The @ref RBTree structure keeps track of a tree of @ref RBTreeNode's. A @ref RBTree MUST be
 * initialized before it is used. A @ref RBTreeNode does NOT need to be initialized before it is used. A
 * @ref RBTreeNode should belong to at most ONE @ref RBTree.
 *
 * The user is required to define a compare function which compares a key with the key of a @ref RBTreeNode.
 * When a @ref RBTreeNode is inserted with a non-unique (an already existing) key, the old @ref RBTreeNode
 * will be discarded and the new @ref RBTreeNode will take its place.
 *
 * The user can OPTIONALLY define a collide function which is called after the old @ref RBTreeNode is
 * replaced. The collide function is called with three arguments, the old @ref RBTreeNode, the new
 * @ref RBTreeNode, and the auxiliary data that was stored in the @ref RBTree during initialization. The
 * collide function can be used for conveniently freeing up resources held by the old @ref RBTreeNode, giving
 * the @ref RBTree multi-key functionality, etc. Note that the auxiliary data is NEVER manipulated by the
 * @ref RBTree. This data is user-defined. This data, for example, could be a memory pool object that is used
 * for freeing up resources held by the old @ref RBTreeNode in the collide function.
 *
 * Example:
 *          struct Object {
 *              int key;
 *              int val;
 *              RBTreeNode n;
 *          };
 *
 *          int compare(const void *key, const RBTreeNode *node) {
 *              return *(const int*)key - rbtree_entry(node, struct Object, n)->key;
 *          }
 *
 *          int main(void) {
 *              struct Object obj;
 *              RBTree rbtree;
 *              int copy_val;
 *
 *              obj.key = 1;
 *
 *              rbtree_init(&rbtree, compare, NULL, NULL);
 *              rbtree_insert(&rbtree, &obj.key, &obj.n);
 *
 *              obj.val = 1000;
 *              copy_val = rbtree_entry(rbtree_first(&rbtree), struct Object, n)->val;
 *              assert(obj.val == copy_val);
 *
 *              return 0;
 *          }
 *
 * Dependencies:
 *      -   C89 assert.h
 *      -   C89 stddef.h
 *
 * API:
 *      ====  TYPES  ====
 *      -   typedef struct RBTree RBTree
 *      -   typedef struct RBTreeNode RBTreeNode
 *      -   typedef enum RBTreeNodeColor RBTreeNodeColor
 *          -   RBTREE_NODE_RED = 0
 *          -   RBTREE_NODE_BLACK = 1
 *
 *      ====  FUNCTIONS  ====
 *      Initializers:
 *          -   rbtree_init
 *      Properties:
 *          -   rbtree_first
 *          -   rbtree_last
 *          -   rbtree_prev
 *          -   rbtree_next
 *          -   rbtree_size
 *          -   rbtree_empty
 *          -   rbtree_contains_key
 *      Array Interfacing:
 *          -   rbtree_index_of
 *          -   rbtree_at
 *      Insertion:
 *          -   rbtree_insert
 *      Lookup:
 *          -   rbtree_lookup_key
 *      Removal:
 *          -   rbtree_remove
 *          -   rbtree_remove_key
 *          -   rbtree_remove_first
 *          -   rbtree_remove_last
 *          -   rbtree_remove_all
 *
 *      ====  MACROS  ====
 *      Constants:
 *          -   RBTREE_POISON_PARENT
 *          -   RBTREE_POISON_LEFT_CHILD
 *          -   RBTREE_POISON_RIGHT_CHILD
 *      Convenient Node Initializer:
 *          -   RBTREE_NODE_INIT
 *      Properties:
 *          -   rbtree_entry
 *      Traversal:
 *          -   rbtree_for_each
 *          -   rbtree_for_each_reverse
 *          -   rbtree_for_each_safe
 *          -   rbtree_for_each_safe_reverse
 *          -   rbtree_for_each_after
 *          -   rbtree_for_each_after_reverse
 *          -   rbtree_for_each_safe_after
 *          -   rbtree_for_each_safe_after_reverse
 *          -   rbtree_for_each_from
 *          -   rbtree_for_each_from_reverse
 *          -   rbtree_for_each_safe_from
 *          -   rbtree_for_each_safe_from_reverse
 */

#ifndef RBTREE_H
#define RBTREE_H

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
struct RBTree;
struct RBTreeNode;

/* Struct typedef's. */
typedef struct RBTree RBTree;
typedef struct RBTreeNode RBTreeNode;

/**
 * Represents a red-black tree.
 */
struct RBTree {
    int (*compare)(const void *key, const RBTreeNode *node);
    void (*collide)(const RBTreeNode *old_node, const RBTreeNode *new_node, void *auxiliary_data);
    void *auxiliary_data;
    RBTreeNode *root;
    size_t size;
};

/**
 * Represents the color of a @ref RBTreeNode.
 */
typedef enum RBTreeNodeColor {
    RBTREE_NODE_RED = 0,
    RBTREE_NODE_BLACK = 1
} RBTreeNodeColor;

/**
 * Represents a node in a @ref RBTree. Embed this into your structure to make it a node.
 */
struct RBTreeNode {
    RBTreeNode *parent;
    RBTreeNode *left_child;
    RBTreeNode *right_child;
    RBTreeNodeColor color;
};

/* ========================================================================================================
 *
 *                                               PROTOTYPES
 *
 * ======================================================================================================== */

/**
 * Initializes/resets the @ref rbtree.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *      -   @ref compare != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param rbtree                The @ref RBTree to be initialized/reset.
 * @param compare               The callback function used to compare a key with the key of a @ref RBTreeNode.
 * @param collide               The OPTIONAL (i.e. can be NULL) callback function used to handle key
 *                              collisions. If non-NULL, @ref collide will be called after the old
 *                              @ref RBTreeNode is replaced by the new @ref RBTreeNode.
 * @param auxiliary_data        The auxiliary data passed to the OPTIONAL @ref collide callback function if
 *                              the @ref collide callback function is non-NULL. This data is NEVER manipulated
 *                              by the @ref rbtree. This data is user-defined. For example, this data could be
 *                              a memory pool object that is used for freeing up resources held by the old
 *                              @ref RBTreeNode in the @ref collide callback function.
 */
void rbtree_init(
    RBTree *rbtree,
    int (*compare)(const void *key, const RBTreeNode *node),
    void (*collide)(const RBTreeNode *old_node, const RBTreeNode *new_node, void *auxiliary_data),
    void *auxiliary_data
);

/**
 * Returns the first inorder @ref RBTreeNode of the @ref rbtree.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree whose first inorder @ref RBTreeNode will be returned.
 * @return                      The first inorder @ref RBTreeNode of the @ref rbtree.
 */
RBTreeNode* rbtree_first(const RBTree *rbtree);

/**
 * Returns the last inorder @ref RBTreeNode of the @ref rbtree.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree whose last inorder @ref RBTreeNode will be returned.
 * @return                      The last inorder @ref RBTreeNode of the @ref rbtree.
 */
RBTreeNode* rbtree_last(const RBTree *rbtree);

/**
 * Returns the @ref RBTreeNode before the @ref node (i.e. the inorder predecessor). NULL if @ref node == NULL.
 *
 * Requirements:
 *      -   None
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param node                  The @ref RBTreeNode whose inorder predecessor will be returned.
 * @return                      NULL if @ref node == NULL; otherwise, the inorder predecessor of the
 *                              @ref node.
 */
RBTreeNode* rbtree_prev(const RBTreeNode *node);

/**
 * Returns the @ref RBTreeNode after the @ref node (i.e. the inorder successor). NULL if @ref node == NULL.
 *
 * Requirements:
 *      -   None
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param node                  The @ref RBTreeNode whose inorder successor will be returned.
 * @return                      NULL if @ref node == NULL; otherwise, the inorder successor of the @ref node.
 */
RBTreeNode* rbtree_next(const RBTreeNode *node);

/**
 * Returns the size of the @ref rbtree.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param rbtree                The @ref RBTree whose "size" member will be returned.
 * @return                      @ref rbtree->size.
 */
size_t rbtree_size(const RBTree *rbtree);

/**
 * Returns whether or not the @ref rbtree is empty (i.e. @ref rbtree->size == 0).
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param rbtree                The @ref RBTree whose "size" member will be used to determine if it is empty.
 * @return                      Whether or not the @ref rbtree is empty (i.e. @ref rbtree->size == 0).
 */
int rbtree_empty(const RBTree *rbtree);

/**
 * Returns whether or not the @ref rbtree contains the @ref RBTreeNode associated with the @ref key.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree that may potentially contain the @ref RBTreeNode associated
 *                              with the @ref key.
 * @param key                   The key used for lookup.
 * @return                      Whether or not the @ref rbtree contains the @ref RBTreeNode associated with
 *                              the @ref key.
 */
int rbtree_contains_key(const RBTree *rbtree, const void *key);

/**
 * Retrieves the (inorder) index of the @ref node in the @ref rbtree.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *      -   @ref node != NULL
 *
 * Time complexity:
 *      -   If first/last:
 *          -   O(1)
 *      -   Else:
 *          -   On average:     O(n/2)
 *          -   Worst case:     O(n)
 *
 * @param rbtree                The @ref RBTree that contains the @ref node.
 * @param node                  The @ref RBTreeNode whose index is wanted.
 * @return                      The index of the @ref node in the @ref rbtree.
 */
size_t rbtree_index_of(const RBTree *rbtree, const RBTreeNode *node);

/**
 * Retrieves @ref RBTreeNode at the (inorder) @ref index.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *      -   @ref index < @ref rbtree->size
 *
 * Time complexity:
 *      -   If first/last:
 *          -   O(1)
 *      -   Else:
 *          -   On average:     O(n/4)
 *          -   Worst case:     O(n/2)
 *
 * @param rbtree                The @ref RBTree containing nodes.
 * @param index                 The index of the wanted @ref RBTreeNode.
 * @return                      The @ref RBTreeNode at the @ref index in the @ref rbtree.
 */
RBTreeNode* rbtree_at(const RBTree *rbtree, size_t index);

/**
 * Inserts the @ref node with associated @ref key into the @ref rbtree. If a @ref RBTreeNode already exists
 * with the same @ref key, the already existing @ref RBTreeNode will be replaced by the new @ref RBTreeNode,
 * and then the @ref rbtree->collide function will be called (if non-NULL).
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *      -   @ref node != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree to be operated on.
 * @param key                   The key associated with the @ref node.
 * @param node                  The @ref RBTreeNode to be inserted.
 */
void rbtree_insert(RBTree *rbtree, const void *key, RBTreeNode *node);

/**
 * Returns the @ref RBTreeNode associated with the @ref key in the @ref rbtree. NULL if a match for the @ref
 * key is not found.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree containing nodes.
 * @param key                   The key used for lookup.
 * @return                      NULL if a match for the @ref key is not found; otherwise, the @ref RBTreeNode
 *                              associated with the @ref key.
 */
RBTreeNode* rbtree_lookup_key(const RBTree *rbtree, const void *key);

/**
 * Removes the @ref node from the @ref rbtree. If @ref node == NULL, this function simply returns.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree containing the @ref node to be removed.
 * @param node                  The @ref RBTreeNode in the @ref rbtree to be removed.
 */
void rbtree_remove(RBTree *rbtree, RBTreeNode *node);

/**
 * Removes the @ref RBTreeNode associated with the @ref key from the @ref rbtree. If a match for the @ref key
 * is not found, this function simply returns.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree to be operated on.
 * @param key                   The key used for lookup.
 */
void rbtree_remove_key(RBTree *rbtree, const void *key);

/**
 * Removes the first inorder @ref RBTreeNode from the @ref rbtree. If the @ref rbtree is empty, this function
 * simply returns.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree to be operated on.
 */
void rbtree_remove_first(RBTree *rbtree);

/**
 * Removes the last inorder @ref RBTreeNode from the @ref rbtree. If the @ref rbtree is empty, this function
 * simply returns.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(log(n))
 *
 * @param rbtree                The @ref RBTree to be operated on.
 */
void rbtree_remove_last(RBTree *rbtree);

/**
 * Removes all the @ref RBTreeNode's from the @ref rbtree. If the @ref rbtree is empty, this function simply
 * returns.
 *
 * Requirements:
 *      -   @ref rbtree != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param rbtree                The @ref RBTree to be operated on.
 */
void rbtree_remove_all(RBTree *rbtree);

/* ========================================================================================================
 *
 *                                                 MACROS
 *
 * ======================================================================================================== */

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "parent" member of the
 * root @ref RBTreeNode in a chain of removed @ref RBTreeNode's or a single removed @ref RBTreeNode. Useful
 * for identifying bugs.
 */
#define RBTREE_POISON_PARENT ((RBTreeNode*) 0x100)

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "left_child" member of
 * the root @ref RBTreeNode in a chain of removed @ref RBTreeNode's or a single removed @ref RBTreeNode.
 * Useful for identifying bugs.
 */
#define RBTREE_POISON_LEFT_CHILD ((RBTreeNode*) 0x200)

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "right_child" member of
 * the root @ref RBTreeNode in a chain of removed @ref RBTreeNode's or a single removed @ref RBTreeNode.
 * Useful for identifying bugs.
 */
#define RBTREE_POISON_RIGHT_CHILD ((RBTreeNode*) 0x300)

/**
 * Initializing a @ref RBTreeNode before it is used is NOT required. This macro is simply for allowing you to
 * initialize a struct (containing one or more @ref RBTreeNode's) with an initializer-list conveniently.
 */
#define RBTREE_NODE_INIT { RBTREE_POISON_PARENT, RBTREE_POISON_LEFT_CHILD, RBTREE_POISON_RIGHT_CHILD, RBTREE_NODE_RED }

/**
 * Obtains the pointer to the struct for this entry.
 *
 * Requirements:
 *      -   @ref node_ptr != NULL
 *
 * @param node_ptr              The pointer to the @ref RBTreeNode in the struct.
 * @param type                  The type of the struct the @ref RBTreeNode is embedded in.
 * @param member                The name of the @ref RBTreeNode in the struct.
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define rbtree_entry(node_ptr, type, member) \
        ({ \
            const typeof(((type*)0)->member) *__mptr = (node_ptr); \
            (type*) ((char*)__mptr - offsetof(type, member)); \
        })
#else
    #define rbtree_entry(node_ptr, type, member) \
        ( \
            (type*) ((char*)(node_ptr) - offsetof(type, member)) \
        )
#endif

/**
 * Iterates over the @ref RBTree (inorder) from the first @ref RBTreeNode to the last @ref RBTreeNode.
 *
 * Requirements:
 *      -   @ref rbtree_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param rbtree_ptr            The pointer to a @ref RBTree that will be iterated over.
 */
#define rbtree_for_each(cursor_node_ptr, rbtree_ptr) \
    for ( \
        cursor_node_ptr = rbtree_first(rbtree_ptr); \
        cursor_node_ptr; \
        cursor_node_ptr = rbtree_next(cursor_node_ptr) \
    )

/**
 * Iterates over the @ref RBTree (inorder) from the last @ref RBTreeNode to the first @ref RBTreeNode.
 *
 * Requirements:
 *      -   @ref rbtree_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param rbtree_ptr            The pointer to a @ref RBTree that will be iterated over.
 */
#define rbtree_for_each_reverse(cursor_node_ptr, rbtree_ptr) \
    for ( \
        cursor_node_ptr = rbtree_last(rbtree_ptr); \
        cursor_node_ptr; \
        cursor_node_ptr = rbtree_prev(cursor_node_ptr) \
    )

/**
 * Iterates over the @ref RBTree (inorder) from the first @ref RBTreeNode to the last @ref RBTreeNode, and is
 * safe against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref rbtree_ptr != NULL.
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref RBTreeNode to use as a temporary storage.
 * @param rbtree_ptr            The pointer to a @ref RBTree that will be iterated over.
 */
#define rbtree_for_each_safe(cursor_node_ptr, backup_node_ptr, rbtree_ptr) \
    for ( \
        cursor_node_ptr = rbtree_first(rbtree_ptr), \
        backup_node_ptr = rbtree_next(cursor_node_ptr); \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = rbtree_next(backup_node_ptr) \
    )

/**
 * Iterates over the @ref RBTree (inorder) from the last @ref RBTreeNode to the first @ref RBTreeNode, and is
 * safe against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref rbtree_ptr != NULL.
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref RBTreeNode to use as a temporary storage.
 * @param rbtree_ptr            The pointer to a @ref RBTree that will be iterated over.
 */
#define rbtree_for_each_safe_reverse(cursor_node_ptr, backup_node_ptr, rbtree_ptr) \
    for ( \
        cursor_node_ptr = rbtree_last(rbtree_ptr), \
        backup_node_ptr = rbtree_prev(cursor_node_ptr); \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = rbtree_prev(backup_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree, continuing AFTER the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 */
#define rbtree_for_each_after(cursor_node_ptr) \
    for ( \
        cursor_node_ptr = rbtree_next(cursor_node_ptr); \
        cursor_node_ptr; \
        cursor_node_ptr = rbtree_next(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree in reverse, continuing AFTER the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 */
#define rbtree_for_each_after_reverse(cursor_node_ptr) \
    for ( \
        cursor_node_ptr = rbtree_prev(cursor_node_ptr); \
        cursor_node_ptr; \
        cursor_node_ptr = rbtree_prev(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree, continuing AFTER the current position, and is safe
 * against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref RBTreeNode to use as temporary storage.
 */
#define rbtree_for_each_safe_after(cursor_node_ptr, backup_node_ptr) \
    for ( \
        cursor_node_ptr = rbtree_next(cursor_node_ptr), \
        backup_node_ptr = rbtree_next(cursor_node_ptr); \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = rbtree_next(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree in reverse, continuing AFTER the current position, and
 * is safe against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref RBTreeNode to use as temporary storage.
 */
#define rbtree_for_each_safe_after_reverse(cursor_node_ptr, backup_node_ptr) \
    for ( \
        cursor_node_ptr = rbtree_prev(cursor_node_ptr), \
        backup_node_ptr = rbtree_prev(cursor_node_ptr); \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = rbtree_prev(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree, continuing FROM the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 */
#define rbtree_for_each_from(cursor_node_ptr) \
    for ( \
        ; \
        cursor_node_ptr; \
        cursor_node_ptr = rbtree_next(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree in reverse, continuing FROM the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 */
#define rbtree_for_each_from_reverse(cursor_node_ptr) \
    for ( \
        ; \
        cursor_node_ptr; \
        cursor_node_ptr = rbtree_prev(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree, continuing FROM the current position, and is safe
 * against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref RBTreeNode to use as temporary storage.
 */
#define rbtree_for_each_safe_from(cursor_node_ptr, backup_node_ptr) \
    for ( \
        backup_node_ptr = rbtree_next(cursor_node_ptr); \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = rbtree_next(cursor_node_ptr) \
    )

/**
 * Continues iterating (inorder) over the @ref RBTree in reverse, continuing FROM the current position, and is
 * safe against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref RBTree in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref RBTreeNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref RBTreeNode to use as temporary storage.
 */
#define rbtree_for_each_safe_from_reverse(cursor_node_ptr, backup_node_ptr) \
    for ( \
        backup_node_ptr = rbtree_prev(cursor_node_ptr); \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = rbtree_prev(cursor_node_ptr) \
    )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RBTREE_H */
