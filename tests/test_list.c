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
#include <assert.h>

#include "alloc.h"

// Test header guard.
#include "../src/list.h"
#include "../src/list.h"

int static_var1[] = {1}, static_var2[] = {2}, static_var3[] = {3}, static_var4[] = {4}, static_var5[] = {5}, static_var6[] = {6};

void test_list_create_and_destroy(void) {
    // Test bad allocation.
    List *list = list_create(test_bad_malloc, test_free, NULL);
    assert(list == NULL);
    list_destroy(list);
    list = NULL;

    // Test allocation/deallocation on an empty list WITH ownership.
    list = list_create(test_malloc, test_free, test_free);
    assert(list != NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->list_malloc == test_malloc);
    assert(list->list_free == test_free);
    assert(list->data_free == test_free);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test allocation/deallocation on a non-empty list WITH ownership. Assumes list_push_back works as
    // intended for the sake of adding items to the list.
    list = list_create(test_malloc, test_free, test_free);
    int *dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    int *dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    int *dynamic_var3 = (int*) test_malloc(sizeof(int));
    *dynamic_var3 = 3;
    int *dynamic_var4 = (int*) test_malloc(sizeof(int));
    *dynamic_var4 = 4;
    list_push_back(list, dynamic_var1);
    list_push_back(list, dynamic_var2);
    list_push_back(list, dynamic_var3);
    list_push_back(list, dynamic_var4);
    list_destroy(list);
    list = NULL;

    // Test allocation/deallocation on an empty list WITHOUT ownership.
    list = list_create(test_malloc, test_free, NULL);
    assert(list != NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->list_malloc == test_malloc);
    assert(list->list_free == test_free);
    assert(list->data_free == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test allocation/deallocation on a non-empty list WITHOUT ownership. Assumes list_push_back works as
    // intended for the sake of adding items to the list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_destroy(list);
    list = NULL;
}

void test_list_to_array(void) {
    // Test bad allocation.
    List *list = list_create(test_malloc, test_free, test_free);
    list->list_malloc = test_bad_malloc;
    assert(list_to_array(list) == NULL);
    list_destroy(list);
    list = NULL;

    // Test on empty list.
    list = list_create(test_malloc, test_free, test_free);
    void **array = list_to_array(list);
    assert(array != NULL);
    test_free(array);
    list_destroy(list);
    list = NULL;
    array = NULL;

    // Test on non-empty list. Assumes list_push_back works as intended for the sake of adding items to the
    // list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    array = list_to_array(list);
    assert((int*) array[0] == static_var1);
    assert((int*) array[1] == static_var2);
    assert((int*) array[2] == static_var3);
    assert((int*) array[3] == static_var4);
    test_free(array);
    list_destroy(list);
    list = NULL;
    array = NULL;
}

void test_list_index_of(void) {
    // Assume list_push_back works as intended.
    List *list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    assert(list_index_of(list, list->head) == 0);
    assert(list_index_of(list, list->head->next) == 1);
    assert(list_index_of(list, list->tail->prev) == 2);
    assert(list_index_of(list, list->tail) == 3);
    list_destroy(list);
    list = NULL;
}

void test_list_at(void) {
    // Assume list_push_back works as intended.
    List *list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    assert(list_at(list, 0) == list->head);
    assert(list_at(list, 1) == list->head->next);
    assert(list_at(list, 2) == list->tail->prev);
    assert(list_at(list, 3) == list->tail);
    list_destroy(list);
    list = NULL;
}

void test_list_insert_left(void) {
    // Test insertion with bad malloc. Assume list_push_back works as intended.
    List *list = list_create(test_malloc, test_free, NULL);
    ListNode *node = list_push_back(list, static_var1);
    list->list_malloc = test_bad_malloc;
    assert(list_insert_left(list, static_var2, node) == NULL);
    list_destroy(list);
    list = NULL;
    node = NULL;

    // Test insertion at head of list. Assume list_push_back works as intended.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *tail = list_push_back(list, static_var2);
    ListNode *head = list_insert_left(list, static_var1, tail);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var2);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    tail = NULL;
    head = NULL;

    // Test insertion elsewhere. Assume list_push_back works as intended.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var3);
    ListNode *middle = list_insert_left(list, static_var2, tail);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 3);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;
    middle = NULL;
}

