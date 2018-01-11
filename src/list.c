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

#include "list.h"

void list_init(List *list) {
    assert(list);

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

ListNode* list_front(const List *list) {
    assert(list);

    return list->head;
}

ListNode* list_back(const List *list) {
    assert(list);

    return list->tail;
}

ListNode* list_prev(const ListNode *node) {
    return node ? node->prev : NULL;
}

ListNode* list_next(const ListNode *node) {
    return node ? node->next : NULL;
}

size_t list_size(const List *list) {
    assert(list);

    return list->size;
}

int list_empty(const List *list) {
    assert(list);

    return list->size == 0;
}

size_t list_index_of(const List *list, const ListNode *node) {
    assert(list && node);

    if (list->tail == node) {
        return list->size - 1;
    } else {
        ListNode *n;
        size_t i = 0;
        list_for_each(n, list) {
            if (n == node) {
                return i;
            }
            ++i;
        }
    }

    assert(0);
    return (size_t) -1;
}

ListNode* list_at(const List *list, size_t index) {
    ListNode *n;
    size_t i;

    assert(list && index < list->size);

    if (index < list->size / 2) {
        i = 0;
        list_for_each(n, list) {
            if (i == index) {
                return n;
            }
            ++i;
        }
    } else {
        i = list->size - 1;
        list_for_each_reverse(n, list) {
            if (i == index) {
                return n;
            }
            --i;
        }
    }

    assert(0);
    return NULL;
}

void list_insert_left(List *list, ListNode *new_node, ListNode *position) {
    assert(list && new_node);

    list_paste(list, position ? position->prev : NULL, new_node, new_node, position, 1);
}

void list_insert_right(List *list, ListNode *new_node, ListNode *position) {
    assert(list && new_node);

    list_paste(list, position, new_node, new_node, position ? position->next : NULL, 1);
}

void list_insert_front(List *list, ListNode *new_node) {
    assert(list && new_node);

    list_paste(list, NULL, new_node, new_node, list->head, 1);
}

void list_insert_back(List *list, ListNode *new_node) {
    assert(list && new_node);

    list_paste(list, list->tail, new_node, new_node, NULL, 1);
}

void list_splice_left(List *list, List *src_list, ListNode *position) {
    ListNode *from, *to;
    size_t range_size;

    assert(list && src_list);

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, position ? position->prev : NULL, from, to, position, range_size);
}

void list_splice_right(List *list, List *src_list, ListNode *position) {
    ListNode *from, *to;
    size_t range_size;

    assert(list && src_list);

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, position, from, to, position ? position->next : NULL, range_size);
}

void list_splice_front(List *list, List *src_list) {
    ListNode *from, *to;
    size_t range_size;

    assert(list && src_list);

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, NULL, from, to, list->head, range_size);
}

void list_splice_back(List *list, List *src_list) {
    ListNode *from, *to;
    size_t range_size;

    assert(list && src_list);

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, list->tail, from, to, NULL, range_size);
}

void list_remove(List *list, ListNode *node) {
    assert(list);

    list_cut(list, node, node, 1);
}

void list_remove_front(List *list) {
    assert(list);

    list_cut(list, list->head, list->head, 1);
}

void list_remove_back(List *list) {
    assert(list);

    list_cut(list, list->tail, list->tail, 1);
}

void list_remove_all(List *list) {
    assert(list);

    list_cut(list, list->head, list->tail, list->size);
}

void list_cut(List *list, ListNode *from, ListNode *to, size_t range_size) {
    assert(list && ((!from && !to) || (from && to)));

    if (!from) {
        return;
    }

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

    from->prev = LIST_POISON_PREV;
    to->next = LIST_POISON_NEXT;

    list->size -= range_size;
}

void list_paste(List *list, ListNode *left, ListNode *from, ListNode *to, ListNode *right, size_t range_size) {
    assert(list && ((!from && !to) || (from && to)));

    if (!from) {
        return;
    }

    if (left) {
        left->next = from;
    } else {
        list->head = from;
    }
    from->prev = left;

    if (right) {
        right->prev = to;
    } else {
        list->tail = to;
    }
    to->next = right;

    list->size += range_size;
}

void list_sort(List *list, int (*compare)(const ListNode *a, const ListNode *b)) {
    ListNode *head, *tail, *left, *right, *next;
    size_t list_size, num_merges, left_size, right_size;

    assert(list && compare);

    if (list->size < 2) {
        return;
    }

    list_size = 1;
    head = list->head;
    do {
        num_merges = 0;
        left = head;
        head = NULL;
        tail = NULL;
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
                if (left_size == 0) {
                    next = right;
                    right = right->next;
                    --right_size;
                } else if (right_size == 0 || !right) {
                    next = left;
                    left = left->next;
                    --left_size;
                } else if (compare(left, right) <= 0) {
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
