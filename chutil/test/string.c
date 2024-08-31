
#include "_string.h"
#include "chutil/string.h"
#include <string.h>

#include "unity/unity_internals.h"
#include "unity/unity.h"


static void test_s_append_char(void) {
    string_t *s = new_string();
    
    const char *adds = "Hello";
    size_t len = strlen(adds);
    for (size_t i = 0; i < len; i++) {
        s_append_char(s, adds[i]);
    }
    
    TEST_ASSERT_EQUAL_size_t(len, s_len(s));
    TEST_ASSERT_EQUAL_STRING(adds, s_get_cstr(s));

    delete_string(s);
}

static void test_s_append_string(void) {
    const char *cstr1 = "Hello, ";
    const char *cstr2 = "World";
    string_t *add_s = new_string_from_cstr("!");

    string_t *s = new_string_from_cstr(cstr1);
    s_append_cstr(s, cstr2);
    s_append_string(s, add_s);

    TEST_ASSERT_EQUAL_STRING("Hello, World!", s_get_cstr(s));

    delete_string(s);
    delete_string(add_s);
}

static void test_s_substring(void) {
    const char *big_str = "HDFDFDksfjsdkfjdf219231923291jsdjsdfjsdjf";
    string_t *s = new_string_from_cstr(big_str);

    const size_t start = 3, end = 11;
    string_t *sub1 = s_substring(s, start, end);

    for (size_t i = start; i < end; i++) {
        TEST_ASSERT_EQUAL_CHAR(big_str[i], s_get_char(s, i));
    }

    // Make sure this doesn't modify big_str;
    s_append_char(sub1, '_');
    TEST_ASSERT_FALSE(big_str[end] == s_get_char(sub1, end));

    delete_string(sub1);
    delete_string(s);
}

static void test_s_big(void) {
    const char *msg = "Heyo";
    const size_t msg_len = strlen(msg);
    const size_t msg_adds = 1000;

    string_t *s1 = new_string();
    for (size_t i = 0; i < msg_adds; i++) {
        s_append_cstr(s1, msg);
    }

    string_t *s2 = s_copy(s1);
    for (size_t i = 0; i < msg_adds * msg_len; i++) {
        if (i % msg_len == 0) {
            s_set_char(s1, i, '*');
            s_set_char(s2, i, '&');
        }
    }

    for (size_t i = 0; i < msg_adds * msg_len; i++) {
        if (i % msg_len == 0) {
            TEST_ASSERT_EQUAL_CHAR('*', s_get_char(s1, i));
            TEST_ASSERT_EQUAL_CHAR('&', s_get_char(s2, i));
        } else {
            TEST_ASSERT_EQUAL_CHAR(msg[i % msg_len], s_get_char(s1, i));
            TEST_ASSERT_EQUAL_CHAR(msg[i % msg_len], s_get_char(s2, i));
        }
    }

    delete_string(s1);
    delete_string(s2);
}

// Now I need to test this from literal stuff.

static void test_s_from_literal_1(void) {
    string_t *s1 = new_string_from_literal("Hello");
    string_t *s2 = s_copy(s1);

    // checking basic reference counting here.

    delete_string(s1);
    TEST_ASSERT_EQUAL_STRING("Hello", s_get_cstr(s2));
    
    delete_string(s2);
}

static void test_s_from_literal_2(void) {
    string_t *s1 = new_string_from_literal("Hello");
    string_t *s2 = s_copy(s1);

    s_append_char(s2, '!');
    TEST_ASSERT_EQUAL_STRING("Hello!", s_get_cstr(s2));
    TEST_ASSERT_EQUAL_STRING("Hello", s_get_cstr(s1));

    string_t *s3 = s_copy(s2);
    s_append_cstr(s3, " Woo!");
    TEST_ASSERT_EQUAL_STRING("Hello!", s_get_cstr(s2));
    TEST_ASSERT_EQUAL_STRING("Hello! Woo!", s_get_cstr(s3));

    s_set_char(s1, 0, 'p');
    TEST_ASSERT_EQUAL_STRING("pello", s_get_cstr(s1));
    
    delete_string(s1);
    delete_string(s2);
    delete_string(s3);
}

static void test_s_from_literal_3(void) {
    string_t *s1 = new_string_from_literal("Bob");
    string_t *s2 = s_copy(s1);

    s_set_char(s2, 0, 'M');
    TEST_ASSERT_EQUAL_STRING("Mob", s_get_cstr(s2));

    s_set_char(s2, 2, 'p');
    TEST_ASSERT_EQUAL_STRING("Mop", s_get_cstr(s2));

    s_append_cstr(s2, " was cool");
    TEST_ASSERT_EQUAL_STRING("Mop was cool", s_get_cstr(s2));

    s_append_char(s2, '!');
    TEST_ASSERT_EQUAL_STRING("Mop was cool!", s_get_cstr(s2));

    s_append_char(s2, '!');
    TEST_ASSERT_EQUAL_STRING("Mop was cool!!", s_get_cstr(s2));

    string_t *s3 = s_copy(s2);

    s_set_char(s2, 0, 'B');
    TEST_ASSERT_EQUAL_STRING("Bop was cool!!", s_get_cstr(s2));
    TEST_ASSERT_EQUAL_STRING("Mop was cool!!", s_get_cstr(s3));

    TEST_ASSERT_EQUAL_STRING("Bob", s_get_cstr(s1));

    delete_string(s1);
    delete_string(s2);
    delete_string(s3);
}

void string_tests(void) {
    RUN_TEST(test_s_append_char);
    RUN_TEST(test_s_append_string);
    RUN_TEST(test_s_substring);
    RUN_TEST(test_s_big);
    RUN_TEST(test_s_from_literal_1);
    RUN_TEST(test_s_from_literal_2);
    RUN_TEST(test_s_from_literal_3);
}

