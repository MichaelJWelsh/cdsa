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
 * @file    list.h
 * @brief   Doubly Linked List.
 *
 * Dependencies:
 *      -   C99 standard library
 *
 * Unique Features:
 *      -   Supports accessing nodes by index.
 *      -   Allows you to define what functions to call for memory allocation/deallocation.
 *      -   Can optionally take ownership over stored data and automatically free it when necessary.
 *
 * API:
 *      ====  FUNCTIONS  ====
 *      -   @ref list_create
 *      -   @ref list_destroy
 *      -   @ref list_to_array
 *      -   @ref list_position
 *      -   @ref list_at
 *      -   @ref list_insert
 *      -   @ref list_insert_left
 *      -   @ref list_insert_right
 *      -   @ref list_push_front
 *      -   @ref list_push_back
 *      -   @ref list_clear
 *      -   @ref list_delete
 *      -   @ref list_delete_node
 *      -   @ref list_delete_left
 *      -   @ref list_delete_right
 *      -   @ref list_pop_front
 *      -   @ref list_pop_back
 *      -   @ref list_sort
 *
 *      ====  MACROS  ====
 *      -   @ref list_for_each
 *      -   @ref list_for_each_reverse
 *      -   @ref list_for_each_safe
 *      -   @ref list_for_each_safe_reverse
 *      -   @ref list_for_each_continue
 *      -   @ref list_for_each_continue_reverse
 *      -   @ref list_for_each_safe_continue
 *      -   @ref list_for_each_safe_continue_reverse
 *      -   @ref list_for_each_from
 *      -   @ref list_for_each_from_reverse
 *      -   @ref list_for_each_safe_from
 *      -   @ref list_for_each_safe_from_reverse
 */

#ifndef LIST_H__
#define LIST_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>

/**
 * @brief       Represents a node in a @ref List.
 */
