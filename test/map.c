
#include "chutil/map.h"
#include <stdio.h>

#include "unity/unity.h"
#include "unity/unity_internals.h"

static bool u64_eq_f(const uint64_t *k1, const uint64_t *k2) {
    return *k1 == *k2;
}

static bool u64_hash_f(const uint64_t *k) {
    return ((((*k) + 42934191239) * 3) + 12312388491) * 5;
}

static void hm_construct_and_destruct(void) {
    hash_map_t *hm = new_hash_map(sizeof(uint64_t), sizeof(uint64_t),
            (hash_map_hash_ft)u64_hash_f, (hash_map_eq_ft)u64_eq_f);
    TEST_ASSERT_NOT_NULL(hm);
    delete_hash_map(hm);
}


void map_tests(void) {
    RUN_TEST(hm_construct_and_destruct); 
}

