
#include <stdio.h>
#include "chjson/json_helpers.h"
#include "chutil/string.h"
#include "chjson/json.h"
#include "json_helper.h"
#include "json.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"

void setUp(void) {
    reset_malloc_count();
}

void tearDown(void) {
    TEST_ASSERT_EQUAL_UINT(0, get_malloc_count());
}

int main(void) {
    UNITY_BEGIN();
    json_tests();
    json_helpers_tests();
    return UNITY_END();
}