typedef struct ListNode {
    void *data;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

/**
 * @brief       Represents a doubly linked list.
 */
typedef struct List {
    ListNode *head;
    ListNode *tail;
    void* (*list_malloc)(size_t);
    void (*list_free)(void*);
    void (*data_free)(void*);
    size_t size;
} List;

/**
 * @function                    list_create
 * @brief                       Dynamically allocates a new empty @ref List.
 * @param list_malloc           The malloc function used to dynamically allocate all memory used by the
 *                              @ref List, including the creation of the @ref List itself. For example,
 *                              @ref ListNode's are allocated using this function, and @ref list_to_array uses
 *                              this function to allocate an array of data pointers.
 * @param list_free             The free function used to dynamically free all memory used by the @ref List.
 * @param data_free             The free function used to dynamically free the stored data in each
 *                              @ref ListNode. If NULL, then the @ref List will not take ownership over the
 *                              data contained in each @ref ListNode, meaning it will not free up the data
 *                              in a @ref ListNode when the @ref ListNode is removed.
 * @return                      NULL if the allocation failed, otherwise the memory address of newly created
 *                              @ref List.
 * @warning                     Undefined behavior if: @param list_malloc == NULL, or @param list_free == NULL.
 */
List* list_create(void* (*list_malloc)(size_t), void (*list_free)(void*), void (*data_free)(void*));

/**
 * @function                    list_destroy
 * @brief                       Dynamically frees the @ref List and all @ref ListNodes associated with it. If
 *                              @param list is a NULL pointer, this function simply returns.
 * @param list                  The @ref List to be destroyed (i.e. deallocated).
 */
void list_destroy(List *list);

/**
 * @function                    list_to_array
 * @brief                       Dynamically allocates an array of pointers-to-void of size list->size.
 *                              list->list_malloc is used to dynamically allocate this array.
 * @param list                  The @ref List to be used to create the array.
 * @return                      NULL if allocation failed, otherwise the memory address of the newly created
 *                              array of pointers-to-void (i.e. pointers to data).
 * @warning                     Undefined behavior if: @param list == NULL. Note that the user is responsible
 *                              for freeing this array.
 */
void** list_to_array(List *list);

/**
 * @function                    list_position
 * @brief                       Retrieves the position of the @param node in the @param list.
 * @param list                  The @ref List that contains the @param node.
 * @param node                  The @ref ListNode whose index is wanted.
 * @return                      The position of the @param node in the @param list.
 * @warning                     Undefined behavior if: @param list == NULL, @param node == NULL, or
 *                              @param node is not in @param list.
 */
size_t list_position(List *list, ListNode *node);

/**
 * @function                    list_at
 * @brief                       Retrieves the @ref ListNode at the @param index.
 * @param list                  The @ref List containing nodes.
 * @param index                 The index of the wanted @ref ListNode.
 * @return                      The @ref ListNode at the @param index.
 * @warning                     Undefined behavior if: @param list == NULL, or @param index is out of bounds.
 */
ListNode* list_at(List *list, size_t index);

/**
 * @function                    list_insert
 * @brief                       Inserts a new @ref ListNode with value @param data at position @param index.
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @param index                 The index of where the new @ref ListNode will reside.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, or @param index greater than the
 *                              size of @param list.
 */
ListNode* list_insert(List *list, void *data, size_t index);

/**
 * @function                    list_insert_left
 * @brief                       Inserts a new @ref ListNode with value @param data to the left of @param node.
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @param node                  The @ref ListNode which is used as a reference point.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, @param node == NULL, or
 *                              @param node is not in @param list.
 */
ListNode* list_insert_left(List *list, void *data, ListNode *node);

/**
 * @function                    list_insert_right
 * @brief                       Inserts a new @ref ListNode with value @param data to the right of
 *                              @param node.
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @param node                  The @ref ListNode which is used as a reference point.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, @param node == NULL, or
 *                              @param node is not in @param list.
 */
ListNode* list_insert_right(List *list, void *data, ListNode *node);

/**
 * @function                    list_push_front
 * @brief                       Pushes a new @ref ListNode with value @param data onto the beginning of the
 *                              @param list.
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL.
 */
ListNode* list_push_front(List *list, void *data);

/**
 * @function                    list_push_back
 * @brief                       Pushes a new @ref ListNode with value @param data onto the end of the
 *                              @param list.
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL.
 */
ListNode* list_push_back(List *list, void *data);

/**
 * @function                    list_clear
 * @brief                       Clears the @param list of all @ref ListNode's. list->size == 0
 * @param list                  The @ref List to clear.
 * @warning                     Undefined behavior if: @param list == NULL.
 */
void list_clear(List *list);

/**
 * @function                    list_delete
 * @brief                       Removes the @ref ListNode at @param index from the @param list.
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @param index                 The index of the @ref ListNode in the @param list to remove.
 * @return                      NULL if the @param list has ownership of the stored data to be removed,
 *                              otherwise returns the stored data in the removed @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, or @param index is out of bounds.
 */
void* list_delete(List *list, size_t index);

/**
 * @function                    list_delete_node
 * @brief                       Removes the @param node from the @param list.
 * @param list                  The @ref List containing the @param node to be removed.
 * @param node                  The @ref ListNode to be removed.
 * @return                      NULL if @param node is NULL, or the @param list has ownership of the stored
 *                              data to be removed, otherwise returns the stored data in the removed
 *                              @param node.
 * @warning                     Undefined behavior if: @param list == NULL, or @param node is not NULL and is
 *                              not in @param list.
 */
void* list_delete_node(List *list, ListNode *node);

/**
 * @function                    list_delete_left
 * @brief                       Removes the @ref ListNode to the left of @param node.
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @param node                  The @ref ListNode which is used as a reference point.
 * @return                      NULL if the @param list has ownership of the stored data to be removed,
 *                              otherwise returns the stored data in the removed @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, @param node == NULL, @param node
 *                              is not in @param list, or @param list->head == @param node.
 */
void* list_delete_left(List *list, ListNode *node);

/**
 * @function                    list_delete_right
 * @brief                       Removes the @ref ListNode to the right of @param node.
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @param node                  The @ref ListNode which is used as a reference point.
 * @return                      NULL if the @param list has ownership of the stored data to be removed,
 *                              otherwise returns the stored data in the removed @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, @param node == NULL, @param node
 *                              is not in @param list, or @param list->tail == @param node.
 */
void* list_delete_right(List *list, ListNode *node);

/**
 * @function                    list_pop_front
 * @brief                       Removes the front @ref ListNode from the @param list.
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @return                      NULL if the @param list has ownership of the stored data to be removed,
 *                              otherwise returns the stored data in the removed @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, or the @param list is empty.
 */
void* list_pop_front(List *list);

/**
 * @function                    list_pop_back
 * @brief                       Removes the back @ref ListNode from the @param list.
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @return                      NULL if the @param list has ownership of the stored data to be removed,
 *                              otherwise returns the stored data in the removed @ref ListNode.
 * @warning                     Undefined behavior if: @param list == NULL, or the @param list is empty.
 */
void* list_pop_back(List *list);

/**
 * @function                    list_sort
 * @brief                       Uses the merge sort algorithm to sort the @param list in-place. This sort is
 *                              stable (order of "equal" @ref ListNode's is preserved). O(nlog(n)) time
 *                              complexity, O(1) space complexity. The time-space complexity is achieved
 *                              through an iterative sort rather than a recursive one.
 * @param list                  The @ref List to sort.
 * @param compare_data          The compare function to be used. The data members of the two @ref ListNode's
 *                              being compared is passed to this function.
 * @warning                     Undefined behavior if: @param list == NULL, or @param compare == NULL.
 */
void list_sort(List *list, int (*compare_data)(const void*, const void*));

/**
 * @function                    list_for_each
 * @brief                       Iterates over the @ref List from front to back.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 * @warning                     Undefined behavior if: @param list_ptr == NULL, or the @ref ListNode
 *                              @param temp_name is reassigned/removed.
 */
#define list_for_each(temp_name, list_ptr) \
    for (ListNode *temp_name = (list_ptr)->head; temp_name; temp_name = temp_name->next)

/**
 * @function                    list_for_each_reverse
 * @brief                       Iterates over the @ref List from back to front.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 * @warning                     Undefined behavior if: @param list_ptr == NULL, or the @ref ListNode
 *                              @param temp_name is reassigned/removed.
 */
#define list_for_each_reverse(temp_name, list_ptr) \
    for (ListNode *temp_name = (list_ptr)->tail; temp_name; temp_name = temp_name->prev)

/**
 * @function                    list_for_each_safe
 * @brief                       Iterates over the @ref List from front to back, and is safe against
 *                              reassignment and/or removal of the @ref ListNode @param temp_name.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 * @warning                     Undefined behavior if: @param list_ptr == NULL.
 */
#define list_for_each_safe(temp_name, list_ptr) \
    for ( \
        ListNode *temp_name = (list_ptr)->head, \
        *cpy_ ## __LINE__ = (list_ptr)->head; \
        temp_name; \
        temp_name = cpy_ ## __LINE__->next, cpy_ ## __LINE__ = temp_name \
    )

/**
 * @function                    list_for_each_safe_reverse
 * @brief                       Iterates over the @ref List from back to front, and is safe against
 *                              reassignment and/or removal of the @ref ListNode @param temp_name.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 * @warning                     Undefined behavior if: @param list_ptr == NULL.
 */
#define list_for_each_safe_reverse(temp_name, list_ptr) \
    for ( \
        ListNode *temp_name = (list_ptr)->tail, \
        *cpy_ ## __LINE__ = (list_ptr)->tail; \
        temp_name; \
        temp_name = cpy_ ## __LINE__->prev, cpy_ ## __LINE__ = temp_name \
    )

/**
 * @function                    list_for_each_continue
 * @brief                       Continues iterating over the @ref List, continuing AFTER the
 *                              @param current_listnode_ptr.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "next" member
 *                              will be the initial value of the @ref ListNode @param temp_name.
 * @warning                     Undefined behavior if: @ref ListNode @param temp_name is reassigned/removed.
 */
#define list_for_each_continue(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) ? \
            ((ListNode*)(current_listnode_ptr))->next : NULL; \
        temp_name; \
        temp_name = temp_name->next \
    )

