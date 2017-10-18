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
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "alloc.h"

/*
 * A very basic singly linked list implementation used internally by the alloc testing module. This data
 * structure keeps track of all memory addresses allocated/freed.
 */
typedef struct LinkedList__ {
    void *address;
    struct LinkedList__ *next;
} LinkedList;

/*
 * This global variable with internal linkage is used to keep track of all memory addresses allocated/freed by
 * the test functions in this translation unit.
 */
static LinkedList *head = NULL;

/*
 * Completely clears the linked list of all nodes. Everything allocated by the test functions in this
 * translation unit is freed. Every address freed is printed to stderr.
 */
static void linkedlist_clear(bool print_addresses) {
    LinkedList *current = head;
    while (current != NULL) {
        if (print_addresses) {
            fprintf(stderr, "Error: address would have gone unfreed: %p\n", current->address);
        }
        free(current->address);
        LinkedList *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

/*
 * Pushes a new node containing address into the front of the linked list.
 */
static void* linkedlist_push_front(void *address) {
    LinkedList *new_node = (LinkedList*) malloc(sizeof(LinkedList));
    if (new_node == NULL) {
        free(address);
        linkedlist_clear(false);
        fprintf(stderr, "Out of memory! Unable to continue...");
        abort();
    }

    new_node->address = address;
    new_node->next = head;
    head = new_node;

    return address;
}

/*
 * Removes the node containing address and frees the data at the address. Aborts program if address is not in
 * the linked list chain.
 */
static void linkedlist_remove(void *address) {
    if (head != NULL && head->address == address) {
        free(address);
        LinkedList *next = head->next;
        free(head);
        head = next;
        return;
    } else {
        LinkedList *prev = head;
        LinkedList *current = head->next;
        while (current != NULL) {
            if (current->address == address) {
                prev->next = current->next;
                free(address);
                free(current);
                return;
            }
            prev = current;
            current = current->next;
        }
    }

    free(address);
    linkedlist_clear(false);
    fprintf(stderr, "Random address given. It was not allocated by test functions!");
    abort();
}

/*
 * Tests if all resources allocated has been deallocated. If there are still resources that need to be
 * deallocated, the memory address of those resources is printed to stderr and then they are properly freed.
 * This function then aborts the program.
 */
static void test_assert_all_resources_free(void) {
    if (head != NULL) {
        linkedlist_clear(true);
        abort();
    }
}

void run_tests(TestFunc *test_funcs, size_t nitems) {
    for (size_t i = 0; i < nitems; ++i) {
        test_funcs[i]();
        test_assert_all_resources_free();
    }
}

void* test_bad_malloc(size_t size) {
    return NULL;
}

void* test_bad_calloc(size_t nitems, size_t size) {
    return NULL;
}

void* test_bad_realloc(void *ptr, size_t size) {
    return NULL;
}

void* test_malloc(size_t size) {
    void *address = malloc(size);
    if (address == NULL) {
        linkedlist_clear(false);
        fprintf(stderr, "Out of memory! Unable to continue...");
        abort();
    }
    memset(address, '?', size);

    return linkedlist_push_front(address);
}

void* test_calloc(size_t nitems, size_t size) {
    void *address = calloc(nitems, size);
    if (address == NULL) {
        linkedlist_clear(false);
        fprintf(stderr, "Out of memory! Unable to continue...");
        abort();
    }

    return linkedlist_push_front(address);
}

void* test_realloc(void *ptr, size_t size) {
    void *address = realloc(ptr, size);
    if (address == NULL) {
        linkedlist_clear(false);
        fprintf(stderr, "Out of memory! Unable to continue...");
        abort();
    }

    if (ptr != NULL) {
        linkedlist_remove(ptr);
    }

    return linkedlist_push_front(address);
}

void test_free(void *ptr) {
    linkedlist_remove(ptr);
}
