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
 * @file    testing_framework.h
 *
 * Tools for testing the data structures and algorithms.
 */

#ifndef TESTING_FRAMEWORK_H__
#define TESTING_FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>
#include <stdio.h>

/* ========================================================================================================
 *
 *                                                  TYPES
 *
 * ======================================================================================================== */

/**
 * Useful for creating an array of test functions.
 */
typedef void (*TestFunc)(void);

/* ========================================================================================================
 *
 *                                               PROTOTYPES
 *
 * ======================================================================================================== */

/**
 * Runs each test function in @param test_funcs and does internal checks prior to calling each function. Upon
 * completion of all tests, outputs message. An optional function can be called before every test function
 * call.
 *
 * @param test_funcs            The array of test functions.
 * @param nitems                The number of items in the array.
 * @param msg                   The message to be outputed upon passing all tests.
 * @param callback              This function is called before every test function call if non-NULL.
 */
static void run_tests(TestFunc *test_funcs, size_t nitems, const char *msg, void (*callback)(void));

/* ========================================================================================================
 *
 *                                          FUNCTION DEFINITIONS
 *
 * ======================================================================================================== */

static void run_tests(TestFunc *test_funcs, size_t nitems, const char *msg, void (*callback)(void)) {
    size_t i;
    for (i = 0; i < nitems; ++i) {
        if (callback) {
            callback();
        }
        test_funcs[i]();
    }
    printf("\nPASSED: %s\n\n", msg);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TESTING_FRAMEWORK_H__ */
