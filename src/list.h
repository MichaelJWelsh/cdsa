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
 * @brief   DOUBLY LINKED LIST
 *
 * Dependencies:
 *      -   C99 standard library
 *
 * API:
 *      ====  FUNCTIONS  ====
 *      -   list_create
 *      -   list_destroy
 *      -   list_to_array
 *      -   list_index_of
 *      -   list_at
 *      -   list_insert_left
 *      -   list_insert_right
 *      -   list_push_front
 *      -   list_push_back
 *      -   list_clear
 *      -   list_remove
 *      -   list_pop_front
 *      -   list_pop_back
 *      -   list_sort
 *      -   list_splice_left
 *      -   list_splice_right
 *      -   list_splice_front
 *      -   list_splice_back
 *
 *      ====  MACROS  ====
 *      -   list_for_each
 *      -   list_for_each_reverse
 *      -   list_for_each_safe
 *      -   list_for_each_safe_reverse
 *      -   list_for_each_continue
 *      -   list_for_each_continue_reverse
 *      -   list_for_each_safe_continue
 *      -   list_for_each_safe_continue_reverse
 *      -   list_for_each_from
 *      -   list_for_each_from_reverse
 *      -   list_for_each_safe_from
 *      -   list_for_each_safe_from_reverse
 */

#ifndef CDSAA_LIST_H
#define CDSAA_LIST_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>

/**
 * Represents a node in a @ref List.
 */
