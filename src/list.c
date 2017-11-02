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
#include <string.h>
#include <assert.h>

#include "list.h"

/* ========================================================================================================
 *
 *                                        STATIC FUNCTION PROTOTYPES
 *
 * ======================================================================================================== */

/*
 * Free's the node and, if the list has ownership, also frees the data stored inside the node.
 *
 * No reassignment takes place on the list or its members.
 *
 * Returns the data that was stored inside the node if the list does NOT have ownership; otherwise, returns
 * NULL.
 */
static void* list_free_single_node__(List *list, ListNode *node);

/*
 * Free's all ListNodes in the list. If the list has ownership, the data stored in each ListNode is also
 * freed.
 *
 * No reassignment takes place on the list or its members.
 */
static void list_free_all_nodes__(List *list);

/*
 * Inserts the range [from, to] into the list, resulting in the following ListNode connectivity:
 *      ... <-> left <-> from <-> ... <-> to <-> right <-> ...
 *
 * The list's size is then incrememented to account for the new ListNode(s).
 */
static void list_insert_range__(List *list, ListNode *left, ListNode *from, ListNode *to, ListNode *right, size_t range_size);

/*
 * Removes the range [from, to] from the list, resulting in the following ListNode connectivity:
 *      ... <-> ...
 *
 * The list's size is then decremented to account for the loss of the ListNode(s).
 *
 * The removed ListNode(s) and their associated data stored within are NOT freed.
 */
static void list_delete_range_no_free__(List *list, ListNode *from, ListNode *to, size_t range_size);

/* ========================================================================================================
 *
 *                                        STATIC FUNCTION DEFINITIONS
 *
 * ======================================================================================================== */

static void* list_free_single_node__(List *list, ListNode *node) {
    // ONLY free data if list has ownership.
    if (list->data_free != NULL) {
        list->data_free(node->data);
        list->list_free(node);
        return NULL;
    } else {
        void *data = node->data;
        list->list_free(node);
        return data;
    }
}

static void list_free_all_nodes__(List *list) {
    if (list->size > 0) {
        // ONLY free data if list has ownership. These loops are optimized for speed, so the use of an extra
        // variable to make this loop "safe" (sort of like list_for_each_safe) is not needed thanks to the way
        // these loops are implemented.
        if (list->data_free != NULL) {
            for (ListNode *n = list->head->next; n != NULL; n = n->next) {
                list->data_free(n->prev->data);
                list->list_free(n->prev);
            }
            list->data_free(list->tail->data);
            list->list_free(list->tail);
        } else {
            for (ListNode *n = list->head->next; n != NULL; n = n->next) {
                list->list_free(n->prev);
            }
            list->list_free(list->tail);
        }
    }
}

static void list_insert_range__(List *list, ListNode *left, ListNode *from, ListNode *to, ListNode *right, size_t range_size) {
    if (left == NULL) {
        from->prev = NULL;
        list->head = from;
    } else {
        from->prev = left;
        left->next = from;
    }
    if (right == NULL) {
        to->next = NULL;
        list->tail = to;
    } else {
        to->next = right;
        right->prev = to;
    }

    list->size += range_size;
}

static void list_delete_range_no_free__(List *list, ListNode *from, ListNode *to, size_t range_size) {
    if (list->head == from) {
        list->head = to->next;
    } else {
        from->prev->next = to->next;
    }
    if (list->tail == to) {
        list->tail = from->prev;
    } else {
        to->next->prev = from->prev;
    }

    list->size -= range_size;
}

/* ========================================================================================================
 *
 *                                        EXTERN FUNCTION DEFINITIONS
 *
 * ======================================================================================================== */

List* list_create(void* (*list_malloc)(size_t), void (*list_free)(void*), void (*data_free)(void*)) {
    assert(list_malloc != NULL);
    assert(list_free != NULL);

    List *list = (List*) list_malloc(sizeof(List));
    if (list == NULL) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->list_malloc = list_malloc;
    list->list_free = list_free;
    list->data_free = data_free;
    list->size = 0;

    return list;
}

void list_destroy(void *list) {
    if (list == NULL) {
        return;
    }

    List *list_ptr = (List*) list;

    list_free_all_nodes__(list_ptr);

    list_ptr->list_free(list_ptr);
}

void** list_to_array(List *list) {
    assert(list != NULL);

    void **array = (void **) list->list_malloc(list->size * sizeof(void*));
    if (array == NULL) {
        return NULL;
    }

    size_t i = 0;
    list_for_each(n, list) {
        array[i] = n->data;
        ++i;
    }

    return array;
}

size_t list_index_of(List *list, ListNode *node) {
    assert(list != NULL);
    assert(node != NULL);

    // Micro-optimization for cases where list->tail == node, since we iterate from head to tail.
    if (list->tail == node) {
        return list->size - 1;
    }

    size_t i = 0;
    list_for_each(n, list) {
        if (n == node) {
            return i;
        }
        ++i;
    }

    // Reaching this point means that the list and node are not related.
    assert(0);
    return (size_t) -1;
}

ListNode* list_at(List *list, size_t index) {
    assert(list != NULL);
    assert(index < list->size);

    // Figure out if it's relatively quicker to iterate from front to back or back to front.
    if (index < list->size / 2) {
        size_t i = 0;
        list_for_each(n, list) {
            if (i == index) {
                return n;
            }
            ++i;
        }
    } else {
        size_t i = list->size - 1;
        list_for_each_reverse(n, list) {
            if (i == index) {
                return n;
            }
            --i;
        }
    }

    // Out of bounds.
    assert(0);
    return NULL;
}