void test_list_insert_right(void) {
    // Test insertion with bad malloc. Assume list_push_back works as intended.
    List *list = list_create(test_malloc, test_free, NULL);
    ListNode *node = list_push_back(list, static_var1);
    list->list_malloc = test_bad_malloc;
    assert(list_insert_right(list, static_var2, node) == NULL);
    list_destroy(list);
    list = NULL;
    node = NULL;

    // Test insertion at tail of list. Assume list_push_back works as intended.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list, static_var1);
    ListNode *tail = list_insert_right(list, static_var2, head);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var2);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;

    // Test insertion elsewhere. Assume list_push_back works as intended.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var3);
    ListNode *middle = list_insert_right(list, static_var2, head);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 3);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;
    middle = NULL;
}

void test_list_push_front(void) {
    // Test insertion with bad malloc.
    List *list = list_create(test_malloc, test_free, NULL);
    list->list_malloc = test_bad_malloc;
    assert(list_push_front(list, static_var1) == NULL);
    list_destroy(list);
    list = NULL;

    // Test insertion on empty list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *node = list_push_front(list, static_var1);
    assert(list->head == node);
    assert(list->tail == node);
    assert(list->size == 1);
    assert(node->data == static_var1);
    assert(node->prev == NULL);
    assert(node->next == NULL);
    list_destroy(list);
    list = NULL;
    node = NULL;

    // Test insertion on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *tail = list_push_front(list, static_var2);
    ListNode *head = list_push_front(list, static_var1);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var2);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    tail = NULL;
    head = NULL;
}

void test_list_push_back(void) {
    // Test insertion with bad malloc.
    List *list = list_create(test_malloc, test_free, NULL);
    list->list_malloc = test_bad_malloc;
    assert(list_push_back(list, static_var1) == NULL);
    list_destroy(list);
    list = NULL;

    // Test insertion on empty list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *node = list_push_back(list, static_var1);
    assert(list->head == node);
    assert(list->tail == node);
    assert(list->size == 1);
    assert(node->data == static_var1);
    assert(node->prev == NULL);
    assert(node->next == NULL);
    list_destroy(list);
    list = NULL;
    node = NULL;

    // Test insertion on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list, static_var1);
    ListNode *tail = list_push_back(list, static_var2);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var2);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;
}

