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

#include "rbtree.h"

/* ========================================================================================================
 *
 *                                        STATIC FUNCTION PROTOTYPES
 *
 * ======================================================================================================== */

/*
 * Returns the color of the @ref node. If @ref node == NULL, return @ref RBTREE_NODE_BLACK.
 */
static RBTreeNodeColor color(const RBTreeNode *node);

/*
* Returns the sibling of the @ref node.
*/
static RBTreeNode* sibling(const RBTreeNode *node);

/*
* Returns the grandparent of the @ref node.
*/
static RBTreeNode* grandparent(const RBTreeNode *node);

/*
* Returns the uncle of the @ref node.
*/
static RBTreeNode* uncle(const RBTreeNode *node);

/*
 * Replaces the @ref old_node with the @ref new_node in the @ref rbtree.
 */
static void replace(RBTree *rbtree, RBTreeNode *old_node, RBTreeNode *new_node);

/*
 * Performs a transplant on the @ref old_node and the @ref new_node in the @ref rbtree.
 */
static void transplant(RBTree *rbtree, RBTreeNode *old_node, RBTreeNode *new_node);

/*
 * Swaps the places of the @ref high_node and @ref low_node in the @ref rbtree. The @ref high_node must be
 * higher in the @ref rbtree than the @ref low_node.
 */
static void swap_places(RBTree *rbtree, RBTreeNode *high_node, RBTreeNode *low_node);

/*
 * Performs a left rotation around the @ref node in the @ref rbtree.
 */
static void rotate_left(RBTree *rbtree, RBTreeNode *node);

/*
 * Performs a right rotation around the @ref node in the @ref rbtree.
 */
static void rotate_right(RBTree *rbtree, RBTreeNode *node);

/*
 * Repairs the @ref rbtree after the insertion of the @ref node.
 */
static void repair_after_insert(RBTree *rbtree, RBTreeNode *node);

/*
 * Repairs the @ref rbtree after the removal of the @ref node.
 */
static void repair_after_remove(RBTree *rbtree, RBTreeNode *node);

/* ========================================================================================================
 *
 *                                        STATIC FUNCTION DEFINITIONS
 *
 * ======================================================================================================== */

static RBTreeNodeColor color(const RBTreeNode *node) {
    return node ? node->color : RBTREE_NODE_BLACK;
}

static RBTreeNode* sibling(const RBTreeNode *node) {
    assert(node && node->parent);

    if (node == node->parent->left_child) {
        return node->parent->right_child;
    } else {
        return node->parent->left_child;
    }
}

static RBTreeNode* grandparent(const RBTreeNode *node) {
    assert(node && node->parent && node->parent->parent);

    return node->parent->parent;
}

static RBTreeNode* uncle(const RBTreeNode *node) {
    assert(node && node->parent && node->parent->parent);

    return sibling(node->parent);
}

static void replace(RBTree *rbtree, RBTreeNode *old_node, RBTreeNode *new_node) {
    assert(rbtree && old_node && new_node);

    if (rbtree->root == old_node) {
        rbtree->root = new_node;
    } else if (old_node == old_node->parent->left_child) {
        old_node->parent->left_child = new_node;
    } else {
        old_node->parent->right_child = new_node;
    }

    if (old_node->left_child) {
        old_node->left_child->parent = new_node;
    }

    if (old_node->right_child) {
        old_node->right_child->parent = new_node;
    }

    *new_node = *old_node;

    old_node->parent = RBTREE_POISON_PARENT;
    old_node->left_child = RBTREE_POISON_LEFT_CHILD;
    old_node->right_child = RBTREE_POISON_RIGHT_CHILD;
}

static void transplant(RBTree *rbtree, RBTreeNode *old_node, RBTreeNode *new_node) {
    assert(rbtree && old_node);

    if (!old_node->parent) {
        rbtree->root = new_node;
    } else if (old_node == old_node->parent->left_child) {
        old_node->parent->left_child = new_node;
    } else {
        old_node->parent->right_child = new_node;
    }

    if (new_node) {
        new_node->parent = old_node->parent;
    }
}

