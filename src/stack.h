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
 * @file    stack.h
 * @brief   STACK (LIFO)
 *
 * Embed one or more @ref StackNode's into your struct to make it a potential node in one or more stacks. The
 * @ref Stack structure keeps track of a chain of @ref StackNode's. A @ref Stack structure MUST be initialized
 * before it is used. A @ref StackNode structure does NOT need to be initialized before it is used.  A
 * @ref StackNode should belong to at most ONE @ref Stack.
 *
 * Example:
 *          struct Object {
 *              int val;
 *              StackNode n;
 *          };
 *
 *          int main(void) {
 *              struct Object obj;
 *              Stack stack;
 *              int copy_val;
 *
 *              stack_init(&stack);
 *              stack_push(&stack, &obj.n);
 *
 *              obj.val = 5;
 *              copy_val = stack_entry(stack_peek(&stack), struct Object, n)->val;
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
 *      -   typedef struct Stack Stack
 *      -   typedef struct StackNode StackNode
 *
 *      ====  FUNCTIONS  ====
 *      Initializers:
 *          -   stack_init
 *      Properties:
 *          -   stack_peek
 *          -   stack_size
 *          -   stack_empty
 *      Insertion:
 *          -   stack_push
 *      Removal:
 *          -   stack_pop
 *          -   stack_remove_all
 *
 *      ====  MACROS  ====
 *      Constants:
 *          -   STACK_POISON_PREV
 *      Convenient Node Initializer:
 *          -   STACK_NODE_INIT
 *      Properties:
 *          -   stack_entry
 *      Traversal:
 *          -   stack_for_each
 *          -   stack_for_each_safe
 */

#ifndef STACK_H
#define STACK_H

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
struct Stack;
struct StackNode;

/* Struct typedef's. */
typedef struct Stack Stack;
typedef struct StackNode StackNode;

/**
 * Represents a stack.
 */
struct Stack {
    StackNode *tail;
    size_t size;
};

/**
 * Represents a node in a @ref Stack. Embed this into your structure to make it a node.
 */
struct StackNode {
    StackNode *prev;
};

/* ========================================================================================================
 *
 *                                               PROTOTYPES
 *
 * ======================================================================================================== */

/**
 * Initializes/resets the @ref stack.
 *
 * Requirements:
 *      -   @ref stack != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref Stack to be initialized/reset.
 */
void stack_init(Stack *stack);

/**
 * Returns the top @ref StackNode of the @ref stack.
 *
 * Requirements:
 *      -   @ref stack != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref Stack whose "tail" member will be returned.
 * @return                      @ref stack->tail.
 */
StackNode* stack_peek(const Stack *stack);

/**
 * Returns the size of the @ref stack.
 *
 * Requirements:
 *      -   @ref stack != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref Stack whose "size" member will be returned.
 * @return                      @ref stack->size.
 */
size_t stack_size(const Stack *stack);

/**
 * Returns whether or not the @ref stack is empty (i.e. @ref stack->size == 0).
 *
 * Requirements:
 *      -   @ref stack != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref Stack whose "size" member will be used to determine if it is empty.
 * @return                      Whether or not the @ref stack is empty (i.e. @ref stack->size == 0).
 */
int stack_empty(const Stack *stack);

/**
 * Pushes the @ref node into the top of the @ref stack.
 *
 * Requirements:
 *      -   @ref stack != NULL
 *      -   @ref node != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref Stack to be operated on.
 * @param node                  The @ref StackNode to be inserted.
 */
void stack_push(Stack *stack, StackNode *node);

/**
 * Pops off the top @ref StackNode of the @ref stack AND returns it. If the @ref stack is empty, this function
 * simply returns NULL.
 *
 * Requirements:
 *      -   @ref stack != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref Stack to be operated on.
 * @return                      The removed top @ref StackNode.
 */
StackNode* stack_pop(Stack *stack);

/**
 * Removes all @ref StackNode's from the @ref stack. If the @ref stack is empty, this function simply returns.
 *
 * Requirements:
 *      -   @ref stack != NULL
 *
 * Time complexity:
 *      -   O(1)
 *
 * @param stack                 The @ref stack to be operated on.
 */
void stack_remove_all(Stack *stack);

/* ========================================================================================================
 *
 *                                                 MACROS
 *
 * ======================================================================================================== */

/**
 * Non-NULL pointer that will result in page faults under normal circumstances. Is the "prev" member of the
 * top @ref StackNode in a chain of removed @ref StackNode's or a single removed @ref StackNode. Useful for
 * identifying bugs.
 */
#define STACK_POISON_PREV ((StackNode*) 0x100)

/**
 * Initializing a @ref StackNode before it is used is NOT required. This macro is simply for allowing you to
 * initialize a struct (containing one or more @ref StackNode's) with an initializer-list conveniently.
 */
#define STACK_NODE_INIT { STACK_POISON_PREV }

/**
 * Obtains the pointer to the struct for this entry.
 *
 * Requirements:
 *      -   @ref node_ptr != NULL
 *
 * @param node_ptr              The pointer to the @ref StackNode in the struct.
 * @param type                  The type of the struct the @ref StackNode is embedded in.
 * @param member                The name of the @ref StackNode in the struct.
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define stack_entry(node_ptr, type, member) \
        ({ \
            const typeof(((type*)0)->member) *__mptr = (node_ptr); \
            (type*) ((char*)__mptr - offsetof(type, member)); \
        })
#else
    #define stack_entry(node_ptr, type, member) \
        ( \
            (type*) ((char*)(node_ptr) - offsetof(type, member)) \
        )
#endif

/**
 * Iterates over the @ref Stack from top to bottom.
 *
 * Requirements:
 *      -   @ref stack_ptr != NULL
 *      -   The @ref cursor_node_ptr is neither reassigned nor removed from its associated @ref Stack in the
 *          loop's body.
 *
 * @param cursor_node_ptr       The @ref StackNode to use as a loop cursor.
 * @param stack_ptr             The pointer to a @ref Stack that will be iterated over.
 */
#define stack_for_each(cursor_node_ptr, stack_ptr) \
    for ( \
        cursor_node_ptr = (stack_ptr)->tail; \
        cursor_node_ptr; \
        cursor_node_ptr = cursor_node_ptr->prev \
    )

/**
 * Iterates over the @ref Stack from top to bottom, and is safe against reassignment and/or removal of the
 * @ref cursor_node_ptr.
 *
 * Requirements:
 *      -   @ref stack_ptr != NULL
 *      -   @ref backup_node_ptr is neither reassigned nor removed from its associated @ref Stack in the
 *          loop's body.
 *      -   @ref backup_node_ptr and @ref cursor_node_ptr are not the same variable.
 *
 * @param cursor_node_ptr       The @ref StackNode to use as a loop cursor.
 * @param backup_node_ptr       Another @ref StackNode to use as temporary storage.
 * @param stack_ptr             The pointer to a @ref Stack that will be iterated over.
 */
#define stack_for_each_safe(cursor_node_ptr, backup_node_ptr, stack_ptr) \
    for ( \
        cursor_node_ptr = (stack_ptr)->tail, \
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

#endif /* STACK_H */
