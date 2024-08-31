
#include "chutil/stream.h"
#include "chutil/string.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"
#include "utf8.h"
#include "chutil/utf8.h"

typedef struct _unicode_cstr_pair_t {
    const char *cstr;
    unicode_t uni;
} unicode_cstr_pair_t;

static void test_unicode_from_cstr(void) {
    const unicode_cstr_pair_t cases[] = {
        {.cstr = "0000", .uni = 0x0000},
        {.cstr = "1234", .uni = 0x1234},
        {.cstr = "FABC", .uni = 0xFABC},
        {.cstr = "0fcC", .uni = 0x0FCC},
        {.cstr = "baDe", .uni = 0xBADE},
    };

    const size_t num_cases = sizeof(cases) / sizeof(unicode_cstr_pair_t);
    for (size_t i = 0; i < num_cases; i++) {
        TEST_ASSERT_EQUAL_UINT16(
                cases[i].uni, unicode_from_cstr(cases[i].cstr));
    }
}

static void test_unicode_to_and_from_utf8(void) {
    const unicode_t cases[] = {
        0x0000,
        0x0001,
        0x0055,
        0x007F,
        0x0080,
        0x0081,
        0x009F,
        0x02FF,
        0x07FF,
        0x0800,
        0x08FF,
        0x18FF,
        0xBBBB,
        0xFFFF,
    };

    const size_t num_cases = sizeof(cases) / sizeof(unicode_t);
    for (size_t i = 0; i < num_cases; i++) {
        unicode_t c = cases[i];
        unicode_t actual;

        string_t *s = new_string();

        out_stream_t *os = new_out_stream_to_string(s);
        TEST_ASSERT_TRUE(STREAM_SUCCESS == unicode_to_utf8(os, c));
        delete_out_stream(os);


        in_stream_t *is = new_in_stream_from_string(s);
        TEST_ASSERT_TRUE(STREAM_SUCCESS == unicode_from_utf8(is, &actual));
        delete_in_stream(is); // This in stream will own the given string.
                              // Might want to change this in the future IMO.

        TEST_ASSERT_EQUAL_UINT16(c, actual);
    };

}

static void test_bad_utf8(void) {
    const char * const bad_cases[] = {
        "\xFF",
        "\xCF\xCF",
        "\xEA\x8F\xCF",
        "\xEA\xCF\x8F",
    };

    const size_t num_cases = sizeof(bad_cases) / sizeof(const char *);
    for (size_t i = 0; i < num_cases; i++) {
        string_t *s = new_string_from_literal(bad_cases[i]);
        in_stream_t *is = new_in_stream_from_string(s);

        unicode_t out;
        TEST_ASSERT_TRUE(STREAM_SUCCESS == unicode_from_utf8(is, &out));
        TEST_ASSERT_EQUAL_UINT16(' ', out);

        delete_in_stream(is);
    }
}

void utf8_tests(void) {
    RUN_TEST(test_unicode_from_cstr);
    RUN_TEST(test_unicode_to_and_from_utf8);
    RUN_TEST(test_bad_utf8);
}