static void swap_places(RBTree *rbtree, RBTreeNode *high_node, RBTreeNode *low_node) {
    RBTreeNode high_cpy;

    assert(rbtree && high_node && low_node);

    if (!high_node->parent) {
        rbtree->root = low_node;
    } else if (high_node->parent->left_child == high_node) {
        high_node->parent->left_child = low_node;
    } else {
        high_node->parent->right_child = low_node;
    }

    if (low_node->left_child) {
        low_node->left_child->parent = high_node;
    }

    if (low_node->right_child) {
        low_node->right_child->parent = high_node;
    }

    if (high_node->left_child == low_node) {
        if (high_node->right_child) {
            high_node->right_child->parent = low_node;
        }

        high_node->left_child = high_node;
        low_node->parent = low_node;
    } else if (high_node->right_child == low_node) {
        if (high_node->left_child) {
            high_node->left_child->parent = low_node;
        }

        high_node->right_child = high_node;
        low_node->parent = low_node;
    } else {
        if (high_node->left_child) {
            high_node->left_child->parent = low_node;
        }

        if (high_node->right_child) {
            high_node->right_child->parent = low_node;
        }

        if (low_node->parent->left_child == low_node) {
            low_node->parent->left_child = high_node;
        } else {
            low_node->parent->right_child = high_node;
        }
    }

    high_cpy = *high_node;
    *high_node = *low_node;
    *low_node = high_cpy;
}

static void rotate_left(RBTree *rbtree, RBTreeNode *node) {
    RBTreeNode *n;

    assert(rbtree && node);

    n = node->right_child;

    transplant(rbtree, node, n);

    node->right_child = n->left_child;

    if (n->left_child) {
        n->left_child->parent = node;
    }

    n->left_child = node;
    node->parent = n;
}

static void rotate_right(RBTree *rbtree, RBTreeNode *node) {
    RBTreeNode *n;

    assert(rbtree && node);

    n = node->left_child;

    transplant(rbtree, node, n);

    node->left_child = n->right_child;

    if (n->right_child) {
        n->right_child->parent = node;
    }

    n->right_child = node;
    node->parent = n;
}

static void repair_after_insert(RBTree *rbtree, RBTreeNode *node) {
    assert(rbtree && node);

    for ( ; ; ) {
        if (!node->parent) {
            node->color = RBTREE_NODE_BLACK;

            break;
        }

        if (color(node->parent) == RBTREE_NODE_BLACK) {
            break;
        }

        if (color(uncle(node)) == RBTREE_NODE_RED) {
            node->parent->color = RBTREE_NODE_BLACK;
            uncle(node)->color = RBTREE_NODE_BLACK;
            grandparent(node)->color = RBTREE_NODE_RED;
            node = grandparent(node);

            continue;
        }

        if (node == node->parent->right_child && node->parent == grandparent(node)->left_child) {
            rotate_left(rbtree, node->parent);

            node = node->left_child;
        } else if (node == node->parent->left_child && node->parent == grandparent(node)->right_child) {
            rotate_right(rbtree, node->parent);

            node = node->right_child;
        }

        node->parent->color = RBTREE_NODE_BLACK;
        grandparent(node)->color = RBTREE_NODE_RED;

        if (node == node->parent->left_child && node->parent == grandparent(node)->left_child) {
            rotate_right(rbtree, grandparent(node));
        } else {
            rotate_left(rbtree, grandparent(node));
        }

        break;
    }
}

