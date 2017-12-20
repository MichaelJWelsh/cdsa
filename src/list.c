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

#include <stddef.h>

#include "list.h"

ListFuncStat list_initialize(List *list) {
    if (!list) {
        return LIST_FUNC_STAT_ERROR;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return LIST_FUNC_STAT_OK;
}

ListNode* list_front(const List *list) {
    return list ? list->head : NULL;
}

ListNode* list_back(const List *list) {
    return list ? list->tail : NULL;
}

ListNode* list_prev(const ListNode *node) {
    return node ? node->prev : NULL;
}

ListNode* list_next(const ListNode *node) {
    return node ? node->next : NULL;
}

size_t list_size(const List *list) {
    return list ? list->size : 0;
}

int list_empty(const List *list) {
    return list ? list->size == 0 : 1;
}

ListFuncStat list_index_of(const List *list, const ListNode *node, size_t *out) {
    if (!list || !node || !out) {
        return LIST_FUNC_STAT_ERROR;
    }

    if (list->tail == node) {
        *out = list->size - 1;
        return LIST_FUNC_STAT_OK;
    } else {
        ListNode *n;
        size_t i = 0;
        list_for_each(n, list) {
            if (n == node) {
                *out = i;
                return LIST_FUNC_STAT_OK;
            }
            ++i;
        }
    }

    return LIST_FUNC_STAT_ERROR;
}

ListFuncStat list_at(const List *list, size_t index, ListNode **out) {
    ListNode *n;
    size_t i;

    if (!list || index >= list->size || !out) {
        return LIST_FUNC_STAT_ERROR;
    }

    if (index < list->size / 2) {
        i = 0;
        list_for_each(n, list) {
            if (i == index) {
                *out = n;
                return LIST_FUNC_STAT_OK;
            }
            ++i;
        }
    } else {
        i = list->size - 1;
        list_for_each_reverse(n, list) {
            if (i == index) {
                *out = n;
                return LIST_FUNC_STAT_OK;
            }
            --i;
        }
    }

    return LIST_FUNC_STAT_ERROR;
}

ListFuncStat list_insert_left(List *list, ListNode *new_node, ListNode *position) {
    if (!list || !new_node) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_paste(list, position ? position->prev : NULL, new_node, new_node, position, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_insert_right(List *list, ListNode *new_node, ListNode *position) {
    if (!list || !new_node) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_paste(list, position, new_node, new_node, position ? position->next : NULL, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_insert_front(List *list, ListNode *new_node) {
    if (!list || !new_node) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_paste(list, NULL, new_node, new_node, list->head, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_insert_back(List *list, ListNode *new_node) {
    if (!list || !new_node) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_paste(list, list->tail, new_node, new_node, NULL, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_splice_left(List *list, List *src_list, ListNode *position) {
    ListNode *from, *to;
    size_t range_size;

    if (!list || !src_list) {
        return LIST_FUNC_STAT_ERROR;
    }

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, position ? position->prev : NULL, from, to, position, range_size);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_splice_right(List *list, List *src_list, ListNode *position) {
    ListNode *from, *to;
    size_t range_size;

    if (!list || !src_list) {
        return LIST_FUNC_STAT_ERROR;
    }

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, position, from, to, position ? position->next : NULL, range_size);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_splice_front(List *list, List *src_list) {
    ListNode *from, *to;
    size_t range_size;

    if (!list || !src_list) {
        return LIST_FUNC_STAT_ERROR;
    }

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, NULL, from, to, list->head, range_size);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_splice_back(List *list, List *src_list) {
    ListNode *from, *to;
    size_t range_size;

    if (!list || !src_list) {
        return LIST_FUNC_STAT_ERROR;
    }

    from = src_list->head;
    to = src_list->tail;
    range_size = src_list->size;

    list_cut(src_list, from, to, range_size);
    list_paste(list, list->tail, from, to, NULL, range_size);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_remove(List *list, ListNode *node) {
    if (!list) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_cut(list, node, node, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_remove_front(List *list) {
    if (!list) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_cut(list, list->head, list->head, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_remove_back(List *list) {
    if (!list) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_cut(list, list->tail, list->tail, 1);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_remove_all(List *list) {
    if (!list) {
        return LIST_FUNC_STAT_ERROR;
    }

    list_cut(list, list->head, list->tail, list->size);

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_cut(List *list, ListNode *from, ListNode *to, size_t range_size) {
    if (!list || (from && !to) || (!from && to)) {
        return LIST_FUNC_STAT_ERROR;
    }

    if (!from) {
        return LIST_FUNC_STAT_OK;
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

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_paste(List *list, ListNode *left, ListNode *from, ListNode *to, ListNode *right, size_t range_size) {
    if (!list || (from && !to) || (!from && to)) {
        return LIST_FUNC_STAT_ERROR;
    }

    if (!from) {
        return LIST_FUNC_STAT_OK;
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

    return LIST_FUNC_STAT_OK;
}

ListFuncStat list_sort(List *list, int (*compare)(const ListNode *a, const ListNode *b)) {
    ListNode *head, *tail, *left, *right, *next;
    size_t list_size, num_merges, left_size, right_size;

    if (!list || !compare) {
        return LIST_FUNC_STAT_ERROR;
    }

    if (list->size < 2) {
        return LIST_FUNC_STAT_OK;
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

    return LIST_FUNC_STAT_OK;
}
