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
 * @file    queue.h
 * @brief   QUEUE (FIFO)
 *
 * Embed one or more @ref QueueNode's into your struct to make it a potential node in one or more queues. The
 * @ref Queue structure keeps track of a chain of @ref QueueNode's. A @ref Queue structure MUST be initialized
 * before it is used. A @ref QueueNode structure does NOT need to be initialized before it is used.  A
 * @ref QueueNode should belong to at most ONE @ref Queue.
 *
 * Example:
 *          struct Object {
 *              int val;
 *              QueueNode n;
 *          };
 *
 *          int main(void) {
 *              struct Object obj;
 *              Queue queue;
 *              int copy_val;
 *
 *              queue_init(&queue);
 *              queue_push(&queue, &obj.n);
 *
 *              obj.val = 5;
 *              copy_val = queue_entry(queue_peek(&queue), struct Object, n)->val;
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
 *      -   typedef struct Queue Queue
 *      -   typedef struct QueueNode QueueNode
 *
 *      ====  FUNCTIONS  ====
 *      Initializers:
 *          -   queue_init
 *      Properties:
 *          -   queue_peek
 *          -   queue_size
 *          -   queue_empty
 *      Insertion:
 *          -   queue_push
 *      Removal:
 *          -   queue_pop
 *          -   queue_remove_all
 *
 *      ====  MACROS  ====
 *      Constants:
 *          -   QUEUE_POISON_NEXT
 *      Convenient Node Initializer:
 *          -   QUEUE_NODE_INIT
 *      Properties:
 *          -   queue_entry
 *      Traversal:
 *          -   queue_for_each
 *          -   queue_for_each_safe
 */

#ifndef QUEUE_H
#define QUEUE_H

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
struct Queue;
struct QueueNode;

/* Struct typedef's. */
typedef struct Queue Queue;
typedef struct QueueNode QueueNode;

/**
 * Represents a queue.
 */
struct Queue {
    QueueNode *head;
    QueueNode *tail;
    size_t size;
};

/**
 * Represents a node in a @ref Queue. Embed this into your structure to make it a node.
 */
struct QueueNode {
    QueueNode *next;
};

/* ========================================================================================================
 *
 *                                               PROTOTYPES
 *
 * ======================================================================================================== */

/**
 * Initializes/resets the @ref queue.
 *
 * Requirements:
 *      -   @ref queue != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref Queue to be initialized/reset.
 */
void queue_init(Queue *queue);

/**
 * Returns the front @ref QueueNode of the @ref queue.
 *
 * Requirements:
 *      -   @ref queue != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref Queue whose "head" member will be returned.
 * @return                      @ref queue->head.
 */
QueueNode* queue_peek(const Queue *queue);

/**
 * Returns the size of the @ref queue.
 *
 * Requirements:
 *      -   @ref queue != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref Queue whose "size" member will be returned.
 * @return                      @ref queue->size.
 */
size_t queue_size(const Queue *queue);

/**
 * Returns whether or not the @ref queue is empty (i.e. @ref queue->size == 0).
 *
 * Requirements:
 *      -   @ref queue != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref Queue whose "size" member will be used to determine if it is empty.
 * @return                      Whether or not the @ref queue is empty (i.e. @ref queue->size == 0).
 */
int queue_empty(const Queue *queue);

/**
 * Pushes the @ref node into the back of the @ref queue.
 *
 * Requirements:
 *      -   @ref queue != NULL
 *      -   @ref node != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref Queue to be operated on.
 * @param node                  The @ref QueueNode to be inserted.
 */
void queue_push(Queue *queue, QueueNode *node);

/**
 * Pops off the front @ref QueueNode of the @ref queue AND returns it. If the @ref queue is empty, this
 * function simply returns NULL.
 *
 * Requirements:
 *      -   @ref queue != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref Queue to be operated on.
 * @return                      The removed front @ref QueueNode.
 */
QueueNode* queue_pop(Queue *queue);

/**
 * Removes all @ref QueueNode's from the @ref queue. If the @ref queue is empty, this function simply returns.
 *
 * Requirements:
 *      -   @ref queue != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param queue                 The @ref queue to be operated on.
 */
void queue_remove_all(Queue *queue);

/* ========================================================================================================
 *
 *                                                 MACROS
 *
 * ======================================================================================================== */

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "next" member of the
 * front/back @ref QueueNode in a chain of removed @ref QueueNode's or a single removed @ref QueueNode. Useful
 * for identifying bugs.
 */
#define QUEUE_POISON_NEXT ((QueueNode*) 0x100)

/**
 * Initializing a @ref QueueNode before it is used is NOT required. This macro is simply for allowing you to
 * initialize a struct (containing one or more @ref QueueNode's) with an initializer-list conveniently.
 */
#define QUEUE_NODE_INIT { QUEUE_POISON_NEXT }

/**
 * Obtains the pointer to the struct for this entry.
 *
 * Requirements:
 *      -   @ref node_ptr != NULL
 *
 * @param node_ptr              The pointer to the @ref QueueNode in the struct.
 * @param type                  The type of the struct the @ref QueueNode is embedded in.
 * @param member                The name of the @ref QueueNode in the struct.
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define queue_entry(node_ptr, type, member) \
        ({ \
            const typeof(((type*)0)->member) *__mptr = (node_ptr); \
            (type*) ((char*)__mptr - offsetof(type, member)); \
        })
#else
    #define queue_entry(node_ptr, type, member) \
        ( \
            (type*) ((char*)(node_ptr) - offsetof(type, member)) \
        )
#endif

/**
 * Iterates over the @ref Queue from front to back.
 *
 * Requirements:
 *      -   @ref queue_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref Queue in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref QueueNode to use as a loop cursor.
 * @param queue_ptr             The pointer to a @ref Queue that will be iterated over.
 */
#define queue_for_each(cursor_node_ptr, queue_ptr) \
    for ( \
        cursor_node_ptr = (queue_ptr)->head; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->next \
    )

/**
 * Iterates over the @ref Queue from front to back, and is safe against reassignment and/or removal of the
 * @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref queue_ptr != NULL
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref Queue in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref QueueNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref QueueNode to use as temporary storage.
 * @param queue_ptr             The pointer to a @ref Queue that will be iterated over.
 */
#define queue_for_each_safe(cursor_node_ptr, backup_node_ptr, queue_ptr) \
    for ( \
        cursor_node_ptr = (queue_ptr)->head, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL; \
        \
        cursor_node_ptr; \
        \
        cursor_node_ptr = backup_node_ptr, \
        backup_node_ptr = cursor_node_ptr ? cursor_node_ptr->next : NULL \
    )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* QUEUE_H */