typedef struct ListNode {
    void *data;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

/**
 * Represents a doubly linked list.
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
 * Creates (allocates) a new empty @ref List.
 *
 * Requirements:
 *      -   @ref list_malloc != NULL
 *      -   @ref list_free != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list_malloc           The malloc function used to allocate all memory used by the @ref List,
 *                              including the creation of the @ref List itself. For example, @ref ListNode's
 *                              are allocated using this function, and @ref list_to_array uses this function
 *                              to allocate an array of data pointers.
 * @param list_free             The free function used to deallocate all memory used by the @ref List.
 * @param data_free             The free function used to deallocate the stored data in each @ref ListNode. If
 *                              NULL, then the @ref List will not take ownership over the data contained in
 *                              each @ref ListNode, meaning it will not free up the data in a @ref ListNode
 *                              when the @ref ListNode is to be deallocated.
 * @return                      NULL if the allocation failed, otherwise the memory address of newly created
 *                              @ref List.
 */
List* list_create(void* (*list_malloc)(size_t), void (*list_free)(void*), void (*data_free)(void*));

/**
 * Deallocates the @ref List and all @ref ListNodes associated with it. If the @ref list has ownership over
 * the stored data, then all stored data is also deallocated. If @ref list is a NULL pointer, this function
 * simply returns. Note that the parameter of this function is a pointer-to-void, meaning that its type is the
 * same as every generic free function, meaning that, for example, @ref list->data_free can equal this
 * function.
 *
 * Requirements:
 *      -   none
 *
 * Time complexity:
 *      -   O(n)
 *
 * @param list                  The @ref List to be destroyed.
 */
void list_destroy(void *list);

/**
 * Allocates an array of pointers-to-void (pointers to what the data pointers point to in each @ref ListNode)
 * of size @ref list->size. @ref list->list_malloc is used to allocate this array. This array will contain a
 * dangling pointer if the @ref list has ownership over the data, and a @ref ListNode is deallocated. Note
 * that the user is responsible for freeing this array.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(n)
 *
 * @param list                  The @ref List to be used to create the array.
 * @return                      NULL if allocation failed, otherwise the memory address of the newly created
 *                              array of pointers-to-void (i.e. pointers to data).
 */
void** list_to_array(List *list);

/**
 * Retrieves the index of the @ref node in the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref node != NULL
 *      -   @ref node is part of @ref list
 *
 * Time complexity:
 *      -   If front/back:
 *          -   O(1)
 *      -   Else:
 *          -   On average:     O(n/2)
 *          -   Worst case:     O(n)
 *
 * @param list                  The @ref List that contains the @ref node.
 * @param node                  The @ref ListNode whose index is wanted.
 * @return                      The index of the @ref node in the @ref list.
 */
size_t list_index_of(List *list, ListNode *node);

/**
 * Retrieves the @ref ListNode at the @ref index.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref index < @ref list->size
 *
 * Time complexity:
 *      -   If front/back:
 *          -   O(1)
 *      -   Else:
 *          -   On average:     O(n/4)
 *          -   Worst case:     O(n/2)
 *
 * @param list                  The @ref List containing nodes.
 * @param index                 The index of the wanted @ref ListNode.
 * @return                      The @ref ListNode at the @ref index.
 */
ListNode* list_at(List *list, size_t index);

/**
 * Inserts a newly allocated @ref ListNode with value @ref data to the left of @ref node.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref node != NULL
 *      -   @ref node is part of @ref list
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @param node                  The @ref ListNode which is used as a reference point.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 */
ListNode* list_insert_left(List *list, void *data, ListNode *node);

/**
 * Inserts a newly allocated @ref ListNode with value @ref data to the right of @ref node.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref node != NULL
 *      -   @ref node is part of @ref list
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @param node                  The @ref ListNode which is used as a reference point.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 */
ListNode* list_insert_right(List *list, void *data, ListNode *node);

/**
 * Pushes a newly allocated @ref ListNode with value @ref data into the front of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 */
ListNode* list_push_front(List *list, void *data);

/**
 * Pushes a newly allocated @ref ListNode with value @ref data into the back of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param data                  The data of the new @ref ListNode.
 * @return                      NULL if allocation of new @ref ListNode failed, otherwise returns the new
 *                              @ref ListNode.
 */
ListNode* list_push_back(List *list, void *data);

/**
 * Clears the @ref list of all @ref ListNode's and deallocates them. If the @ref list has ownership over the
 * stored data, then all stored data is also deallocated.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(n)
 *
 * @param list                  The @ref List to clear.
 */
void list_clear(List *list);

/**
 * Removes the @ref node from the @ref list and deallocates it. If the @ref list has ownership over the stored
 * data, then the @ref node's stored data is also deallocated. If @ref node is a NULL pointer, this function
 * simply returns NULL.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   If @ref node != NULL, then it must be part of @ref list
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List containing the @ref node to be removed.
 * @param node                  The @ref ListNode to be removed.
 * @return                      NULL if @ref node == NULL or the @ref list has ownership of the stored data to
 *                              be removed, otherwise returns the stored data in the removed @ref node.
 */
void* list_remove(List *list, ListNode *node);

/**
 * Removes the front @ref ListNode from the @ref list and deallocates it. If the @ref list has ownership over
 * the stored data, then the @ref ListNode's data is also deallocated. If the @ref list is empty, this
 * function simply returns NULL.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @return                      NULL if the @ref list is empty or has ownership of the stored data to be
 *                              removed, otherwise returns the stored data in the removed @ref ListNode.
 */
void* list_pop_front(List *list);

/**
 * Removes the back @ref ListNode from the @ref list and deallocates it. If the @ref list has ownership over
 * the stored data, then the @ref ListNode's data is also deallocated. If the @ref list is empty, this
 * function simply returns NULL.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List containing the @ref ListNode to be removed.
 * @return                      NULL if the @ref list is empty or has ownership of the stored data to be
 *                              removed, otherwise returns the stored data in the removed @ref ListNode.
 */
void* list_pop_back(List *list);

/**
 * Uses the merge sort algorithm to sort the @ref list in-place. This sort is stable (order of "equal"
 * @ref ListNode's is preserved). This function uses an iterative algorithm rather than a recursive one,
 * meaning it has an auxiliary space complexity of O(1).
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref compare_data != NULL
 *
 * Time complexity:
 *      -   O(nlog(n))
 *
 * @param list                  The @ref List to sort.
 * @param compare_data          The compare function to be used. The data members of the two @ref ListNode's
 *                              being compared is passed to this function.
 */
void list_sort(List *list, int (*compare_data)(const void*, const void*));

/**
 * Transfers the @ref ListNode's from @ref list2 in the range [@ref from, @ref to] to the left of @ref node in
 * @ref list1. This effectively inserts the @ref ListNode's in the range [@ref from, @ref to] into @ref list1
 * and removes them from @ref list2, altering the sizes of both @ref List's. This operation does not involve
 * the creation or deletion of any @ref ListNode's. The allocators/deallocators of both @ref List's must be
 * equal. If @ref from and @ref to are both equal to NULL, this function simply returns. @ref list1 is allowed
 * to equal @ref list2 so long as several additional requirements are met.
 *
 * Requirements:
 *      -   @ref list1 != NULL
 *      -   @ref node != NULL
 *      -   @ref node is part of @ref list1
 *      -   @ref list2 != NULL
 *      -   The allocators/deallocators of both @ref List's must be equal.
 *      -   (@ref from != NULL and @ref to != NULL) or (@ref from == NULL and @ref to == NULL)
 *      -   @ref from and @ref to must be part of @ref list2 (if they are not NULL)
 *      -   @ref range_size correctly represents the size of the range [@ref from, @ref to]
 *      -   If @ref list1 == @ref list2, then @ref node cannot be in the range [@ref from, @ref to]
 *
 * Time complexity:
 *      O(1)
 *
 * @param list1                 The @ref List that will become the new owner of the @ref ListNode's in the
 *                              range [@ref from, @ref to].
 * @param node                  The @ref ListNode which is used as a reference point.
 * @param list2                 The @ref List that is the original owner of the @ref ListNode's in the range
 *                              [@ref from, @ref to].
 * @param from                  The @ref ListNode that marks the beginning of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param to                    The @ref ListNode that marks the end of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param range_size            The size of the range [@ref from, @ref to]. Note that this function is able to
 *                              have such a low time complexity because it does not calculate the size of the
 *                              range itself. It is up to the user to calculate the size of the range. This is
 *                              done because sometimes the size of the range is known without having to
 *                              iterate from the beginning to the end of the range.
 */
void list_splice_left(List *list1, ListNode *node, List *list2, ListNode *from, ListNode *to, size_t range_size);

/**
 * Transfers the @ref ListNode's from @ref list2 in the range [@ref from, @ref to] to the right of @ref node
 * in @ref list1. This effectively inserts the @ref ListNode's in the range [@ref from, @ref to] into
 * @ref list1 and removes them from @ref list2, altering the sizes of both @ref List's. This operation does
 * not involve the creation or deletion of any @ref ListNode's. The allocators/deallocators of both
 * @ref List's must be equal. If @ref from and @ref to are both equal to NULL, this function simply returns.
 * @ref list1 is allowed to equal @ref list2 so long as several additional requirements are met.
 *
 * Requirements:
 *      -   @ref list1 != NULL
 *      -   @ref node != NULL
 *      -   @ref node is part of @ref list1
 *      -   @ref list2 != NULL
 *      -   The allocators/deallocators of both @ref List's must be equal.
 *      -   (@ref from != NULL and @ref to != NULL) or (@ref from == NULL and @ref to == NULL)
 *      -   @ref from and @ref to must be part of @ref list2 (if they are not NULL)
 *      -   @ref range_size correctly represents the size of the range [@ref from, @ref to]
 *      -   If @ref list1 == @ref list2, then @ref node cannot be in the range [@ref from, @ref to]
 *
 * Time complexity:
 *      O(1)
 *
 * @param list1                 The @ref List that will become the new owner of the @ref ListNode's in the
 *                              range [@ref from, @ref to].
 * @param node                  The @ref ListNode which is used as a reference point.
 * @param list2                 The @ref List that is the original owner of the @ref ListNode's in the range
 *                              [@ref from, @ref to].
 * @param from                  The @ref ListNode that marks the beginning of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param to                    The @ref ListNode that marks the end of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param range_size            The size of the range [@ref from, @ref to]. Note that this function is able to
 *                              have such a low time complexity because it does not calculate the size of the
 *                              range itself. It is up to the user to calculate the size of the range. This is
 *                              done because sometimes the size of the range is known without having to
 *                              iterate from the beginning to the end of the range.
 */
void list_splice_right(List *list1, ListNode *node, List *list2, ListNode *from, ListNode *to, size_t range_size);

/**
 * Transfers the @ref ListNode's from @ref list2 in the range [@ref from, @ref to] to the front of @ref list1.
 * This effectively inserts the @ref ListNode's in the range [@ref from, @ref to] into @ref list1 and removes
 * them from @ref list2, altering the sizes of both @ref List's. This operation does not involve the creation
 * or deletion of any @ref ListNode's. The allocators/deallocators of both @ref List's must be equal. If
 * @ref from and @ref to are both equal to NULL, this function simply returns. @ref list1 is allowed to equal
 * @ref list2.
 *
 * Requirements:
 *      -   @ref list1 != NULL
 *      -   @ref list2 != NULL
 *      -   The allocators/deallocators of both @ref List's must be equal.
 *      -   (@ref from != NULL and @ref to != NULL) or (@ref from == NULL and @ref to == NULL)
 *      -   @ref from and @ref to must be part of @ref list2 (if they are not NULL)
 *      -   @ref range_size correctly represents the size of the range [@ref from, @ref to]
 *
 * Time complexity:
 *      O(1)
 *
 * @param list1                 The @ref List that will become the new owner of the @ref ListNode's in the
 *                              range [@ref from, @ref to].
 * @param list2                 The @ref List that is the original owner of the @ref ListNode's in the range
 *                              [@ref from, @ref to].
 * @param from                  The @ref ListNode that marks the beginning of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param to                    The @ref ListNode that marks the end of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param range_size            The size of the range [@ref from, @ref to]. Note that this function is able to
 *                              have such a low time complexity because it does not calculate the size of the
 *                              range itself. It is up to the user to calculate the size of the range. This is
 *                              done because sometimes the size of the range is known without having to
 *                              iterate from the beginning to the end of the range.
 */
void list_splice_front(List *list1, List *list2, ListNode *from, ListNode *to, size_t range_size);

/**
 * Transfers the @ref ListNode's from @ref list2 in the range [@ref from, @ref to] to the back of @ref list1.
 * This effectively inserts the @ref ListNode's in the range [@ref from, @ref to] into @ref list1 and removes
 * them from @ref list2, altering the sizes of both @ref List's. This operation does not involve the creation
 * or deletion of any @ref ListNode's. The allocators/deallocators of both @ref List's must be equal. If
 * @ref from and @ref to are both equal to NULL, this function simply returns. @ref list1 is allowed to equal
 * @ref list2.
 *
 * Requirements:
 *      -   @ref list1 != NULL
 *      -   @ref list2 != NULL
 *      -   The allocators/deallocators of both @ref List's must be equal.
 *      -   (@ref from != NULL and @ref to != NULL) or (@ref from == NULL and @ref to == NULL)
 *      -   @ref from and @ref to must be part of @ref list2 (if they are not NULL)
 *      -   @ref range_size correctly represents the size of the range [@ref from, @ref to]
 *
 * Time complexity:
 *      O(1)
 *
 * @param list1                 The @ref List that will become the new owner of the @ref ListNode's in the
 *                              range [@ref from, @ref to].
 * @param list2                 The @ref List that is the original owner of the @ref ListNode's in the range
 *                              [@ref from, @ref to].
 * @param from                  The @ref ListNode that marks the beginning of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param to                    The @ref ListNode that marks the end of the range of @ref ListNode's
 *                              (INCLUSIVE).
 * @param range_size            The size of the range [@ref from, @ref to]. Note that this function is able to
 *                              have such a low time complexity because it does not calculate the size of the
 *                              range itself. It is up to the user to calculate the size of the range. This is
 *                              done because sometimes the size of the range is known without having to
 *                              iterate from the beginning to the end of the range.
 */
void list_splice_back(List *list1, List *list2, ListNode *from, ListNode *to, size_t range_size);

/**
 * Iterates over the @ref List from front to back.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *      -   The @ref ListNode @ref temp_name is neither reassigned nor removed from the @ref list in the
 *          loop's body.
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each(temp_name, list_ptr) \
    for ( \
        ListNode *temp_name = (list_ptr)->head; \
        temp_name != NULL; \
        temp_name = temp_name->next \
    )

/**
 * Iterates over the @ref List from back to front.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *      -   The @ref ListNode @ref temp_name is neither reassigned nor removed from the @ref list in the
 *          loop's body.
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each_reverse(temp_name, list_ptr) \
    for ( \
        ListNode *temp_name = (list_ptr)->tail; \
        temp_name != NULL; \
        temp_name = temp_name->prev \
    )

/**
 * Iterates over the @ref List from front to back, and is safe against reassignment and/or removal of the
 * @ref ListNode @ref temp_name.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each_safe(temp_name, list_ptr) \
    for ( \
        ListNode *temp_name = (list_ptr)->head, \
        *cpy_ ## __LINE__ = (list_ptr)->head; \
        temp_name != NULL; \
        temp_name = cpy_ ## __LINE__->next, cpy_ ## __LINE__ = temp_name \
    )

/**
 * Iterates over the @ref List from back to front, and is safe against reassignment and/or removal of the
 * @ref ListNode @ref temp_name.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each_safe_reverse(temp_name, list_ptr) \
    for ( \
        ListNode *temp_name = (list_ptr)->tail, \
        *cpy_ ## __LINE__ = (list_ptr)->tail; \
        temp_name != NULL; \
        temp_name = cpy_ ## __LINE__->prev, cpy_ ## __LINE__ = temp_name \
    )

/**
 * Continues iterating over the @ref List, continuing AFTER the @ref current_listnode_ptr. If either
 * @ref current_listnode_ptr or the @ref ListNode AFTER it is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   The @ref ListNode @ref temp_name is neither reassigned nor removed from its associated @ref List
 *          in the loop's body.
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "next" member
 *                              will be the initial value of the @ref ListNode @ref temp_name.
 */
#define list_for_each_continue(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) != NULL ? \
            ((ListNode*)(current_listnode_ptr))->next : NULL; \
        temp_name != NULL; \
        temp_name = temp_name->next \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing AFTER the @ref current_listnode_ptr. If
 * either @ref current_listnode_ptr or the @ref ListNode AFTER it is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   The @ref ListNode @ref temp_name is neither reassigned nor removed from its associated @ref List
 *          in the loop's body.
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "prev" member
 *                              will be the initial value of the @ref ListNode @ref temp_name.
 */
#define list_for_each_continue_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) != NULL ? \
            ((ListNode*)(current_listnode_ptr))->prev : NULL; \
        temp_name != NULL; \
        temp_name = temp_name->prev \
    )

