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

List* list_create(void* (*list_malloc)(size_t), void (*list_free)(void*), void (*data_free)(void*)) {
    assert(list_malloc != NULL && list_free != NULL);

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

void list_destroy(List *list) {
    if (list == NULL) {
        return;
    }

    if (list->size > 0) {
        // Only free data if list has ownership.
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

    list->list_free(list);
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

ListNode* list_find(List *list, void *data) {
    assert(list != NULL);

    list_for_each(n, list) {
        if (n->data == data) {
            return n;
        }
    }

    return NULL;
}

size_t list_position(List *list, ListNode *node) {
    assert(list != NULL && node != NULL);

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
    assert(list != NULL && index < list->size);

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

    // This will only return NULL if assertions are disabled and index is out of bounds.
    assert(0);
    return NULL;
}

ListNode* list_insert(List *list, void *data, size_t index) {
    assert(list != NULL && index <= list->size);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;

    if (list->size == 0) {
        new_node->prev = NULL;
        new_node->next = NULL;
        list->head = new_node;
        list->tail = new_node;
    } else if (index == 0) {
        new_node->prev = NULL;
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    } else if (index == list->size) {
        new_node->prev = list->tail;
        new_node->next = NULL;
        list->tail->next = new_node;
        list->tail = new_node;
    } else {
        ListNode *node_at_index;

        // Figure out if it's relatively quicker to iterate from front to back or back to front.
        if (index < list->size / 2) {
            size_t i = 0;
            list_for_each(n, list) {
                if (i == index) {
                    node_at_index = n;
                    break;
                }
                ++i;
            }
        } else {
            size_t i = list->size - 1;
            list_for_each_reverse(n, list) {
                if (i == index) {
                    node_at_index = n;
                    break;
                }
                --i;
            }
        }

        new_node->prev = node_at_index->prev;
        new_node->next = node_at_index;
        node_at_index->prev->next = new_node;
        node_at_index->prev = new_node;
    }
    ++list->size;

    return new_node;
}

ListNode* list_insert_left(List *list, void *data, ListNode *node) {
    assert(list != NULL && node != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;

    new_node->next = node;
    if (node->prev == NULL) {
        new_node->prev = NULL;
        list->head = new_node;
    } else {
        new_node->prev = node->prev;
        node->prev->next = new_node;
    }
    node->prev = new_node;
    ++list->size;

    return new_node;
}

ListNode* list_insert_right(List *list, void *data, ListNode *node) {
    assert(list != NULL && node != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;

    new_node->prev = node;
    if (node->next == NULL) {
        new_node->next = NULL;
        list->tail = new_node;
    } else {
        new_node->next = node->next;
        node->next->prev = new_node;
    }
    node->next = new_node;
    ++list->size;

    return new_node;
}

ListNode* list_push_front(List *list, void *data) {
    assert(list != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;

    new_node->prev = NULL;
    if (list->size == 0) {
        new_node->next = NULL;
        list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
    }
    list->head = new_node;
    ++list->size;

    return new_node;
}

ListNode* list_push_back(List *list, void *data) {
    assert(list != NULL);

    ListNode *new_node = (ListNode*) list->list_malloc(sizeof(ListNode));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;

    new_node->next = NULL;
    if (list->size == 0) {
        new_node->prev = NULL;
        list->head = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
    }
    list->tail = new_node;
    ++list->size;

    return new_node;
}

void list_clear(List *list) {
    assert(list != NULL);

    // If list is empty, return. Under no circumstance should an empty list have non-null head/tail pointers.
    if (list->size == 0) {
        assert(list->head == NULL);
        assert(list->tail == NULL);
        return;
    }

    // Only free data if list has ownership.
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

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void* list_delete(List *list, size_t index) {
    assert(list != NULL && index < list->size);

    ListNode *node_at_index;

    if (list->size == 1) {
        node_at_index = list->head;
        list->head = NULL;
        list->tail = NULL;
    } else if (index == 0) {
        node_at_index = list->head;
        list->head->next->prev = NULL;
        list->head = list->head->next;
    } else if (index == list->size - 1) {
        node_at_index = list->tail;
        list->tail->prev->next = NULL;
        list->tail = list->tail->prev;
    } else {
        // Figure out if its relatively quicker to iterate from front to back or back to front.
        if (index < list->size / 2) {
            size_t i = 0;
            list_for_each(n, list) {
                if (i == index) {
                    node_at_index = n;
                    break;
                }
                ++i;
            }
        } else {
            size_t i = list->size - 1;
            list_for_each_reverse(n, list) {
                if (i == index) {
                    node_at_index = n;
                    break;
                }
                --i;
            }
        }

        node_at_index->prev->next = node_at_index->next;
        node_at_index->next->prev = node_at_index->prev;
    }
    --list->size;

    // Only free data if list has ownership.
    if (list->data_free != NULL) {
        list->data_free(node_at_index->data);
        list->list_free(node_at_index);
        return NULL;
    } else {
        void *data = node_at_index->data;
        list->list_free(node_at_index);
        return data;
    }
}

void* list_delete_node(List *list, ListNode *node) {
    assert(list != NULL && node != NULL);

    if (node->prev == NULL) {
        list->head = node->next;
    } else {
        node->prev->next = node->next;
    }
    if (node->next == NULL) {
        list->tail = node->prev;
    } else {
        node->next->prev = node->prev;
    }
    --list->size;

    // Only free data if list has ownership.
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

void* list_delete_left(List *list, ListNode *node) {
    assert(list != NULL && node != NULL && list->head != node);

    return list_delete_node(list, node->prev);
}

void* list_delete_right(List *list, ListNode *node) {
    assert(list != NULL && node != NULL && list->tail != node);

    return list_delete_node(list, node->next);
}

void* list_pop_front(List *list) {
    assert(list != NULL && list->head != NULL);

    return list_delete_node(list, list->head);
}

void* list_pop_back(List *list) {
    assert(list != NULL && list->tail != NULL);

    return list_delete_node(list, list->tail);
}

// Adapted from:
// http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
// https://stackoverflow.com/questions/7685/merge-sort-a-linked-list
void list_sort(List *list, int (*compare)(const void*, const void*)) {
    assert(list != NULL && compare != NULL);
    
    if (list->size < 2) {
        return;
    }

    size_t list_size = 1, num_merges, left_size, right_size;
    ListNode *head = list->head, *tail, *left, *right, *next;

    do {
        num_merges = 0;
        left = head;
        tail = head = NULL;

        while (left) {
            ++num_merges;
            right = left;
            left_size = 0;
            right_size = list_size;

            while (right && left_size < list_size) {
                ++left_size;
                right = right->next;
            }

            while (left_size > 0 || (right_size > 0 && right)) {
                if (!left_size) {
                    next = right;
                    right = right->next;
                    --right_size;
                } else if (!right_size || !right) {
                    next = left;
                    left = left->next;
                    --left_size;
                } else if (compare(left->data, right->data) <= 0) {
                    next = left;
                    left = left->next;
                    --left_size;
                } else {
                    next = right;
                    right = right->next;
                    --right_size;
                }

                if (tail) {
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
