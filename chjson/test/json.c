

#include "unity/unity.h"
#include "unity/unity_internals.h"

#include "chjson/json.h"

static void test_json_delete_simple(void) {
    json_t *json;

    json = new_json_number(1.2);
    delete_json(json);

    json = new_json_boolean(true);
    delete_json(json);

    json = json_null();
    delete_json(json);

    json = new_json_string(new_string_from_literal("hello"));
    delete_json(json);
}

static void test_json_delete_complex(void) {
    json_t *json;

    json = new_json_list_from_eles(
        new_json_number(1.2),
        new_json_string(new_string_from_literal("Hello")),
        json_null()
    );

    delete_json(json);

    json = new_json_object_from_kvps(
        new_string_from_literal("K1"), new_json_number(1.2),
        new_string_from_literal("K2"), new_json_list_from_eles(
            new_json_boolean(true),
            new_json_string(new_string_from_cstr("hello"))
        )
    );

    delete_json(json);
}

void json_tests(void) {
    RUN_TEST(test_json_delete_simple);
    RUN_TEST(test_json_delete_complex);
}