/**
 * @function                    list_for_each_continue_reverse
 * @brief                       Continues iterating in reverse order over the @ref List, continuing AFTER the
 *                              @param current_listnode_ptr.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "prev" member
 *                              will be the initial value of the @ref ListNode @param temp_name.
 * @warning                     Undefined behavior if: @ref ListNode @param temp_name is reassigned/removed.
 */
#define list_for_each_continue_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) ? \
            ((ListNode*)(current_listnode_ptr))->prev : NULL; \
        temp_name; \
        temp_name = temp_name->prev \
    )

/**
 * @function                    list_for_each_safe_continue
 * @brief                       Continues iterating over the @ref List, continuing AFTER the
 *                              @param current_listnode_ptr, and is safe against reassignment and/or removal
 *                              of the @ref ListNode @param temp_name.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "next" member
 *                              will be the initial value of the @ref ListNode @param temp_name.
 */
#define list_for_each_safe_continue(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) ? \
            ((ListNode*)(current_listnode_ptr))->next : NULL, \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)) ? \
            ((ListNode*)(current_listnode_ptr))->next : NULL; \
        temp_name; \
        temp_name = cpy_ ## __LINE__->next, cpy_ ## __LINE__ = temp_name \
    )

/**
 * @function                    list_for_each_safe_continue_reverse
 * @brief                       Continues iterating in reverse order over the @ref List, continuing AFTER the
 *                              @param current_listnode_ptr, and is safe against reassignment and/or removal
 *                              of the @ref ListNode @param temp_name.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "prev" member
 *                              will be the initial value of the @ref ListNode @param temp_name.
 */