static void repair_after_remove(RBTree *rbtree, RBTreeNode *node) {
    assert(rbtree && node);

    for ( ; ; ) {
        if (!node->parent) {
            break;
        }

        if (color(sibling(node)) == RBTREE_NODE_RED) {
            node->parent->color = RBTREE_NODE_RED;
            sibling(node)->color = RBTREE_NODE_BLACK;

            if (node == node->parent->left_child) {
                rotate_left(rbtree, node->parent);
            } else {
                rotate_right(rbtree, node->parent);
            }
        }

        if (
            color(node->parent) == RBTREE_NODE_BLACK &&
            color(sibling(node)) == RBTREE_NODE_BLACK &&
            color(sibling(node)->left_child) == RBTREE_NODE_BLACK &&
            color(sibling(node)->right_child) == RBTREE_NODE_BLACK
        ) {
            sibling(node)->color = RBTREE_NODE_RED;
            node = node->parent;

            continue;
        }

        if (
            color(node->parent) == RBTREE_NODE_RED &&
            color(sibling(node)) == RBTREE_NODE_BLACK &&
            color(sibling(node)->left_child) == RBTREE_NODE_BLACK &&
            color(sibling(node)->right_child) == RBTREE_NODE_BLACK
        ) {
            sibling(node)->color = RBTREE_NODE_RED;
            node->parent->color = RBTREE_NODE_BLACK;

            break;
        }

        if (
            node == node->parent->left_child &&
            color(sibling(node)) == RBTREE_NODE_BLACK &&
            color(sibling(node)->left_child) == RBTREE_NODE_RED &&
            color(sibling(node)->right_child) == RBTREE_NODE_BLACK
        ) {
            sibling(node)->color = RBTREE_NODE_RED;
            sibling(node)->left_child->color = RBTREE_NODE_BLACK;

            rotate_right(rbtree, sibling(node));
        } else if (
            node == node->parent->right_child &&
            color(sibling(node)) == RBTREE_NODE_BLACK &&
            color(sibling(node)->left_child) == RBTREE_NODE_BLACK &&
            color(sibling(node)->right_child) == RBTREE_NODE_RED
        ) {
            sibling(node)->color = RBTREE_NODE_RED;
            sibling(node)->right_child->color = RBTREE_NODE_BLACK;

            rotate_left(rbtree, sibling(node));
        }

        sibling(node)->color = color(node->parent);
        node->parent->color = RBTREE_NODE_BLACK;

        if (node == node->parent->left_child) {
            sibling(node)->right_child->color = RBTREE_NODE_BLACK;

            rotate_left(rbtree, node->parent);
        } else {
            sibling(node)->left_child->color = RBTREE_NODE_BLACK;

            rotate_right(rbtree, node->parent);
        }

        break;
    }
}

/* ========================================================================================================
 *
 *                                        EXTERN FUNCTION DEFINITIONS
 *
 * ======================================================================================================== */

void rbtree_init(
    RBTree *rbtree,
    int (*compare)(const void *key, const RBTreeNode *node),
    void (*collide)(const RBTreeNode *old_node, const RBTreeNode *new_node, void *auxiliary_data),
    void *auxiliary_data
) {
    assert(rbtree && compare);

    rbtree->compare = compare;
    rbtree->collide = collide;
    rbtree->auxiliary_data = auxiliary_data;
    rbtree->root = NULL;
    rbtree->size = 0;
}

RBTreeNode* rbtree_first(const RBTree *rbtree) {
    RBTreeNode *n;

    assert(rbtree);

    n = rbtree->root;

    if (!n) {
        return NULL;
    }

    while (n->left_child) {
        n = n->left_child;
    }

    return n;
}

RBTreeNode* rbtree_last(const RBTree *rbtree) {
    RBTreeNode *n;

    assert(rbtree);

    n = rbtree->root;

    if (!n) {
        return NULL;
    }

    while (n->right_child) {
        n = n->right_child;
    }

    return n;
}

RBTreeNode* rbtree_prev(const RBTreeNode *node) {
    RBTreeNode *n;

    if (!node) {
        return NULL;
    }

    if (node->left_child) {
        node = node->left_child;

        while (node->right_child) {
            node = node->right_child;
        }

        return (RBTreeNode*) node;
    }

    while ((n = node->parent) && node == n->left_child) {
        node = n;
    }

    return n;
}

RBTreeNode* rbtree_next(const RBTreeNode *node) {
    RBTreeNode *n;

    if (!node) {
        return NULL;
    }

    if (node->right_child) {
        node = node->right_child;

        while (node->left_child) {
            node = node->left_child;
        }

        return (RBTreeNode*) node;
    }

    while ((n = node->parent) && node == n->right_child) {
        node = n;
    }

    return n;
}

size_t rbtree_size(const RBTree *rbtree) {
    assert(rbtree);

    return rbtree->size;
}

int rbtree_empty(const RBTree *rbtree) {
    assert(rbtree);

    return rbtree->size == 0;
}

int rbtree_contains_key(const RBTree *rbtree, const void *key) {
    assert(rbtree);

    return rbtree_lookup_key(rbtree, key) != NULL;
}

