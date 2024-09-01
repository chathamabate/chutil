
#include <stdio.h>
#include "chjson/json_helpers.h"
#include "chjson/parser.h"
#include "chutil/list.h"
#include "chutil/map.h"
#include "chutil/stream.h"
#include "chutil/string.h"
#include "chjson/json.h"
#include "json_helper.h"
#include "json.h"
#include "parser.h"
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
    parser_tests();
    return UNITY_END();
}
