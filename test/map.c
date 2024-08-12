
#include "chutil/map.h"
#include "chutil/debug.h"
#include <stdio.h>

#include "unity/unity.h"
#include "unity/unity_internals.h"

static bool u64_eq_f(const uint64_t *k1, const uint64_t *k2) {
    return *k1 == *k2;
}

static uint32_t u64_hash_f(const uint64_t *k) {
    return (uint32_t)(((((*k) + 42934191239) * 3) + 12312388491) * 5);
}

static void test_hm_construct_and_destruct(void) {
    hash_map_t *hm = new_hash_map(sizeof(uint64_t), sizeof(uint64_t),
            (hash_map_hash_ft)u64_hash_f, (hash_map_eq_ft)u64_eq_f);
    TEST_ASSERT_NOT_NULL(hm);
    delete_hash_map(hm);
}

static void test_hm_put_and_get(void) {
    hash_map_t *hm = new_hash_map(sizeof(uint64_t), sizeof(uint64_t),
            (hash_map_hash_ft)u64_hash_f, (hash_map_eq_ft)u64_eq_f);

    const uint64_t NUM_KEYS = 80;
    uint64_t key, in_val, out_val;
    uint64_t *out_ptr;

    for (key = 0; key < NUM_KEYS; key++) {
        in_val = key * 5;

        hm_put(hm, &key, &in_val);
        
        out_ptr = hm_get(hm, &key);
        TEST_ASSERT_NOT_NULL(out_ptr);

        out_val = *out_ptr;
        TEST_ASSERT_EQUAL_UINT64(in_val, out_val);
    }

    delete_hash_map(hm);
}

static void test_hm_put_and_remove(void) {
    hash_map_t *hm = new_hash_map(sizeof(uint64_t), sizeof(uint64_t),
            (hash_map_hash_ft)u64_hash_f, (hash_map_eq_ft)u64_eq_f);

    const uint64_t NUM_KEYS = 50;
    uint64_t key, in_val, out_val;

    for (key = 0; key < NUM_KEYS; key++) {
        in_val = key * key;
        hm_put(hm, &key, &in_val);
    }

    for (key = 0; key < NUM_KEYS; key += 2) {
        TEST_ASSERT_TRUE(hm_contains(hm, &key));
        hm_remove(hm, &key);
        TEST_ASSERT_FALSE(hm_contains(hm, &key));
    }

    TEST_ASSERT_EQUAL_size_t(NUM_KEYS / 2, hm_num_keys(hm));

    for (key = 1; key < NUM_KEYS; key += 2) {
        TEST_ASSERT_TRUE(hm_get_copy(hm, &key, &out_val));
        TEST_ASSERT_EQUAL_UINT64(key * key, out_val);
    }

    delete_hash_map(hm);
}

typedef struct {
    uint64_t x;
    uint64_t y;
    uint64_t z;
} test_coord_t;

bool tct_eq_f(const test_coord_t *tct1, const test_coord_t *tct2) {
    return tct1->x == tct2->x &&
        tct1->y == tct2->y &&
        tct1->z == tct2->z;
}

uint32_t tct_hash_f(const test_coord_t *t) {
    return u64_hash_f(&(t->x)) + u64_hash_f(&(t->y)) + u64_hash_f(&(t->z));  
}

static void test_hm_put_big_key(void) {
    hash_map_t *hm = new_hash_map(sizeof(test_coord_t), sizeof(uint64_t), 
            (hash_map_hash_ft)tct_hash_f, (hash_map_eq_ft)tct_eq_f);

    // Here we are just testing using a larger key size.
    // Value size shouldn't really change anything special.

    const size_t NUM_KEYS = 20;
    test_coord_t tct;
    uint64_t in_val, out_val;

    for (size_t i = 0; i < NUM_KEYS; i++) {
        tct.x = i; tct.y = i; tct.z = 2 * i;
        in_val = i * 5;

        hm_put(hm, &tct, &in_val);
    }

    for (size_t i = 0; i < NUM_KEYS; i++) {
        tct.x = i; tct.y = i; tct.z = 2 * i;
        TEST_ASSERT_TRUE(hm_get_copy(hm, &tct, &out_val)); 
        TEST_ASSERT_EQUAL_UINT64(i * 5, out_val);
    }

    delete_hash_map(hm);
}


static bool u8_eq_f(const uint8_t *k1, const uint8_t *k2) {
    return *k1 == *k2;
}

static uint32_t u8_hash_f(const uint8_t *k) {
    return (uint32_t)(((((*k) + 5) * 3) + 31) * 5);
}

static void test_hm_iterator(void) {
    hash_map_t *hm = new_hash_map(sizeof(uint8_t), sizeof(uint32_t),
        (hash_map_hash_ft)u8_hash_f, (hash_map_eq_ft)u8_eq_f);

    // Confirm that all keys are found, and that their values match up. 

    const size_t NUM_KEYS = 255; // not using all possibilities to make live easy.
    bool *seen = safe_malloc(sizeof(bool) * NUM_KEYS);

    uint8_t key;
    uint32_t val;

    for (key = 0; key < NUM_KEYS; key++) {
        seen[key] = false;     
        val = key * 3;
        hm_put(hm, &key, &val);
    }

    const uint8_t *key_ptr;
    uint32_t *val_ptr;

    key_val_pair_t kvp;

    // Now let's iterate!
    hm_reset_iterator(hm);
    while ((kvp = hm_next_kvp(hm)) != HASH_MAP_EXHAUSTED) {
        key_ptr = kvp_key(hm, kvp);
        val_ptr = kvp_val(hm, kvp);

        TEST_ASSERT_NOT_NULL(key_ptr);
        TEST_ASSERT_NOT_NULL(val_ptr);

        TEST_ASSERT_EQUAL_UINT32(*key_ptr * 3, *val_ptr);
        
        TEST_ASSERT_FALSE(seen[*key_ptr]);
        seen[*key_ptr] = true;
    }
        
    safe_free(seen);
    delete_hash_map(hm);
}

void map_tests(void) {
    RUN_TEST(test_hm_construct_and_destruct); 
    RUN_TEST(test_hm_put_and_get);
    RUN_TEST(test_hm_put_and_remove);
    RUN_TEST(test_hm_put_big_key);
    RUN_TEST(test_hm_iterator);
}