ListNode* list_insert_left(List *list, void *data, ListNode *node) {
    assert(list != NULL);
    assert(node != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->data = data;

    list_insert_range__(list, node->prev, new_node, new_node, node, 1);

    return new_node;
}

ListNode* list_insert_right(List *list, void *data, ListNode *node) {
    assert(list != NULL);
    assert(node != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->data = data;

    list_insert_range__(list, node, new_node, new_node, node->next, 1);

    return new_node;
}

ListNode* list_push_front(List *list, void *data) {
    assert(list != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->data = data;

    list_insert_range__(list, NULL, new_node, new_node, list->head, 1);

    return new_node;
}

ListNode* list_push_back(List *list, void *data) {
    assert(list != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->data = data;

    list_insert_range__(list, list->tail, new_node, new_node, NULL, 1);

    return new_node;
}

void list_clear(List *list) {
    assert(list != NULL);

    list_free_all_nodes__(list);

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void* list_remove(List *list, ListNode *node) {
    assert(list != NULL);

    if (node == NULL) {
        return NULL;
    }

    list_delete_range_no_free__(list, node, node, 1);

    return list_free_single_node__(list, node);
}

void* list_pop_front(List *list) {
    assert(list != NULL);

    if (list->size == 0) {
        return NULL;
    }

    ListNode *node = list->head;
    list_delete_range_no_free__(list, node, node, 1);

    return list_free_single_node__(list, node);
}

void* list_pop_back(List *list) {
    assert(list != NULL);

    if (list->size == 0) {
        return NULL;
    }

    ListNode *node = list->tail;
    list_delete_range_no_free__(list, node, node, 1);

    return list_free_single_node__(list, node);
}

// Iterative merge sort adapted from:
// http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
// https://stackoverflow.com/questions/7685/merge-sort-a-linked-list
void list_sort(List *list, int (*compare_data)(const void*, const void*)) {
    assert(list != NULL);
    assert(compare_data != NULL);

    if (list->size < 2) {
        return;
    }

    size_t list_size = 1, num_merges, left_size, right_size;
    ListNode *head = list->head, *tail, *left, *right, *next;

    do {
        num_merges = 0;
        left = head;
        head = NULL;
        tail = NULL;

        while (left != NULL) {
            ++num_merges;
            right = left;
            left_size = 0;
            right_size = list_size;

            while (right != NULL && left_size < list_size) {
                ++left_size;
                right = right->next;
            }

            while (left_size > 0 || (right_size > 0 && right != NULL)) {
                if (left_size == 0) {
                    next = right;
                    right = right->next;
                    --right_size;
                } else if (right_size == 0 || right == NULL) {
                    next = left;
                    left = left->next;
                    --left_size;
                } else if (compare_data(left->data, right->data) <= 0) {
                    next = left;
                    left = left->next;
                    --left_size;
                } else {
                    next = right;
                    right = right->next;
                    --right_size;
                }

                if (tail != NULL) {
                    tail->next = next;
                } else {
                    head = next;
                }

                next->prev = tail;
                tail = next;
            }

            left = right;
        }

        tail->next = NULL;
        list_size <<= 1;
    } while (num_merges > 1);

    list->head = head;
    list->tail = tail;
}

void list_splice_left(List *list1, ListNode *node, List *list2, ListNode *from, ListNode *to, size_t range_size) {
    assert(list1 != NULL);
    assert(node != NULL);
    assert(list2 != NULL);
    assert(list1->list_malloc == list2->list_malloc);
    assert(list1->list_free == list2->list_free);
    assert(list1->data_free == list2->data_free);
    assert((from != NULL && to != NULL) || (from == NULL && to == NULL));

    if (range_size == 0) {
        return;
    }

    list_delete_range_no_free__(list2, from, to, range_size);
    list_insert_range__(list1, node->prev, from, to, node, range_size);
}

void list_splice_right(List *list1, ListNode *node, List *list2, ListNode *from, ListNode *to, size_t range_size) {
    assert(list1 != NULL);
    assert(node != NULL);
    assert(list2 != NULL);
    assert(list1->list_malloc == list2->list_malloc);
    assert(list1->list_free == list2->list_free);
    assert(list1->data_free == list2->data_free);
    assert((from != NULL && to != NULL) || (from == NULL && to == NULL));

    if (range_size == 0) {
        return;
    }

    list_delete_range_no_free__(list2, from, to, range_size);
    list_insert_range__(list1, node, from, to, node->next, range_size);
}

void list_splice_front(List *list1, List *list2, ListNode *from, ListNode *to, size_t range_size) {
    assert(list1 != NULL);
    assert(list2 != NULL);
    assert(list1->list_malloc == list2->list_malloc);
    assert(list1->list_free == list2->list_free);
    assert(list1->data_free == list2->data_free);
    assert((from != NULL && to != NULL) || (from == NULL && to == NULL));

    if (range_size == 0) {
        return;
    }

    list_delete_range_no_free__(list2, from, to, range_size);
    list_insert_range__(list1, NULL, from, to, list1->head, range_size);
}

void list_splice_back(List *list1, List *list2, ListNode *from, ListNode *to, size_t range_size) {
    assert(list1 != NULL);
    assert(list2 != NULL);
    assert(list1->list_malloc == list2->list_malloc);
    assert(list1->list_free == list2->list_free);
    assert(list1->data_free == list2->data_free);
    assert((from != NULL && to != NULL) || (from == NULL && to == NULL));

    if (range_size == 0) {
        return;
    }

    list_delete_range_no_free__(list2, from, to, range_size);
    list_insert_range__(list1, list1->tail, from, to, NULL, range_size);
}
