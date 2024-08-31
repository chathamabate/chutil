
#include "chjson/json.h"
#include "chjson/json_helpers.h"

#include "chutil/string.h"
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

static void test_json_escapes_to_string(void) {
    json_t *json = new_json_list_from_eles(
            new_json_string(new_string_from_literal("Hello \u00A9 \n\t\r")),
            new_json_string(new_string_from_literal("\\\"\b\f"))
    );

    string_t *s = json_to_string(json, false);

    const char *expected = "["
        "\"Hello \u00A9 \\n\\t\\r\","
        "\"\\\\\\\"\\b\\f\""
    "]";

    TEST_ASSERT_EQUAL_STRING(expected, s_get_cstr(s));
    
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

static void test_json_lookups(void) {
    json_t *json = new_json_object_from_kvps(
        new_string_from_literal("class"),
        new_json_string_from_literal("algebra"),

        new_string_from_literal("students"),
        new_json_list_from_eles(
            new_json_string_from_literal("Bobby"),
            new_json_string_from_literal("Mark"),
            new_json_string_from_literal("Matt"),
            new_json_string_from_literal("Dale")
        )
    );

    json_t *class = json_lookup_key_cstr(json, "class");
    json_t *expected_class = new_json_string_from_literal("algebra");

    TEST_ASSERT_NOT_NULL(class);
    TEST_ASSERT_TRUE(json_equals(expected_class, class));
    delete_json(expected_class);

    json_t *mark = json_lookup_key_cstr(json, "students");
    mark = json_lookup_index(mark, 1);
    json_t *expected_mark = new_json_string_from_literal("Mark");

    TEST_ASSERT_NOT_NULL(mark);
    TEST_ASSERT_TRUE(json_equals(expected_mark, mark));
    delete_json(expected_mark);

    json_t *dummy;

    dummy = json_lookup_key_cstr(json, "Not A Key");
    TEST_ASSERT_NULL(dummy);

    dummy = json_lookup_key_cstr(json, "students");
    dummy = json_lookup_index(dummy, 10);
    TEST_ASSERT_NULL(dummy);

    delete_json(json);
}

void json_helpers_tests(void) {
    RUN_TEST(test_json_to_string);
    RUN_TEST(test_json_escapes_to_string);
    RUN_TEST(test_json_equals);
    RUN_TEST(test_json_lookups);
}
