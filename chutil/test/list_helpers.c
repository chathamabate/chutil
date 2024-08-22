
#include "list_helpers.h"
#include "chutil/list.h"
#include "chutil/list_helpers.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"

#include <stdbool.h>

// These tests will just use an array list as the 
// interior type.

static bool int_eq(const int *i1, const int *i2) {
    return *i1 == *i2;
}

static void test_l_equals_simple(void) {
    list_t *l1 = new_list(ARRAY_LIST_IMPL, sizeof(int));
    list_t *l2 = new_list(ARRAY_LIST_IMPL, sizeof(int));

    TEST_ASSERT_TRUE(l_equals(l1, l2, (list_cell_equals_ft)int_eq));

    int num = 5;
    l_push(l1, &num);
    TEST_ASSERT_FALSE(l_equals(l1, l2, (list_cell_equals_ft)int_eq));

    l_push(l2, &num);
    TEST_ASSERT_TRUE(l_equals(l1, l2, (list_cell_equals_ft)int_eq));

    num = 2;
    l_set(l1, 0, &num);
    TEST_ASSERT_FALSE(l_equals(l1, l2, (list_cell_equals_ft)int_eq));

    delete_list(l1);
    delete_list(l2);
}

static void test_l_equals_big(void) {
    list_t *l1 = new_list(ARRAY_LIST_IMPL, sizeof(int));
    list_t *l2 = new_list(ARRAY_LIST_IMPL, sizeof(int));

    for (int i = 0; i < 100; i++) {
        l_push(l1, &i);
        l_push(l2, &i);
    }

    TEST_ASSERT_TRUE(l_equals(l1, l2, (list_cell_equals_ft)int_eq));

    int num = 11;
    l_set(l2, 44, &num);

    TEST_ASSERT_FALSE(l_equals(l1, l2, (list_cell_equals_ft)int_eq));

    delete_list(l1);
    delete_list(l2);
}

void list_helpers_tests(void) {
    RUN_TEST(test_l_equals_simple);
    RUN_TEST(test_l_equals_big);
}
