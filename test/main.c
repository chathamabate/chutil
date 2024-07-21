#include <stdio.h>

#include "unity/unity_internals.h"
#include <unity/unity.h>

void setUp(void) {

}
void tearDown(void) {

}

void test_Func(void) {
    TEST_ASSERT_EQUAL_INT32(5, 5);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Func);
    return UNITY_END();
}
