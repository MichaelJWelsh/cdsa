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
 * Embed one or more @ref ListNode's into your struct to make it a potential node in one or more linked lists.
 * The @ref List structure keeps track of a chain of @ref ListNode's. A @ref List structure MUST be
 * initialized before it is used. A @ref ListNode structure does NOT need to be initialized before it is used.
 * A @ref ListNode should belong to at most ONE @ref List.
 *
 * Example:
 *          struct Object {
 *              int val;
 *              ListNode n;
 *          };
 *
 *          int main(void) {
 *              struct Object obj;
 *              List list;
 *              int copy_val;
 *
 *              list_init(&list);
 *              list_insert_back(&list, &obj.n);
 *
 *              obj.val = 5;
 *              copy_val = list_entry(list_front(&list), struct Object, n)->val;
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
 *      -   typedef struct List List
 *      -   typedef struct ListNode ListNode
 *
 *      ====  FUNCTIONS  ====
 *      Initializers:
 *          -   list_init
 *      Properties:
 *          -   list_front
 *          -   list_back
 *          -   list_prev
 *          -   list_next
 *          -   list_size
 *          -   list_empty
 *      Array Interfacing:
 *          -   list_index_of
 *          -   list_at
 *      Insertion:
 *          -   list_insert_left
 *          -   list_insert_right
 *          -   list_insert_front
 *          -   list_insert_back
 *      Splicing:
 *          -   list_splice_left
 *          -   list_splice_right
 *          -   list_splice_front
 *          -   list_splice_back
 *      Removal:
 *          -   list_remove
 *          -   list_remove_front
 *          -   list_remove_back
 *          -   list_remove_all
 *      Low-Level Manipulation:
 *          -   list_cut
 *          -   list_paste
 *      Sorting:
 *          -   list_sort
 *
 *      ====  MACROS  ====
 *      Constants:
 *          -   LIST_POISON_PREV
 *          -   LIST_POISON_NEXT
 *      Convenient Node Initializer:
 *          -   LIST_NODE_INIT
 *      Properties:
 *          -   list_entry
 *      Traversal:
 *          -   list_for_each
 *          -   list_for_each_reverse
 *          -   list_for_each_safe
 *          -   list_for_each_safe_reverse
 *          -   list_for_each_after
 *          -   list_for_each_after_reverse
 *          -   list_for_each_safe_after
 *          -   list_for_each_safe_after_reverse
 *          -   list_for_each_from
 *          -   list_for_each_from_reverse
 *          -   list_for_each_safe_from
 *          -   list_for_each_safe_from_reverse
 */

#ifndef LIST_H
#define LIST_H

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
struct List;
struct ListNode;

/* Struct typedef's. */
typedef struct List List;
typedef struct ListNode ListNode;

/**
 * Represents a doubly linked list.
 */
struct List {
    ListNode *head;
    ListNode *tail;
    size_t size;
};

/**
 * Represents a node in a @ref List. Embed this into your structure to make it a node.
 */
struct ListNode {
    ListNode *prev;
    ListNode *next;
};

/* ========================================================================================================
 *
 *                                               PROTOTYPES
 *
 * ======================================================================================================== */

/**
 * Initializes/resets the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be initialized/reset.
 */
void list_init(List *list);

/**
 * Returns the front of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List whose "head" member will be returned.
 * @return                      @ref list->head.
 */
ListNode* list_front(const List *list);

/**
 * Returns the back of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List whose "tail" member will be returned.
 * @return                      @ref list->tail.
 */
ListNode* list_back(const List *list);

/**
 * Returns the @ref ListNode before the @ref node. NULL if @ref node == NULL.
 *
 * Requirements:
 *      -   None
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param node                  The @ref ListNode whose "prev" member will be returned.
 * @return                      NULL if @ref node == NULL; otherwise, @ref node->prev.
 */
ListNode* list_prev(const ListNode *node);

/**
 * Returns the @ref ListNode after the @ref node. NULL if @ref node == NULL.
 *
 * Requirements:
 *      -   None
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param node                  The @ref ListNode whose "next" member will be returned.
 * @return                      NULL if @ref node == NULL; otherwise, @ref node->next.
 */
ListNode* list_next(const ListNode *node);

/**
 * Returns the size of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List whose "size" member will be returned.
 * @return                      @ref list->size.
 */
size_t list_size(const List *list);

/**
 * Returns whether or not the @ref list is empty (i.e. @ref list->size == 0).
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List whose "size" member will be used to determine if it is empty.
 * @return                      Whether or not the @ref list is empty (i.e. @ref list->size == 0).
 */
int list_empty(const List *list);

/**
 * Retrieves the index of the @ref node in the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref node != NULL
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
size_t list_index_of(const List *list, const ListNode *node);

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
 * @return                      The @ref ListNode at the @ref index in the @ref list.
 */
ListNode* list_at(const List *list, size_t index);

/**
 * Inserts the @ref new_node to the left of the @ref position. If @ref position == NULL, inserts the
 * @ref new_node at the head of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref new_node != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param new_node              The @ref ListNode to be inserted.
 * @param position              The @ref ListNode in the @ref list used as a reference point.
 */
void list_insert_left(List *list, ListNode *new_node, ListNode *position);

/**
 * Inserts the @ref new_node to the right of the @ref position. If @ref position == NULL, inserts the
 * @ref new_node at the tail of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref new_node != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param new_node              The @ref ListNode to be inserted.
 * @param position              The @ref ListNode in the @ref list used as a reference point.
 */
void list_insert_right(List *list, ListNode *new_node, ListNode *position);

/**
 * Inserts the @ref new_node into the front of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref new_node != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param new_node              The @ref ListNode to be inserted.
 */
void list_insert_front(List *list, ListNode *new_node);

/**
 * Inserts the @ref new_node into the back of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref new_node != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param new_node              The @ref ListNode to be inserted.
 */
void list_insert_back(List *list, ListNode *new_node);

/**
 * Removes all the @ref ListNode's in the @ref src_list, and inserts them into the @ref list to the left of
 * the @ref position. If @ref position == NULL, inserts all the @ref ListNode's into the head of the
 * @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref src_list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The consumer @ref List to which elements are moved.
 * @param src_list              The producer @ref List from which elements are removed.
 * @param position              The @ref ListNode in the @ref list used as a reference point.
 */
void list_splice_left(List *list, List *src_list, ListNode *position);

/**
 * Removes all the @ref ListNode's in the @ref src_list, and inserts them into the @ref list to the right of
 * the @ref position. If @ref position == NULL, inserts all the @ref ListNode's into the tail of the
 * @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref src_list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The consumer @ref List to which elements are moved.
 * @param src_list              The producer @ref List from which elements are removed.
 * @param position              The @ref ListNode in the @ref list used as a reference point.
 */
void list_splice_right(List *list, List *src_list, ListNode *position);

/**
 * Removes all the @ref ListNode's in the @ref src_list, and inserts them into the front of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref src_list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The consumer @ref List to which elements are moved.
 * @param src_list              The producer @ref List from which elements are removed.
 */
void list_splice_front(List *list, List *src_list);

/**
 * Removes all the @ref ListNode's in the @ref src_list, and inserts them into the back of the @ref list.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref src_list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The consumer @ref List to which elements are moved.
 * @param src_list              The producer @ref List from which elements are removed.
 */
void list_splice_back(List *list, List *src_list);

/**
 * Removes the @ref node from the @ref list. If @ref node == NULL, this function simply returns.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List containing the @ref node to be removed.
 * @param node                  The @ref ListNode in the @ref list to be removed.
 */
void list_remove(List *list, ListNode *node);

/**
 * Removes the front @ref ListNode from the @ref list. If the @ref list is empty, this function simply
 * returns.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 */
void list_remove_front(List *list);

/**
 * Removes the back @ref ListNode from the @ref list. If the @ref list is empty, this function simply returns.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 */
void list_remove_back(List *list);

/**
 * Removes all the @ref ListNode's from the @ref list. If the @ref list is empty, this function simply
 * returns.
 *
 * Requirements:
 *      -   @ref list != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref list to be operated on.
 */
void list_remove_all(List *list);

/**
 * This is a low-level function and should be used with care.
 *
 * Removes the range [@ref from, @ref to] from the @ref list, resulting in the following @ref ListNode
 * connectivity:
 *      ... <-> ...
 *
 * The @ref list->size is then decremented to account for the loss of the @ref ListNode's.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   Either @ref from and @ref to are both NULL, or are both non-NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param from                  The @ref ListNode that marks the beginning of the range. @ref from can equal
 *                              @ref to.
 * @param to                    The @ref ListNode the marks the end of the range. @ref from can equal @ref to.
 * @param range_size            The size of the range [@ref from, @ref to]. Note that this function is able to
 *                              have such a low time complexity because it does not calculate the size of the
 *                              range itself. It is up to the user to calculate the size of the range. This is
 *                              done because sometimes the size of the range is known without having to
 *                              iterate from the beginning to the end of the range.
 */
void list_cut(List *list, ListNode *from, ListNode *to, size_t range_size);

/**
 * This is a low-level function and should be used with care.
 *
 * Inserts the range [@ref from, @ref to] into the @ref list, resulting in the following @ref ListNode
 * connectivity:
 *          ... <-> left <-> from <-> ... <-> to <-> right <-> ...
 *
 * The @ref list->size is then incrememented to account for the new @ref ListNode's.
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   Either @ref from and @ref to are both NULL, or are both non-NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param list                  The @ref List to be operated on.
 * @param left                  The @ref ListNode that will be to the left of @ref from. Can be NULL.
 * @param from                  The @ref ListNode that marks the beginning of the range. @ref from can equal
 *                              @ref to.
 * @param to                    The @ref ListNode the marks the end of the range. @ref from can equal @ref to.
 * @param right                 The @ref ListNode that will be to the right of @ref to. Can be NULL.
 * @param range_size            The size of the range [@ref from, @ref to]. Note that this function is able to
 *                              have such a low time complexity because it does not calculate the size of the
 *                              range itself. It is up to the user to calculate the size of the range. This is
 *                              done because sometimes the size of the range is known without having to
 *                              iterate from the beginning to the end of the range.
 */
void list_paste(List *list, ListNode *left, ListNode *from, ListNode *to, ListNode *right, size_t range_size);

/**
 * Uses the merge sort algorithm to sort the @ref list in-place. This sort is stable (order of "equal"
 * @ref ListNode's is preserved). This function uses an iterative algorithm rather than a recursive one,
 * meaning it has an auxiliary space complexity of O(1).
 *
 * Requirements:
 *      -   @ref list != NULL
 *      -   @ref compare != NULL
 *
 * Time complexity:
 *      -   O(nlog(n))
 *
 * @param list                  The @ref List to sort.
 * @param compare               The compare function to be used.
 */
void list_sort(List *list, int (*compare)(const ListNode *a, const ListNode *b));

/* ========================================================================================================
 *
 *                                                 MACROS
 *
 * ======================================================================================================== */

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "prev" member of the
 * first @ref ListNode in a chain of removed @ref ListNode's or a single removed @ref ListNode. Useful for
 * identifying bugs.
 */
#define LIST_POISON_PREV ((ListNode*) 0x100)

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "next" member of the
 * last @ref ListNode in a chain of removed @ref ListNode's or a single removed @ref ListNode. Useful for
 * identifying bugs.
 */
#define LIST_POISON_NEXT ((ListNode*) 0x200)

/**
 * Initializing a @ref ListNode before it is used is NOT required. This macro is simply for allowing you to
 * initialize a struct (containing one or more @ref ListNode's) with an initializer-list conveniently.
 */
#define LIST_NODE_INIT { LIST_POISON_PREV, LIST_POISON_NEXT }

/**
 * Obtains the pointer to the struct for this entry.
 *
 * Requirements:
 *      -   @ref node_ptr != NULL
 *
 * @param node_ptr              The pointer to the @ref ListNode in the struct.
 * @param type                  The type of the struct the @ref ListNode is embedded in.
 * @param member                The name of the @ref ListNode in the struct.
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define list_entry(node_ptr, type, member) \
        ({ \
            const typeof(((type*)0)->member) *__mptr = (node_ptr); \
            (type*) ((char*)__mptr - offsetof(type, member)); \
        })
#else
    #define list_entry(node_ptr, type, member) \
        ( \
            (type*) ((char*)(node_ptr) - offsetof(type, member)) \
        )
#endif

/**
 * Iterates over the @ref List from front to back.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref List in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each(cursor_node_ptr, list_ptr) \
    for ( \
        cursor_node_ptr = (list_ptr)->head; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->next \
    )

/**
 * Iterates over the @ref List from back to front.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref List in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each_reverse(cursor_node_ptr, list_ptr) \
    for ( \
        cursor_node_ptr = (list_ptr)->tail; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->prev \
    )

/**
 * Iterates over the @ref List from front to back, and is safe against reassignment and/or removal of the
 * @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref List in the loop's
 *          body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref ListNode to use as temporary storage.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each_safe(cursor_node_ptr, backup_node_ptr, list_ptr) \
    for ( \
        cursor_node_ptr = (list_ptr)->head, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL \
    )

/**
 * Iterates over the @ref List from back to front, and is safe against reassignment and/or removal of the
 * @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref list_ptr != NULL
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref List in the loop's
 *          body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref ListNode to use as temporary storage.
 * @param list_ptr              The pointer to a @ref List that will be iterated over.
 */
#define list_for_each_safe_reverse(cursor_node_ptr, backup_node_ptr, list_ptr) \
    for ( \
        cursor_node_ptr = (list_ptr)->tail, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL \
    )

/**
 * Continues iterating over the @ref List, continuing AFTER the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref List in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. If not NULL, its "next" member will
 *                              be the initial value of it.
 */
#define list_for_each_after(cursor_node_ptr) \
    for ( \
        cursor_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->next \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing AFTER the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref List in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. If not NULL, its "prev" member will
 *                              be the initial value of it.
 */
#define list_for_each_after_reverse(cursor_node_ptr) \
    for ( \
        cursor_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->prev \
    )

/**
 * Continues iterating over the @ref List, continuing AFTER the current position, and is safe against
 * reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref List in the loop's
 *          body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. If not NULL, its "next" member will
 *                              be the initial value of it.
 * @param backup_node_ptr       Another @ref ListNode to use as temporary storage.
 */
#define list_for_each_safe_after(cursor_node_ptr, backup_node_ptr) \
    for ( \
        cursor_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing AFTER the current position, and is safe
 * against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref List in the loop's
 *          body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. If not NULL, its "prev" member will
 *                              be the initial value of it.
 * @param backup_node_ptr       Another @ref ListNode to use as temporary storage.
 */
#define list_for_each_safe_after_reverse(cursor_node_ptr, backup_node_ptr) \
    for ( \
        cursor_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL \
    )

/**
 * Continues iterating over the @ref List, continuing FROM the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref List in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. Its value represents the current
 *                              position.
 */
#define list_for_each_from(cursor_node_ptr) \
    for ( \
        ; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->next \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing FROM the current position.
 *
 * Requirements:
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref List in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. Its value represents the current
 *                              position.
 */
#define list_for_each_from_reverse(cursor_node_ptr) \
    for ( \
        ; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->prev \
    )

/**
 * Continues iterating over the @ref List, continuing FROM the current position, and is safe against
 * reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref List in the loop's
 *          body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. Its value represents the current
 *                              position.
 * @param backup_node_ptr       Another @ref ListNode to use as temporary storage.
 */
#define list_for_each_safe_from(cursor_node_ptr, backup_node_ptr) \
    for ( \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL \
    )

/**
 * Continues iterating in reverse order over the @ref List, continuing FROM the current position, and is safe
 * against reassignment and/or removal of the @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref List in the loop's
 *          body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref ListNode to use as a loop cursor. Its value represents the current
 *                              position.
 * @param backup_node_ptr       Another @ref ListNode to use as temporary storage.
 */
#define list_for_each_safe_from_reverse(cursor_node_ptr, backup_node_ptr) \
    for ( \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->prev : NULL \
    )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIST_H */
