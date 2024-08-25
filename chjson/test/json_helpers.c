
#include "chjson/json.h"
#include "chjson/json_helpers.h"

#include "json_helper.h"

#include "unity/unity.h"
#include "unity/unity_internals.h"

static void test_json_to_string(void) {
    json_t *json = new_json_list_from_eles(
        new_json_number(1.2),
        new_json_string(new_string_from_literal("Hello")),
        new_json_boolean(true),
        new_json_object(),
        json_null(),
        new_json_list_from_eles(new_json_number(1.0))
    );

    string_t *s = json_to_string(json, false);

    TEST_ASSERT_EQUAL_STRING("[1.2,\"Hello\",true,{},null,[1]]", s_get_cstr(s));
    
    delete_string(s);
    delete_json(json);
}

void json_helpers_tests(void) {
    RUN_TEST(test_json_to_string);
}
