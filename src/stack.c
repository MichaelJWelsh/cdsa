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

#include "stack.h"

void stack_init(Stack *stack) {
    assert(stack);

    stack->tail = NULL;
    stack->size = 0;
}

StackNode* stack_peek(const Stack *stack) {
    assert(stack);

    return stack->tail;
}

size_t stack_size(const Stack *stack) {
    assert(stack);

    return stack->size;
}

int stack_empty(const Stack *stack) {
    assert(stack);

    return stack->size == 0;
}

void stack_push(Stack *stack, StackNode *node) {
    assert(stack && node);

    node->prev = stack->tail;
    stack->tail = node;

    ++stack->size;
}

StackNode* stack_pop(Stack *stack) {
    StackNode *n;

    assert(stack);

    n = stack->tail;

    if (!n) {
        return NULL;
    }

    stack->tail = n->prev;

    n->prev = STACK_POISON_PREV;

    --stack->size;

    return n;
}

void stack_remove_all(Stack *stack) {
    assert(stack);

    if (stack->tail) {
        stack->tail->prev = STACK_POISON_PREV;
    }

    stack->tail = NULL;
    stack->size = 0;
}
