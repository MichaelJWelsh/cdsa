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
 * @file    alloc.h
 * @brief   Tools for testing the allocation/deallocation operations used in the data structures and
 *          algorithms.
 */

#ifndef ALLOC_H__
#define ALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>

/**
 * Useful for creating an array of test functions.
 */
typedef void (*TestFunc)(void);

/**
 * Runs each test function in @param test_funcs and does internal checks prior to calling each function.
 *
 * @param test_funcs            The array of test functions.
 * @param nitems                The number of items in the array.
 */
void run_tests(TestFunc *test_funcs, size_t nitems);

/**
 * Always returns NULL.
 *
 * @param size                  This parameter goes unused.
 * @return                      NULL.
 */
void* test_bad_malloc(size_t size);

/**
 * Always returns NULL.
 *
 * @param nitems                This parameter goes unused.
 * @param size                  This parameter goes unused.
 * @return                      NULL.
 */
void* test_bad_calloc(size_t nitems, size_t size);

/**
 * Always returns NULL.
 *
 * @param ptr                   This parameter goes unused.
 * @param size                  This parameter goes unused.
 * @return                      NULL.
 */
void* test_bad_realloc(void *ptr, size_t size);

/**
 * Keeps a record internally of all memory allocated with this test function. Uses malloc to actually do the
 * allocation. Assures that all allocated bytes do not equal zero. If malloc returns NULL, aborts program.
 *
 * @param size                  Passed to malloc.
 * @return                      malloc(size). NEVER NULL.
 */
void* test_malloc(size_t size);

/**
 * Keeps a record internally of all memory allocated with this test function. Uses calloc to actually do the
 * allocation. If malloc returns NULL, aborts program.
 *
 * @param nitems                Passed to calloc.
 * @param size                  Passed to calloc.
 * @return                      calloc(nitems, size). NEVER NULL.
 */
void* test_calloc(size_t nitems, size_t size);

/**
 * Keeps a record internally of all memory allocated with this test function. Uses calloc to actually do the
 * allocation. If malloc returns NULL, aborts program.
 *
 * @param ptr                   Passed to realloc.
 * @param size                  Passed to realloc.
 * @return                      realloc(ptr, size). NEVER NULL.
 */
void* test_realloc(void *ptr, size_t size);

/**
 * Checks that the resource to be freed has been allocated by one of these test functions, then removes it
 * from the internal record of resources currently allocated.
 * 
 * @param ptr                   The pointer to the resource that will be freed.
 */
void test_free(void *ptr);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ALLOC_H__
