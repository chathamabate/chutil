
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

typedef struct _test_json_equality_case_t {
    bool expect_equals;
    json_t *json1;
    json_t *json2;
} test_json_equality_case_t;

static void test_json_equals(void) {
    test_json_equality_case_t cases[] = {
        {
            .expect_equals = false,
            .json1 = new_json_boolean(false),
            .json2 = new_json_boolean(true),
        },
        {
            .expect_equals = false,
            .json1 = new_json_boolean(false),
            .json2 = new_json_number(1.2),
        },
        {
            .expect_equals = true,
            .json1 = new_json_number(1.2),
            .json2 = new_json_number(1.2),
        },
        {
            .expect_equals = true,
            .json1 = new_json_string(new_string_from_cstr("hello")),
            .json2 = new_json_string(new_string_from_literal("hello"))
        },
        {
            .expect_equals = false,
            .json1 = new_json_string(new_string_from_cstr("hello1")),
            .json2 = new_json_string(new_string_from_literal("hello"))
        },
        {
            .expect_equals = true,
            .json1 = new_json_list_from_eles(
                new_json_number(1),
                new_json_number(2)
            ),
            .json2 = new_json_list_from_eles(
                new_json_number(1),
                new_json_number(2)
            ),
        },
        {
            .expect_equals = false,
            .json1 = new_json_list_from_eles(
                new_json_number(1),
                new_json_number(2)
            ),
            .json2 = new_json_list_from_eles(
                new_json_number(2),
                new_json_number(1)
            ),
        },
        {
            .expect_equals = true,
            .json1 = new_json_object_from_kvps(
                new_string_from_literal("K1"), new_json_number(1),
                new_string_from_literal("K2"), new_json_number(2)
            ),
            .json2 = new_json_object_from_kvps(
                new_string_from_literal("K2"), new_json_number(2),
                new_string_from_literal("K1"), new_json_number(1)
            )
        },
        {
            .expect_equals = true,
            .json1 = new_json_object_from_kvps(
                new_string_from_literal("K1"), new_json_number(1),
                new_string_from_literal("K2"), new_json_list_from_eles(
                    new_json_boolean(true),
                    json_null()
                )
            ),
            .json2 = new_json_object_from_kvps(
                new_string_from_literal("K2"), new_json_list_from_eles(
                    new_json_boolean(true),
                    json_null()
                ),
                new_string_from_literal("K1"), new_json_number(1)
            ),
        },
        {
            .expect_equals = false,
            .json1 = new_json_object_from_kvps(
                new_string_from_literal("K1"), new_json_number(1),
                new_string_from_literal("K2"), new_json_list_from_eles(
                    new_json_boolean(true),
                    json_null()
                )
            ),
            .json2 = new_json_object_from_kvps(
                new_string_from_literal("K2"), new_json_list_from_eles(
                    json_null(),
                    new_json_boolean(true)
                ),
                new_string_from_literal("K1"), new_json_number(1)
            ),
        },
    };

    const size_t num_cases = sizeof(cases) / sizeof(test_json_equality_case_t);
    for (size_t i = 0; i < num_cases; i++) {
        test_json_equality_case_t *c = &(cases[i]);

        TEST_ASSERT_TRUE(c->expect_equals == json_equals(c->json1, c->json2));
        delete_json(c->json1);
        delete_json(c->json2);
    }
}

void json_helpers_tests(void) {
    RUN_TEST(test_json_to_string);
    RUN_TEST(test_json_equals);
}
