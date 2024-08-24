#include <stdio.h>

#include "chutil/list.h"
#include "unity/unity_internals.h"
#include "unity/unity.h"

#include "chutil/debug.h"

#include "map.h"
#include "list.h"
#include "heap.h"
#include "_string.h"
#include "list_helpers.h"
#include "stream.h"

void setUp(void) {
    reset_malloc_count();
}

void tearDown(void) {
    TEST_ASSERT_EQUAL_UINT(0, get_malloc_count());
}


int main(void) {
    UNITY_BEGIN();
    list_tests();
    list_helpers_tests();
    map_tests();
    heap_tests();
    string_tests(); 
    stream_tests();
    return UNITY_END();
}
