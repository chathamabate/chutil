
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
#include "chsys/sys.h"


void setUp(void) {
    sys_reset_malloc_count(true);
}

void tearDown(void) {
    TEST_ASSERT_EQUAL_size_t(0, sys_get_malloc_count(true));
}

int main(void) {
    sys_init();

    UNITY_BEGIN();
    json_tests();
    json_helpers_tests();
    parser_tests();
    safe_exit(true, UNITY_END());
}