void test_list_clear(void) {
    // Test on empty list without ownership.
    List *list = list_create(test_malloc, test_free, NULL);
    list_clear(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test on non-empty list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_clear(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test on empty list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    list_clear(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test on non-empty list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    int *dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    int *dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    int *dynamic_var3 = (int*) test_malloc(sizeof(int));
    *dynamic_var3 = 3;
    int *dynamic_var4 = (int*) test_malloc(sizeof(int));
    *dynamic_var4 = 4;
    list_push_back(list, dynamic_var1);
    list_push_back(list, dynamic_var2);
    list_push_back(list, dynamic_var3);
    list_push_back(list, dynamic_var4);
    list_clear(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    dynamic_var2 = NULL;
    dynamic_var3 = NULL;
    dynamic_var4 = NULL;
}

void test_list_remove(void) {
    // Test deletion with NULL as input.
    List *list = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list, static_var1);
    ListNode *tail = list_push_back(list, static_var2);
    assert(list_remove(list, NULL) == NULL);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var2);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;

    // Test deletion on single-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *node = list_push_back(list, static_var1);
    assert(list_remove(list, node) == static_var1);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    node = NULL;

    // Test deletion on single-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    int *dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    node = list_push_back(list, dynamic_var1);
    assert(list_remove(list, node) == NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    node = NULL;

    // Test deletion on head of multi-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var2);
    assert(list_remove(list, head) == static_var1);
    assert(list->head == tail);
    assert(list->tail == tail);
    assert(list->size == 1);
    assert(tail->data == static_var2);
    assert(tail->prev == NULL);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;

    // Test deletion on head of multi-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    int *dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    head = list_push_back(list, dynamic_var1);
    tail = list_push_back(list, dynamic_var2);
    assert(list_remove(list, head) == NULL);
    assert(list->head == tail);
    assert(list->tail == tail);
    assert(list->size == 1);
    assert(tail->data == dynamic_var2);
    assert(tail->prev == NULL);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    dynamic_var2 = NULL;
    head = NULL;
    tail = NULL;

    // Test deletion on tail of multi-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var2);
    assert(list_remove(list, tail) == static_var2);
    assert(list->head == head);
    assert(list->tail == head);
    assert(list->size == 1);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;

    // Test deletion on tail of multi-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    head = list_push_back(list, dynamic_var1);
    tail = list_push_back(list, dynamic_var2);
    assert(list_remove(list, tail) == NULL);
    assert(list->head == head);
    assert(list->tail == head);
    assert(list->size == 1);
    assert(head->data == dynamic_var1);
    assert(head->prev == NULL);
    assert(head->next == NULL);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    dynamic_var2 = NULL;
    head = NULL;
    tail = NULL;

    // Test deletion on middle of multi-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    ListNode *middle = list_push_back(list, static_var2);
    tail = list_push_back(list, static_var3);
    assert(list_remove(list, middle) == static_var2);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test deletion on middle of multi-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    int *dynamic_var3 = (int*) test_malloc(sizeof(int));
    *dynamic_var3 = 3;
    head = list_push_back(list, dynamic_var1);
    middle = list_push_back(list, dynamic_var2);
    tail = list_push_back(list, dynamic_var3);
    assert(list_remove(list, middle) == NULL);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == dynamic_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == dynamic_var3);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    dynamic_var2 = NULL;
    dynamic_var3 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;
}

void test_list_pop_front(void) {
    // Test deletion on empty list.
    List *list = list_create(test_malloc, test_free, NULL);
    assert(list_pop_front(list) == NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test deletion on single-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    assert(list_pop_front(list) == static_var1);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test deletion on single-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    int *dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    list_push_back(list, dynamic_var1);
    assert(list_pop_front(list) == NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;

    // Test deletion on multi-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    ListNode *tail = list_push_back(list, static_var2);
    assert(list_pop_front(list) == static_var1);
    assert(list->head == tail);
    assert(list->tail == tail);
    assert(list->size == 1);
    assert(tail->data == static_var2);
    assert(tail->prev == NULL);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    tail = NULL;

    // Test deletion on multi-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    int *dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    list_push_back(list, dynamic_var1);
    tail = list_push_back(list, dynamic_var2);
    assert(list_pop_front(list) == NULL);
    assert(list->head == tail);
    assert(list->tail == tail);
    assert(list->size == 1);
    assert(tail->data == dynamic_var2);
    assert(tail->prev == NULL);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    dynamic_var2 = NULL;
    tail = NULL;
}

void test_list_pop_back(void) {
    // Test deletion on empty list.
    List *list = list_create(test_malloc, test_free, NULL);
    assert(list_pop_front(list) == NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test deletion on single-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    assert(list_pop_back(list) == static_var1);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test deletion on single-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    int *dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    list_push_back(list, dynamic_var1);
    assert(list_pop_back(list) == NULL);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;

    // Test deletion on multi-node list without ownership.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    assert(list_pop_back(list) == static_var2);
    assert(list->head == head);
    assert(list->tail == head);
    assert(list->size == 1);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;

    // Test deletion on multi-node list with ownership.
    list = list_create(test_malloc, test_free, test_free);
    dynamic_var1 = (int*) test_malloc(sizeof(int));
    *dynamic_var1 = 1;
    int *dynamic_var2 = (int*) test_malloc(sizeof(int));
    *dynamic_var2 = 2;
    head = list_push_back(list, dynamic_var1);
    list_push_back(list, dynamic_var2);
    assert(list_pop_back(list) == NULL);
    assert(list->head == head);
    assert(list->tail == head);
    assert(list->size == 1);
    assert(head->data == dynamic_var1);
    assert(head->prev == NULL);
    assert(head->next == NULL);
    list_destroy(list);
    list = NULL;
    dynamic_var1 = NULL;
    dynamic_var2 = NULL;
    head = NULL;
}

// Sorts list in ascending order.
static int test_list_sort_compare(const void *d1, const void* d2) {
    return *(int*) d1 - *(int*) d2;
}

void test_list_sort(void) {
    // Test sort on empty list.
    List *list = list_create(test_malloc, test_free, NULL);
    list_sort(list, test_list_sort_compare);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;

    // Test sort on single-node list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *node = list_push_back(list, static_var1);
    list_sort(list, test_list_sort_compare);
    assert(list->head == node);
    assert(list->tail == node);
    assert(list->size == 1);
    assert(node->data == static_var1);
    assert(node->prev == NULL);
    assert(node->next == NULL);
    list_destroy(list);
    list = NULL;
    node = NULL;

    // Test sort on double-node list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *tail = list_push_back(list, static_var2);
    ListNode *head = list_push_back(list, static_var1);
    list_sort(list, test_list_sort_compare);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 2);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == tail);
    assert(tail->data == static_var2);
    assert(tail->prev == head);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    tail = NULL;
    head = NULL;

    // Test sort on even-sized multi-node list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var4);
    ListNode *third = list_push_back(list, static_var3);
    ListNode *second = list_push_back(list, static_var2);
    list_sort(list, test_list_sort_compare);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 4);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == tail);
    assert(tail->data == static_var4);
    assert(tail->prev == third);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;
    third = NULL;
    second = NULL;

    // Test sort on odd-sized multi-node list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var5);
    third = list_push_back(list, static_var3);
    second = list_push_back(list, static_var2);
    ListNode *fourth = list_push_back(list, static_var4);
    list_sort(list, test_list_sort_compare);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    tail = NULL;
    third = NULL;
    second = NULL;
    fourth = NULL;

    // Test sort on already sorted list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    list_sort(list, test_list_sort_compare);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;

    // Test sort stability.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var1);
    third = list_push_back(list, static_var1);
    fourth = list_push_back(list, static_var1);
    tail = list_push_back(list, static_var1);
    list_sort(list, test_list_sort_compare);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var1);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var1);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var1);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var1);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
}

