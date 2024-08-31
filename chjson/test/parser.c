
#include "chjson/parser.h"
#include "chjson/json.h"
#include "chjson/json_helpers.h"
#include "chutil/stream.h"
#include "parser.h"

#include "unity/unity.h"
#include "unity/unity_internals.h"

typedef struct _expected_parse_case_t {
    const char *input_cstr;
    json_t *expected_json;
} expected_parse_case_t;

static void run_expected_cases(expected_parse_case_t *cases, size_t num_cases) {
    for (size_t i = 0; i < num_cases; i++) {
        expected_parse_case_t *c = &(cases[i]);
        in_stream_t *is = 
            new_in_stream_from_string(new_string_from_cstr(c->input_cstr));

        json_t *actual_json;

        parser_state_t ps = json_from_in_stream(is, &actual_json);
        TEST_ASSERT_TRUE(ps == PARSER_SUCCESS);
        
        TEST_ASSERT_TRUE(json_equals(c->expected_json, actual_json));

        delete_in_stream(is);
        delete_json(actual_json);
        
        // NOTE: This is a little dicey, but we delete the expected JSON
        // object while running.
        delete_json(c->expected_json);
    }
}

static void test_parse_json_strings(void) {
    expected_parse_case_t cases[] = {
        {
            .input_cstr = "\"\"",
            .expected_json = new_json_string(new_string())
        },
        {
            .input_cstr = "\"1\"",
            .expected_json = new_json_string(new_string_from_literal("1"))
        },
        {
            .input_cstr = "\"\\n\"",
            .expected_json = new_json_string(new_string_from_literal("\n"))
        },
        { 
            .input_cstr = "\"∆\"",
            .expected_json = new_json_string(new_string_from_literal("∆"))
        },
        { 
            .input_cstr = "\"Hello World\\n\\r\\t\\f\"",
            .expected_json = new_json_string(new_string_from_literal("Hello World\n\r\t\f"))
        },
        { 
            .input_cstr = "\"\\u00A9\"",
            .expected_json = new_json_string(new_string_from_literal("\u00A9"))
        },
        { 
            .input_cstr = "\"\\\\\"",
            .expected_json = new_json_string(new_string_from_literal("\\"))
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(expected_parse_case_t);
    run_expected_cases(cases, num_cases);
}

static void test_parse_json_numbers(void) {
    // Since really C stdlib is doing the parsing here,
    // we don't need to make these cases all that rigorous imo.
    expected_parse_case_t cases[] = {
        {
            .input_cstr = "1",
            .expected_json = new_json_number(1)
        },
        {
            // NOTE: Technically C can parse this even though
            // it isn't valid JSON, we'll let it slide for now.
            .input_cstr = "0001",
            .expected_json = new_json_number(1)
        },
        {
            .input_cstr = "1.2345",
            .expected_json = new_json_number(1.2345)
        },
        {
            .input_cstr = "10.4e5",
            .expected_json = new_json_number(10.4e5)
        },
        {
            .input_cstr = "-0.4e-5",
            .expected_json = new_json_number(-0.4e-5)
        },
        {
            .input_cstr = "10234.11E+2",
            .expected_json = new_json_number(10234.11E+2)
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(expected_parse_case_t);
    run_expected_cases(cases, num_cases);
}

static void test_parse_json_lists(void) {
    // Since really C stdlib is doing the parsing here,
    // we don't need to make these cases all that rigorous imo.
    expected_parse_case_t cases[] = {
        {
            .input_cstr = "[1]",
            .expected_json = new_json_list_from_eles(
                new_json_number(1)
            )
        },
        {
            .input_cstr = "[1  ,2 ,   3]",
            .expected_json = new_json_list_from_eles(
                new_json_number(1),
                new_json_number(2),
                new_json_number(3)
            )
        },
        {
            .input_cstr = "[  ]",
            .expected_json = new_json_list(),
        },
        {
            .input_cstr = "[[false], [null]]",
            .expected_json = new_json_list_from_eles(
                new_json_list_from_eles(
                    new_json_boolean(false)
                ),
                new_json_list_from_eles(
                    json_null()
                )
            )
        },
        {
            .input_cstr = "[[[[]]]]",
            .expected_json = new_json_list_from_eles(
                new_json_list_from_eles(
                    new_json_list_from_eles(
                        new_json_list()
                    )
                )
            )
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(expected_parse_case_t);
    run_expected_cases(cases, num_cases);
}

static void test_parse_json_objects(void) {
    // Since really C stdlib is doing the parsing here,
    // we don't need to make these cases all that rigorous imo.
    expected_parse_case_t cases[] = {
        {
            .input_cstr = "{}",
            .expected_json = new_json_object() 
        },
        {
            .input_cstr = "{\"A\":1}",
            .expected_json = new_json_object_from_kvps(
                new_string_from_literal("A"), new_json_number(1)
            )
        },
        {
            .input_cstr = "{\"age\" : 32 , \"name\": \"Bobby\"}",
            .expected_json = new_json_object_from_kvps(
                new_string_from_literal("age"), new_json_number(32),

                new_string_from_literal("name"), 
                new_json_string(new_string_from_literal("Bobby"))
            )
        },
        {
            .input_cstr = "{\"b\" : null , \"c\": true, \"a\":[1]}",
            .expected_json = new_json_object_from_kvps(
                new_string_from_literal("a"), new_json_list_from_eles(
                    new_json_number(1)
                ),
                new_string_from_literal("b"), json_null(),
                new_string_from_literal("c"), new_json_boolean(true)
            )
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(expected_parse_case_t);
    run_expected_cases(cases, num_cases);
}

static void test_parse_json_big_cases(void) {
    expected_parse_case_t cases[] = {
        {
            .input_cstr = 
                "["
                "  {\"Name\":\"Bob\", \"Age\":10},"
                "  {\"Name\":\"Dave\", \"Age\":32}"
                "]",
            .expected_json = new_json_list_from_eles(
                new_json_object_from_kvps(
                    new_string_from_literal("Name"),
                    new_json_string(new_string_from_literal("Bob")),

                    new_string_from_literal("Age"),
                    new_json_number(10)
                ),
                new_json_object_from_kvps(
                    new_string_from_literal("Name"),
                    new_json_string(new_string_from_literal("Dave")),

                    new_string_from_literal("Age"),
                    new_json_number(32)
                )
            )
        },
        {
            .input_cstr = 
                "{"
                "  \"id\":1024,"
                "  \"tags\": [ \"yen\", \"usd\", \"eur\" ],"
                "  \"txs\": {"
                "       \"10:10\": 46,"
                "       \"22:10\": 16,"
                "       \"11:22\": null"
                "   }"
                "}",
            .expected_json = new_json_object_from_kvps(
                new_string_from_literal("id"),
                new_json_number(1024),

                new_string_from_literal("tags"),
                new_json_list_from_eles(
                    new_json_string(new_string_from_literal("yen")),
                    new_json_string(new_string_from_literal("usd")),
                    new_json_string(new_string_from_literal("eur"))
                ),

                new_string_from_literal("txs"),
                new_json_object_from_kvps(
                    new_string_from_literal("10:10"),
                    new_json_number(46),

                    new_string_from_literal("22:10"),
                    new_json_number(16),

                    new_string_from_literal("11:22"),
                    json_null()
                )
            )
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(expected_parse_case_t);
    run_expected_cases(cases, num_cases);
}

static void test_parse_json_errors(void) {
    const char *bad_cases[] = {
        "hello",
        "[}",
        "\"123",
        "[,,]",
        "[1,]",
        "{3, 4}",
        "{\"a\":1, \"b\":[2,]}",
        "1ee2",
        "--3",
        "..1",
        "[true, null, false, {}, [], nul]",

        // Some bad string case...
        "\"\\u\"",
        "\"\\u123q\"",
        "\"\\p\"",
        "\"\n\r\"", // Control character not allowed!

    };

    json_t *res;
    const size_t num_cases = sizeof(bad_cases) / sizeof(const char *);
    for (size_t i = 0; i < num_cases; i++) {
        in_stream_t *is = new_in_stream_from_string(
            new_string_from_literal(bad_cases[i])
        );

        parser_state_t ps = json_from_in_stream(is, &res);
        TEST_ASSERT_TRUE(ps == PARSER_SYNTAX_ERROR);

        delete_in_stream(is);
    }
}

void parser_tests(void) {
    RUN_TEST(test_parse_json_strings);
    RUN_TEST(test_parse_json_numbers);
    RUN_TEST(test_parse_json_lists);
    RUN_TEST(test_parse_json_objects);
    RUN_TEST(test_parse_json_big_cases);
    RUN_TEST(test_parse_json_errors);
}