/**
 * Continues iterating over the @ref List, continuing AFTER the @ref current_listnode_ptr, and is safe against
 * reassignment and/or removal of the @ref ListNode @ref temp_name. If either @ref current_listnode_ptr or the
 * @ref ListNode AFTER it is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   none
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "next" member
 *                              will be the initial value of the @ref ListNode @ref temp_name.
 */
#define list_for_each_safe_continue(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) != NULL ? \
            ((ListNode*)(current_listnode_ptr))->next : NULL, \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)) != NULL ? \
            ((ListNode*)(current_listnode_ptr))->next : NULL; \
        temp_name != NULL; \
        temp_name = cpy_ ## __LINE__->next, cpy_ ## __LINE__ = temp_name \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing AFTER the @ref current_listnode_ptr,
 * and is safe against reassignment and/or removal of the @ref ListNode @ref temp_name. If either
 * @ref current_listnode_ptr or the @ref ListNode AFTER it is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   none
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be SKIPPED, but whose "prev" member
 *                              will be the initial value of the @ref ListNode @ref temp_name.
 */
#define list_for_each_safe_continue_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)) != NULL ? \
            ((ListNode*)(current_listnode_ptr))->prev : NULL, \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)) != NULL ? \
            ((ListNode*)(current_listnode_ptr))->prev : NULL; \
        temp_name != NULL; \
        temp_name = cpy_ ## __LINE__->prev, cpy_ ## __LINE__ = temp_name \
    )