size_t rbtree_index_of(const RBTree *rbtree, const RBTreeNode *node) {
    assert(rbtree && node);

    if (rbtree_last(rbtree) == node) {
        return rbtree->size - 1;
    } else {
        RBTreeNode *n;
        size_t i = 0;
        rbtree_for_each(n, rbtree) {
            if (n == node) {
                return i;
            }
            ++i;
        }
    }

    assert(0);
    return (size_t) -1;
}

RBTreeNode* rbtree_at(const RBTree *rbtree, size_t index) {
    RBTreeNode *n;
    size_t i;

    assert(rbtree && index < rbtree->size);

    if (index < rbtree->size / 2) {
        i = 0;
        rbtree_for_each(n, rbtree) {
            if (i == index) {
                return n;
            }
            ++i;
        }
    } else {
        i = rbtree->size - 1;
        rbtree_for_each_reverse(n, rbtree) {
            if (i == index) {
                return n;
            }
            --i;
        }
    }

    assert(0);
    return NULL;
}

void rbtree_insert(RBTree *rbtree, const void *key, RBTreeNode *node) {
    RBTreeNode *n;

    assert(rbtree && node);

    n = rbtree->root;

    if (n) {
        for ( ; ; ) {
            int cmp = rbtree->compare(key, n);

            if (cmp < 0) {
                if (n->left_child) {
                    n = n->left_child;
                } else {
                    n->left_child = node;

                    break;
                }
            } else if (cmp > 0) {
                if (n->right_child) {
                    n = n->right_child;
                } else {
                    n->right_child = node;

                    break;
                }
            } else {
                replace(rbtree, n, node);

                if (rbtree->collide) {
                    rbtree->collide(n, node, rbtree->auxiliary_data);
                }

                return;
            }
        }
    }

    node->parent = n;
    node->left_child = NULL;
    node->right_child = NULL;
    node->color = RBTREE_NODE_RED;

    if (!n) {
        rbtree->root = node;
    }

    repair_after_insert(rbtree, node);

    ++rbtree->size;
}

RBTreeNode* rbtree_lookup_key(const RBTree *rbtree, const void *key) {
    RBTreeNode *n;

    assert(rbtree);

    for (n = rbtree->root; n; ) {
        int cmp = rbtree->compare(key, n);

        if (cmp < 0) {
            n = n->left_child;
        } else if (cmp > 0) {
            n = n->right_child;
        } else {
            break;
        }
    }

    return n;
}

void rbtree_remove(RBTree *rbtree, RBTreeNode *node) {
    RBTreeNode *n;

    assert(rbtree);

    if (!node) {
        return;
    }

    if (node->left_child && node->right_child) {
        RBTreeNode *k = node->left_child;

        while (k->right_child) {
            k = k->right_child;
        }

        swap_places(rbtree, node, k);
    }

    n = node->right_child ? node->right_child : node->left_child;

    if (color(node) == RBTREE_NODE_BLACK) {
        node->color = color(n);

        repair_after_remove(rbtree, node);
    }

    transplant(rbtree, node, n);

    if (!node->parent && n) {
        n->color = RBTREE_NODE_BLACK;
    }

    node->parent = RBTREE_POISON_PARENT;
    node->left_child = RBTREE_POISON_LEFT_CHILD;
    node->right_child = RBTREE_POISON_RIGHT_CHILD;

    --rbtree->size;
}

void rbtree_remove_key(RBTree *rbtree, const void *key) {
    assert(rbtree);

    rbtree_remove(rbtree, rbtree_lookup_key(rbtree, key));
}

void rbtree_remove_first(RBTree *rbtree) {
    assert(rbtree);

    rbtree_remove(rbtree, rbtree_first(rbtree));
}

void rbtree_remove_last(RBTree *rbtree) {
    assert(rbtree);

    rbtree_remove(rbtree, rbtree_last(rbtree));
}

void rbtree_remove_all(RBTree *rbtree) {
    assert(rbtree);

    if (rbtree->root) {
        rbtree->root->parent = RBTREE_POISON_PARENT;
        rbtree->root->left_child = RBTREE_POISON_LEFT_CHILD;
        rbtree->root->right_child = RBTREE_POISON_RIGHT_CHILD;
    }

    rbtree->root = NULL;
    rbtree->size = 0;
}