void test_list_splice_left(void) {
    // Test splicing non-empty list at front with empty list.
    List *list1 = list_create(test_malloc, test_free, NULL);
    List *list2 = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list1, static_var1);
    ListNode *middle = list_push_back(list1, static_var2);
    ListNode *tail = list_push_back(list1, static_var3);
    list_splice_left(list1, head, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list at middle with empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list1, static_var1);
    middle = list_push_back(list1, static_var2);
    tail = list_push_back(list1, static_var3);
    list_splice_left(list1, middle, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list at front with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    ListNode *first = list_push_back(list2, static_var1);
    ListNode *second = list_push_back(list2, static_var2);
    ListNode *third = list_push_back(list2, static_var3);
    ListNode *fourth = list_push_back(list1, static_var4);
    ListNode *fifth = list_push_back(list1, static_var5);
    ListNode *sixth = list_push_back(list1, static_var6);
    list_splice_left(list1, fourth, list2, first, third, 3);
    assert(list1->head == first);
    assert(list1->tail == sixth);
    assert(list1->size == 6);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at middle with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_left(list1, fifth, list2, first, third, 3);
    assert(list1->head == fourth);
    assert(list1->tail == sixth);
    assert(list1->size == 6);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(first->data == static_var1);
    assert(first->prev == fourth);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fifth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == first);
    assert(fifth->data == static_var5);
    assert(fifth->prev == third);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at front with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_left(list1, fourth, list2, first, second, 2);
    assert(list1->head == first);
    assert(list1->tail == sixth);
    assert(list1->size == 5);
    assert(list2->head == third);
    assert(list2->tail == third);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == fourth);
    assert(third->data == static_var3);
    assert(third->prev == NULL);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == second);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at middle with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_left(list1, fifth, list2, second, third, 2);
    assert(list1->head == fourth);
    assert(list1->tail == sixth);
    assert(list1->size == 5);
    assert(list2->head == first);
    assert(list2->tail == first);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == NULL);
    assert(second->data == static_var2);
    assert(second->prev == fourth);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fifth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == second);
    assert(fifth->data == static_var5);
    assert(fifth->prev == third);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at front with part of non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_left(list1, first, list1, second, third, 2);
    assert(list1->head == second);
    assert(list1->tail == first);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == third);
    assert(first->next == NULL);
    assert(second->data == static_var2);
    assert(second->prev == NULL);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == first);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;

    // Test splicing non-empty list at middle with part of non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_left(list1, second, list1, third, third, 1);
    assert(list1->head == first);
    assert(list1->tail == second);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == third);
    assert(second->data == static_var2);
    assert(second->prev == third);
    assert(second->next == NULL);
    assert(third->data == static_var3);
    assert(third->prev == first);
    assert(third->next == second);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
}

