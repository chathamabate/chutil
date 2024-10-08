
#include "chutil/heap.h"

#include "heap.h"
#include "unity/unity.h"
#include <stdint.h>
#include <stdlib.h>

static uint32_t u32_pf(const uint32_t *v) {
    return UINT32_MAX - *v;
}

// pop n elements off of an hp holding uint32_t's.
// Expect elements popped to be non-ascending. 
static void expect_sorted(heap_t *hp, size_t n) {
    uint32_t prev = UINT32_MAX; 
    uint32_t curr;

    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_TRUE(hp_pop(hp, &curr));
            //printf("prev: %u, curr: %u\n", prev, curr);
        TEST_ASSERT_TRUE(prev >= curr);
        prev = curr;
    }
}

static void push_random_seq(heap_t *hp, size_t n) {
    uint32_t val;
    for (size_t i = 0; i < n; i++) {
        val = rand() % UINT32_MAX;
        hp_push(hp, &val);
    }
}

// Just do a quick push and pop, nothing fancy.
static void test_hp_simple1(void) {
    heap_t *hp = new_heap(sizeof(uint32_t), (heap_priority_ft)u32_pf);
    TEST_ASSERT_NOT_NULL(hp);

    uint32_t in = 5;
    uint32_t out = 0;

    hp_push(hp, &in);
    TEST_ASSERT_EQUAL_size_t(1, hp_len(hp));

    uint32_t *root = hp_peek(hp);
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT32(in, *root);

    bool popped = hp_pop(hp, &out);
    TEST_ASSERT_TRUE(popped);

    TEST_ASSERT_EQUAL_UINT32(in, out);

    delete_heap(hp);
}

// Push a few values, and expect the correct order back.
#define TEST_HP_SIMPLE2_PUSHES 5
static void test_hp_simple2(void) {
    const uint32_t PUSHES[TEST_HP_SIMPLE2_PUSHES] = {
        3, 5, 1, 0, 5
    };

    heap_t *hp = new_heap(sizeof(uint32_t), (heap_priority_ft)u32_pf);

    for (size_t i = 0; i < TEST_HP_SIMPLE2_PUSHES; i++) {
        hp_push(hp, &(PUSHES[i]));
    }

    TEST_ASSERT_EQUAL_size_t(TEST_HP_SIMPLE2_PUSHES, hp_len(hp));
    expect_sorted(hp, TEST_HP_SIMPLE2_PUSHES);

    TEST_ASSERT_EQUAL_size_t(0, hp_len(hp));

    delete_heap(hp);
}

// For this one we will add a lot, remove a few,
// add a lot more, than remove one final time.
static void test_hp_big(void) {
    srand(2831823182);

    heap_t *hp = new_heap(sizeof(uint32_t), (heap_priority_ft)u32_pf);

    const size_t add1_n = 100;
    const size_t sub1_n = 40;
    const size_t add2_n = 200;

    push_random_seq(hp, add1_n);
    expect_sorted(hp, sub1_n);
    push_random_seq(hp, add2_n);
    expect_sorted(hp, add1_n + add2_n - sub1_n);
    TEST_ASSERT_EQUAL_size_t(0, hp_len(hp));

    delete_heap(hp);
}

static void test_hp_re_heap(void) {
    heap_t *hp = new_heap(sizeof(uint32_t), (heap_priority_ft)u32_pf);

    const size_t n = 20;
    for (size_t i = 0; i < n; i++) {
        hp_push(hp, &i);         
    }

    // We are going to set all odd values in the heap to 0.
    hp_reset_iterator(hp);
    uint32_t *val;
    while ((val = (uint32_t *)hp_next(hp))) {
        if (*val % 2 == 1) {
            *val = 0;
        }
    }

    const size_t evens = (n / 2) + (n % 2);
    const size_t high_even = (n - 2) + (n % 2);

    hp_re_heap(hp);

    uint32_t out;
    for (size_t i = 0; i < evens; i++) {
        TEST_ASSERT_TRUE(hp_pop(hp, &out)); 
        TEST_ASSERT_EQUAL_UINT32(high_even - (2*i), out);
    }

    // Rest should be 0.
    for (size_t i = 0; i < n - evens; i++) {
        TEST_ASSERT_TRUE(hp_pop(hp, &out)); 
        TEST_ASSERT_EQUAL_UINT32(0, out);
    }

    delete_heap(hp);
}

// This is for testing an awkward cell size!

static uint32_t str_pf(const char *s) {
    return (uint32_t)(s[0]);
}

#define TEST_HP_STR_CAP 10
static void test_hp_str(void) {
    heap_t *hp = new_heap(sizeof(char) * TEST_HP_STR_CAP, (heap_priority_ft)str_pf);

    hp_push(hp, "hello");
    hp_push(hp, "meh");
    hp_push(hp, "ah");
    hp_push(hp, "zebra");

    const char *exp_out[4] = {
        "ah", "hello", "meh", "zebra"
    };

    char out_buf[TEST_HP_STR_CAP];
    size_t i = 0;
    while (hp_pop(hp, out_buf)) {
        TEST_ASSERT_EQUAL_STRING(exp_out[i], out_buf);
        i++;
    }

    delete_heap(hp);
}

void heap_tests(void) {
    RUN_TEST(test_hp_simple1); 
    RUN_TEST(test_hp_simple2);
    RUN_TEST(test_hp_big);
    RUN_TEST(test_hp_re_heap);
    RUN_TEST(test_hp_str);
}