/**
 * Continues iterating over the @ref List, continuing FROM (INCLUSIVE) the @ref current_listnode_ptr. If
 * @ref current_listnode_ptr is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   The @ref ListNode @ref temp_name is neither reassigned nor removed from its associated @ref List
 *          in the loop's body.
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @ref temp_name.
 */
#define list_for_each_from(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)); \
        temp_name != NULL; \
        temp_name = temp_name->next \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing FROM (INCLUSIVE) the
 * @ref current_listnode_ptr. If @ref current_listnode_ptr is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   The @ref ListNode @ref temp_name is neither reassigned nor removed from its associated @ref List
 *          in the loop's body.
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @ref temp_name.
 */
#define list_for_each_from_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)); \
        temp_name != NULL; \
        temp_name = temp_name->prev \
    )

/**
 * Continues iterating over the @ref List, continuing FROM (INCLUSIVE) the @ref current_listnode_ptr, and is
 * safe against reassignment and/or removal of the @ref ListNode @ref temp_name. If @ref current_listnode_ptr
 * is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   none
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @ref temp_name.
 */
#define list_for_each_safe_from(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)), \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)); \
        temp_name != NULL; \
        temp_name = cpy_ ## __LINE__->next, cpy_ ## __LINE__ = temp_name \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing FROM (INCLUSIVE) the
 * @ref current_listnode_ptr, and is safe against reassignment and/or removal of the @ref ListNode
 * @ref temp_name. If @ref current_listnode_ptr is NULL, the loop's body will not execute.
 *
 * Requirements:
 *      -   none
 *
 * @param temp_name             The temporary variable name used in the loop's scope.
 * @param current_listnode_ptr  The pointer to a @ref ListNode that will be the initial value of the
 *                              @ref ListNode @ref temp_name.
 */
#define list_for_each_safe_from_reverse(temp_name, current_listnode_ptr) \
    for ( \
        ListNode *temp_name = ((ListNode*)(current_listnode_ptr)), \
        *cpy_ ## __LINE__ = ((ListNode*)(current_listnode_ptr)); \
        temp_name != NULL; \
        temp_name = cpy_ ## __LINE__->prev, cpy_ ## __LINE__ = temp_name \
    )

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CDSAA_LIST_H