#define list_for_each_safe_continue_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) ? \
            ((ListNode*)(current_listnode_ptr))->prev : NULL, \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)) ? \
            ((ListNode*)(current_listnode_ptr))->prev : NULL; \
        temp_name; \
        temp_name = cpy_ ## __LINE__->prev, cpy_ ## __LINE__ = temp_name \
    )

/**
 * @function                    list_for_each_from
 * @brief                       Continues iterating over the @ref List, continuing FROM (INCLUSIVE) the
 *                              @param current_listnode_ptr.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @param temp_name.
 * @warning                     Undefined behavior if: @ref ListNode @param temp_name is reassigned/removed.
 */
#define list_for_each_from(temp_name, current_listnode_ptr) \
    for (ListNode *temp_name = ((ListNode*)(current_listnode_ptr)); temp_name; temp_name = temp_name->next)

/**
 * @function                    list_for_each_from_reverse
 * @brief                       Continues iterating in reverse order over the @ref List, continuing FROM
 *                              (INCLUSIVE) the @param current_listnode_ptr.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @param temp_name.
 * @warning                     Undefined behavior if: @ref ListNode @param temp_name is reassigned/removed.
 */
#define list_for_each_from_reverse(temp_name, current_listnode_ptr) \
    for (ListNode *temp_name = ((ListNode*)(current_listnode_ptr)); temp_name; temp_name = temp_name->prev)

/**
 * @function                    list_for_each_safe_from
 * @brief                       Continues iterating over the @ref List, continuing FROM (INCLUSIVE) the
 *                              @param current_listnode_ptr, and is safe against reassignment and/or removal
 *                              of the @ref ListNode @param temp_name.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @param temp_name.
 */
#define list_for_each_safe_from(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)), \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)); \
        temp_name; \
        temp_name = cpy_ ## __LINE__->next, cpy_ ## __LINE__ = temp_name \
    )

/**
 * @function                    list_for_each_safe_from_reverse
 * @brief                       Continues iterating in reverse order over the @ref List, continuing FROM
 *                              (INCLUSIVE) the @param current_listnode_ptr, and is safe against reassignment
 *                              and/or removal of the @ref ListNode @param temp_name.
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @param temp_name.
 */
#define list_for_each_safe_from_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)), \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)); \
        temp_name; \
        temp_name = cpy_ ## __LINE__->prev, cpy_ ## __LINE__ = temp_name \
    )

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIST_H__
