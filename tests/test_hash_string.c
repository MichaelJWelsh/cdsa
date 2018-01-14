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
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "testing_framework.h"
#include "../src/hashtable.h"

/* Test header guard. */
#include "../src/hash_string.h"
#include "../src/hash_string.h"

/* ========================================================================================================
 *
 *                                             TESTING UTILITIES
 *
 * ======================================================================================================== */

#define ASSERT_HASH_STRING(string, hashcode) \
    assert(hash_string(string) == hashcode##ul)


static size_t mirror_hash_string(const char *str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static int dummy_equal_func(const void *key, const HashTableNode *node) {
    return (const HashTableNode*)key == node;
}

/* ========================================================================================================
 *
 *                                             TESTING FUNCTIONS
 *
 * ======================================================================================================== */

void test_hashtable_compatibility(void) {
    HashTable hashtable;
    HashTableNode *bkt_arr[1];

    hashtable_init(&hashtable, bkt_arr, 1, hash_string, dummy_equal_func, NULL, NULL);
}

void test_hash_string(void) {
    size_t counter;

    ASSERT_HASH_STRING("abcde", 210706217108);
    ASSERT_HASH_STRING("12abc12", 229395199025009);
    ASSERT_HASH_STRING("asdfjkl;", 7572171320972735);
    ASSERT_HASH_STRING("qwertyuiopasdfghjkl;lkjhgfdsapoiuytrewqqwerty;;;", 16245301107329722347);
    ASSERT_HASH_STRING("1", 177622);
    ASSERT_HASH_STRING("a", 177670);
    ASSERT_HASH_STRING("A", 177638);
    ASSERT_HASH_STRING("wieruasl!", 249912170171059506);
    ASSERT_HASH_STRING("BADADASDADADSADFADF", 15974083569823714108);
    ASSERT_HASH_STRING("[]cxyz", 6953064445163);
    ASSERT_HASH_STRING("send_help college_debt_high btc_addr_below:", 10862613210741622356);
    ASSERT_HASH_STRING("13oQbQZFWqdk1GHvaquNm3xhish3xQW7z4", 8489840333573805946);

    for (counter = 0; counter < 50000; ++counter) {
        char str[500];
        size_t i;

        for (i = 0; i < 499; ++i) {
            str[i] = (rand() % 43 == 0) ? ('\0') : ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz;:[]{}-=_+!@#$^&*()~`"[rand() % 73]);
        }
        str[499] = '\0';

        assert(hash_string(str) == mirror_hash_string(str));
    }
}

TestFunc test_funcs[] = {
    test_hashtable_compatibility,
    test_hash_string
};

int main(int argc, char *argv[]) {
    char msg[100] = "hash_string(...) ";
    assert(argc == 2);
    strcat(msg, argv[1]);

    assert(sizeof(test_funcs) / sizeof(TestFunc) == 2);
    run_tests(test_funcs, sizeof(test_funcs) / sizeof(TestFunc), msg, NULL);

    return 0;
}
