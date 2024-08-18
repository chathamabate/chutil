
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

void heap_tests(void) {
    RUN_TEST(test_hp_simple1); 
    RUN_TEST(test_hp_simple2);
    RUN_TEST(test_hp_big);
}

