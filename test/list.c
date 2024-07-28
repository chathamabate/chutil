
#include "chutil/list.h"
#include "unity/unity_internals.h"
#include "unity/unity.h"

void al_test_simple(void) {
    uint32_t a, b;

    array_list_t *al = new_array_list(sizeof(uint32_t));

    TEST_ASSERT_EQUAL_size_t(0, al_len(al));

    a = 4;
    al_push(al, &a);
    TEST_ASSERT_EQUAL_size_t(1, al_len(al));
    TEST_ASSERT_GREATER_THAN_size_t(0, al_cap(al));
    
    al_get_copy(al, 0, &b);
    TEST_ASSERT_EQUAL_UINT32(a, b);

    al_pop(al, &b);
    TEST_ASSERT_EQUAL_UINT32(a, b);
    TEST_ASSERT_EQUAL_size_t(0, al_len(al));

    delete_array_list(al);
}

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} al_coord_t;

void al_test_big(void) {
    array_list_t *al = new_array_list(sizeof(al_coord_t));
    
    TEST_ASSERT_EQUAL_size_t(sizeof(al_coord_t), al_cell_size(al));  

    const size_t LEN = 17;
    for (size_t i = 0; i < LEN; i++) {
        al_coord_t c = {
            .x = i,
            .y = i * 2,
            .z = i * 3,
        };
        
        al_push(al, &c);
    }

    TEST_ASSERT_EQUAL_size_t(LEN, al_len(al));
    TEST_ASSERT_GREATER_THAN_size_t(LEN-1, al_cap(al));

    const al_coord_t NULL_COORD = {
        .x = 0, .y = 0, .z = 0
    };

    for (size_t i = 0; i < LEN; i++) {
        const al_coord_t *c_ptr = al_get(al, i);
        TEST_ASSERT_EQUAL_UINT32(i, c_ptr->x);
        TEST_ASSERT_EQUAL_UINT32(i*2, c_ptr->y);
        TEST_ASSERT_EQUAL_UINT32(i*3, c_ptr->z);

        if (i % 2 == 0) {
            al_set(al, i, &NULL_COORD);
        }
    }

    const size_t POP_AMT = 5;
    for (size_t i = LEN - 1; i >= LEN - POP_AMT; i--) {
        al_coord_t c;
        al_pop(al, &c);

        if (i % 2 == 0) {
            TEST_ASSERT_EQUAL_UINT32(0, c.x);
            TEST_ASSERT_EQUAL_UINT32(0, c.y);
            TEST_ASSERT_EQUAL_UINT32(0, c.z);
        } else {
            TEST_ASSERT_EQUAL_UINT32(i, c.x);
            TEST_ASSERT_EQUAL_UINT32(i*2, c.y);
            TEST_ASSERT_EQUAL_UINT32(i*3, c.z);
        }
    }

    TEST_ASSERT_EQUAL_size_t(LEN - POP_AMT, al_len(al));

    for (size_t i = 0; i < POP_AMT; i++) {
        al_push(al, &NULL_COORD);
    }

    TEST_ASSERT_EQUAL_size_t(LEN, al_len(al));

    delete_array_list(al);
}

void array_list_tests(void) {
    RUN_TEST(al_test_simple);
    RUN_TEST(al_test_big);
}
