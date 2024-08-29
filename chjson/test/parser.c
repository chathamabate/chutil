
#include "chjson/parser.h"
#include "chjson/json.h"
#include "chjson/json_helpers.h"
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
            .input_cstr = "\"1\"",
            .expected_json = new_json_string(new_string_from_literal("1"))
        },
        {
            .input_cstr = "\"\\n\"",
            .expected_json = new_json_string(new_string_from_literal("\\n"))
        },
        { 
            .input_cstr = "\"∆\"",
            .expected_json = new_json_string(new_string_from_literal("∆"))
        },
        { 
            .input_cstr = "\"∆\"",
            .expected_json = new_json_string(new_string_from_literal("∆"))
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(expected_parse_case_t);
    run_expected_cases(cases, num_cases);
}

void parser_tests(void) {
    RUN_TEST(test_parse_json_strings);
}