void test_list_splice_right(void) {
    // Test splicing non-empty list at middle with empty list.
    List *list1 = list_create(test_malloc, test_free, NULL);
    List *list2 = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list1, static_var1);
    ListNode *middle = list_push_back(list1, static_var2);
    ListNode *tail = list_push_back(list1, static_var3);
    list_splice_right(list1, head, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list at back with empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list1, static_var1);
    middle = list_push_back(list1, static_var2);
    tail = list_push_back(list1, static_var3);
    list_splice_right(list1, tail, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list at middle with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list1, static_var1);
    middle = list_push_back(list1, static_var2);
    tail = list_push_back(list1, static_var3);
    list_splice_right(list1, tail, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list at back with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    ListNode *first = list_push_back(list2, static_var1);
    ListNode *second = list_push_back(list2, static_var2);
    ListNode *third = list_push_back(list2, static_var3);
    ListNode *fourth = list_push_back(list1, static_var4);
    ListNode *fifth = list_push_back(list1, static_var5);
    ListNode *sixth = list_push_back(list1, static_var6);
    list_splice_right(list1, sixth, list2, first, third, 3);
    assert(list1->head == fourth);
    assert(list1->tail == third);
    assert(list1->size == 6);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(first->data == static_var1);
    assert(first->prev == sixth);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == first);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at middle with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_right(list1, fifth, list2, first, second, 2);
    assert(list1->head == fourth);
    assert(list1->tail == sixth);
    assert(list1->size == 5);
    assert(list2->head == third);
    assert(list2->tail == third);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == fifth);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == sixth);
    assert(third->data == static_var3);
    assert(third->prev == NULL);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == first);
    assert(sixth->data == static_var6);
    assert(sixth->prev == second);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at back with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_right(list1, sixth, list2, second, third, 2);
    assert(list1->head == fourth);
    assert(list1->tail == third);
    assert(list1->size == 5);
    assert(list2->head == first);
    assert(list2->tail == first);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == NULL);
    assert(second->data == static_var2);
    assert(second->prev == sixth);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == second);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list at middle with part of non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_right(list1, first, list1, third, third, 1);
    assert(list1->head == first);
    assert(list1->tail == second);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == third);
    assert(second->data == static_var2);
    assert(second->prev == third);
    assert(second->next == NULL);
    assert(third->data == static_var3);
    assert(third->prev == first);
    assert(third->next == second);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;

    // Test splicing non-empty list at back with part of non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_right(list1, third, list1, first, second, 2);
    assert(list1->head == third);
    assert(list1->tail == second);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == third);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == NULL);
    assert(third->data == static_var3);
    assert(third->prev == NULL);
    assert(third->next == first);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
}

void test_list_splice_front(void) {
    // Test splicing empty list with empty list.
    List *list1 = list_create(test_malloc, test_free, NULL);
    List *list2 = list_create(test_malloc, test_free, NULL);
    list_splice_front(list1, list2, NULL, NULL, 0);
    assert(list1->head == NULL);
    assert(list1->tail == NULL);
    assert(list1->size == 0);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;

    // Test splicing empty list with empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    list_splice_front(list1, list1, NULL, NULL, 0);
    assert(list1->head == NULL);
    assert(list1->tail == NULL);
    assert(list1->size == 0);
    list_destroy(list1);
    list1 = NULL;

    // Test splicing empty list with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list2, static_var1);
    ListNode *middle = list_push_back(list2, static_var2);
    ListNode *tail = list_push_back(list2, static_var3);
    list_splice_front(list1, list2, head, tail, 3);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing empty list with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    ListNode *first = list_push_back(list2, static_var1);
    ListNode *second = list_push_back(list2, static_var2);
    ListNode *third = list_push_back(list2, static_var3);
    list_splice_front(list1, list2, second, third, 2);
    assert(list1->head == second);
    assert(list1->tail == third);
    assert(list1->size == 2);
    assert(list2->head == first);
    assert(list2->tail == first);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == NULL);
    assert(second->data == static_var2);
    assert(second->prev == NULL);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;

    // Test splicing non-empty list with empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list1, static_var1);
    middle = list_push_back(list1, static_var2);
    tail = list_push_back(list1, static_var3);
    list_splice_front(list1, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    ListNode *fourth = list_push_back(list1, static_var4);
    ListNode *fifth = list_push_back(list1, static_var5);
    ListNode *sixth = list_push_back(list1, static_var6);
    list_splice_front(list1, list2, first, third, 3);
    assert(list1->head == first);
    assert(list1->tail == sixth);
    assert(list1->size == 6);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_front(list1, list2, first, second, 2);
    assert(list1->head == first);
    assert(list1->tail == sixth);
    assert(list1->size == 5);
    assert(list2->head == third);
    assert(list2->tail == third);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == fourth);
    assert(third->data == static_var3);
    assert(third->prev == NULL);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == second);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list with entire non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_front(list1, list1, first, third, 3);
    assert(list1->head == first);
    assert(list1->tail == third);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;

    // Test splicing non-empty list with part of non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_front(list1, list1, second, second, 1);
    assert(list1->head == second);
    assert(list1->tail == third);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == second);
    assert(first->next == third);
    assert(second->data == static_var2);
    assert(second->prev == NULL);
    assert(second->next == first);
    assert(third->data == static_var3);
    assert(third->prev == first);
    assert(third->next == NULL);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
}

void test_list_splice_back(void) {
    // Test splicing empty list with empty list.
    List *list1 = list_create(test_malloc, test_free, NULL);
    List *list2 = list_create(test_malloc, test_free, NULL);
    list_splice_back(list1, list2, NULL, NULL, 0);
    assert(list1->head == NULL);
    assert(list1->tail == NULL);
    assert(list1->size == 0);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;

    // Test splicing empty list with empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    list_splice_back(list1, list1, NULL, NULL, 0);
    assert(list1->head == NULL);
    assert(list1->tail == NULL);
    assert(list1->size == 0);
    list_destroy(list1);
    list1 = NULL;

    // Test splicing empty list with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list2, static_var1);
    ListNode *middle = list_push_back(list2, static_var2);
    ListNode *tail = list_push_back(list2, static_var3);
    list_splice_back(list1, list2, head, tail, 3);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing empty list with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    ListNode *first = list_push_back(list2, static_var1);
    ListNode *second = list_push_back(list2, static_var2);
    ListNode *third = list_push_back(list2, static_var3);
    list_splice_back(list1, list2, second, third, 2);
    assert(list1->head == second);
    assert(list1->tail == third);
    assert(list1->size == 2);
    assert(list2->head == first);
    assert(list2->tail == first);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == NULL);
    assert(second->data == static_var2);
    assert(second->prev == NULL);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;

    // Test splicing non-empty list with empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list1, static_var1);
    middle = list_push_back(list1, static_var2);
    tail = list_push_back(list1, static_var3);
    list_splice_back(list1, list2, NULL, NULL, 0);
    assert(list1->head == head);
    assert(list1->tail == tail);
    assert(list1->size == 3);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == middle);
    assert(middle->data == static_var2);
    assert(middle->prev == head);
    assert(middle->next == tail);
    assert(tail->data == static_var3);
    assert(tail->prev == middle);
    assert(tail->next == NULL);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    head = NULL;
    middle = NULL;
    tail = NULL;

    // Test splicing non-empty list with entire non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    ListNode *fourth = list_push_back(list1, static_var4);
    ListNode *fifth = list_push_back(list1, static_var5);
    ListNode *sixth = list_push_back(list1, static_var6);
    list_splice_back(list1, list2, first, third, 3);
    assert(list1->head == fourth);
    assert(list1->tail == third);
    assert(list1->size == 6);
    assert(list2->head == NULL);
    assert(list2->tail == NULL);
    assert(list2->size == 0);
    assert(first->data == static_var1);
    assert(first->prev == sixth);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == first);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list with part of non-empty list.
    list1 = list_create(test_malloc, test_free, NULL);
    list2 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list2, static_var1);
    second = list_push_back(list2, static_var2);
    third = list_push_back(list2, static_var3);
    fourth = list_push_back(list1, static_var4);
    fifth = list_push_back(list1, static_var5);
    sixth = list_push_back(list1, static_var6);
    list_splice_back(list1, list2, first, second, 2);
    assert(list1->head == fourth);
    assert(list1->tail == second);
    assert(list1->size == 5);
    assert(list2->head == third);
    assert(list2->tail == third);
    assert(list2->size == 1);
    assert(first->data == static_var1);
    assert(first->prev == sixth);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == NULL);
    assert(third->data == static_var3);
    assert(third->prev == NULL);
    assert(third->next == NULL);
    assert(fourth->data == static_var4);
    assert(fourth->prev == NULL);
    assert(fourth->next == fifth);
    assert(fifth->data == static_var5);
    assert(fifth->prev == fourth);
    assert(fifth->next == sixth);
    assert(sixth->data == static_var6);
    assert(sixth->prev == fifth);
    assert(sixth->next == first);
    list_destroy(list1);
    list_destroy(list2);
    list1 = NULL;
    list2 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    fifth = NULL;
    sixth = NULL;

    // Test splicing non-empty list with entire non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_back(list1, list1, first, third, 3);
    assert(list1->head == first);
    assert(list1->tail == third);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == second);
    assert(second->data == static_var2);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == NULL);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;

    // Test splicing non-empty list with part of non-empty list, where both lists are the same list.
    list1 = list_create(test_malloc, test_free, NULL);
    first = list_push_back(list1, static_var1);
    second = list_push_back(list1, static_var2);
    third = list_push_back(list1, static_var3);
    list_splice_back(list1, list1, second, second, 1);
    assert(list1->head == first);
    assert(list1->tail == second);
    assert(list1->size == 3);
    assert(first->data == static_var1);
    assert(first->prev == NULL);
    assert(first->next == third);
    assert(second->data == static_var2);
    assert(second->prev == third);
    assert(second->next == NULL);
    assert(third->data == static_var3);
    assert(third->prev == first);
    assert(third->next == second);
    list_destroy(list1);
    list1 = NULL;
    first = NULL;
    second = NULL;
    third = NULL;
}

void test_list_traversal_macros(void) {
    void *static_var_ptrs[] = {static_var1, static_var2, static_var3, static_var4, static_var5};

    // Test list_for_each on empty list.
    List *list = list_create(test_malloc, test_free, NULL);
    size_t i = 0;
    list_for_each(n, list) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_reverse on empty list.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_reverse(n, list) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_safe on empty list.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_safe(n, list) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_safe_reverse on empty list.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_safe_reverse(n, list) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_continue on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_continue(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_continue_reverse on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_continue_reverse(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_safe_continue on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_safe_continue(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_safe_continue_reverse on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_safe_continue_reverse(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_from on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_from(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_from_reverse on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_from_reverse(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_safe_from on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_safe_from(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each_safe_from_reverse on NULL input.
    list = list_create(test_malloc, test_free, NULL);
    i = 0;
    list_for_each_safe_from_reverse(n, NULL) {
        ++i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test list_for_each on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    ListNode *head = list_push_back(list, static_var1);
    ListNode *second = list_push_back(list, static_var2);
    ListNode *third = list_push_back(list, static_var3);
    ListNode *fourth = list_push_back(list, static_var4);
    ListNode *tail = list_push_back(list, static_var5);
    i = 0;
    list_for_each(n, list) {
        assert(n->data == static_var_ptrs[i]);
        ++i;
    }
    assert(i == list->size);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_reverse on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size;
    list_for_each_reverse(n, list) {
        assert(n->data == static_var_ptrs[i - 1]);
        --i;
    }
    assert(i == 0);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_safe on non-empty list, and simultaneously test reassignment.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = 0;
    list_for_each_safe(n, list) {
        assert(n->data == static_var_ptrs[i]);
        n = NULL;
        ++i;
    }
    assert(i == list->size);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_safe_reverse on non-empty list, and simultaneously test reassignment.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_reverse(n, list) {
        assert(n->data == static_var_ptrs[i - 1]);
        n = NULL;
        --i;
    }
    assert(i == 0);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_continue on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = 1;
    list_for_each_continue(n, list->head) {
        assert(n->data == static_var_ptrs[i]);
        ++i;
    }
    assert(i == list->size);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_continue_reverse on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size - 1;
    list_for_each_continue_reverse(n, list->tail) {
        assert(n->data == static_var_ptrs[i - 1]);
        --i;
    }
    assert(i == 0);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_safe_continue on non-empty list, and simultaneously test reassignment.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = 1;
    list_for_each_safe_continue(n, list->head) {
        assert(n->data == static_var_ptrs[i]);
        n = NULL;
        ++i;
    }
    assert(i == list->size);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_safe_continue_reverse on non-empty list, and simultaneously test reassignment.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size - 1;
    list_for_each_safe_continue_reverse(n, list->tail) {
        assert(n->data == static_var_ptrs[i - 1]);
        n = NULL;
        --i;
    }
    assert(i == 0);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_from on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = 0;
    list_for_each_from(n, list->head) {
        assert(n->data == static_var_ptrs[i]);
        ++i;
    }
    assert(i == list->size);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_from_reverse on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size;
    list_for_each_from_reverse(n, list->tail) {
        assert(n->data == static_var_ptrs[i - 1]);
        --i;
    }
    assert(i == 0);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_safe_from on non-empty list, and simultaneously test reassignment.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = 0;
    list_for_each_safe_from(n, list->head) {
        assert(n->data == static_var_ptrs[i]);
        n = NULL;
        ++i;
    }
    assert(i == list->size);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test list_for_each_safe_from_reverse on non-empty list, and simultaneously test reassignment.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    second = list_push_back(list, static_var2);
    third = list_push_back(list, static_var3);
    fourth = list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_from_reverse(n, list->tail) {
        assert(n->data == static_var_ptrs[i - 1]);
        n = NULL;
        --i;
    }
    assert(i == 0);
    assert(list->head == head);
    assert(list->tail == tail);
    assert(list->size == 5);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == second);
    assert(second->data == static_var2);
    assert(second->prev == head);
    assert(second->next == third);
    assert(third->data == static_var3);
    assert(third->prev == second);
    assert(third->next == fourth);
    assert(fourth->data == static_var4);
    assert(fourth->prev == third);
    assert(fourth->next == tail);
    assert(tail->data == static_var5);
    assert(tail->prev == fourth);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    second = NULL;
    third = NULL;
    fourth = NULL;
    tail = NULL;
    i = 100;

    // Test node removal using list_for_each_safe on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe(n, list) {
        list_remove(list, n);
        --i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test node removal using list_for_each_safe_reverse on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_reverse(n, list) {
        list_remove(list, n);
        --i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test node removal using list_for_each_safe_continue on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    head = list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_continue(n, list->head) {
        list_remove(list, n);
        --i;
    }
    assert(i == 1);
    assert(list->head == head);
    assert(list->tail == head);
    assert(list->size == 1);
    assert(head->data == static_var1);
    assert(head->prev == NULL);
    assert(head->next == NULL);
    list_destroy(list);
    list = NULL;
    head = NULL;
    i = 100;

    // Test node removal using list_for_each_safe_continue_reverse on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    tail = list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_continue_reverse(n, list->tail) {
        list_remove(list, n);
        --i;
    }
    assert(i == 1);
    assert(list->head == tail);
    assert(list->tail == tail);
    assert(list->size == 1);
    assert(tail->data == static_var5);
    assert(tail->prev == NULL);
    assert(tail->next == NULL);
    list_destroy(list);
    list = NULL;
    tail = NULL;
    i = 100;

    // Test node removal using list_for_each_safe_from on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_from(n, list->head) {
        list_remove(list, n);
        --i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;

    // Test node removal using list_for_each_safe_from_reverse on non-empty list.
    list = list_create(test_malloc, test_free, NULL);
    list_push_back(list, static_var1);
    list_push_back(list, static_var2);
    list_push_back(list, static_var3);
    list_push_back(list, static_var4);
    list_push_back(list, static_var5);
    i = list->size;
    list_for_each_safe_from_reverse(n, list->tail) {
        list_remove(list, n);
        --i;
    }
    assert(i == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    list_destroy(list);
    list = NULL;
    i = 100;
}

TestFunc test_funcs[] = {
    test_list_create_and_destroy,
    test_list_to_array,
    test_list_index_of,
    test_list_at,
    test_list_insert_left,
    test_list_insert_right,
    test_list_push_front,
    test_list_push_back,
    test_list_clear,
    test_list_remove,
    test_list_pop_front,
    test_list_pop_back,
    test_list_sort,
    test_list_splice_left,
    test_list_splice_right,
    test_list_splice_front,
    test_list_splice_back,
    test_list_traversal_macros
};

int main(void) {
    assert(sizeof(test_funcs) / sizeof(TestFunc) == 18);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc));
    return 0;
}
