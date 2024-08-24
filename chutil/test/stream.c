
#include "stream.h"
#include "chutil/stream.h"

#include <string.h>

#include "chutil/string.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"

static void test_string_in_stream(void) {
    const char *s = "Hello";
    size_t len = strlen(s);
    char out;
    stream_state_t state;

    in_stream_t *is = 
        new_in_stream_from_string(new_string_from_literal(s));

    for (size_t i = 0; i < len; i++) {
        state = is_peek_char(is, &out);
        TEST_ASSERT_TRUE(state == STREAM_SUCCESS);
        TEST_ASSERT_EQUAL_CHAR(s[i], out);

        state = is_next_char(is, &out);
        TEST_ASSERT_TRUE(state == STREAM_SUCCESS);
        TEST_ASSERT_EQUAL_CHAR(s[i], out);
    }

    TEST_ASSERT_TRUE(STREAM_EMPTY == is_peek_char(is, NULL));
    TEST_ASSERT_TRUE(STREAM_EMPTY == is_next_char(is, NULL));
    
    delete_in_stream(is);
}

// Not going to run this everytime.
// Will just point to a absolute path on my machine.
#define TEST_READ_FILE_PATH "/Users/chathamabate/Desktop/Git-Workspaces/chlibs/chutil/test/read.txt"
static void test_file_in_stream(void) {
    in_stream_t *is; 

    is = new_in_stream_from_file("NOT A FILE");
    TEST_ASSERT_NULL(is);

    is = new_in_stream_from_file(TEST_READ_FILE_PATH);
    TEST_ASSERT_NOT_NULL(is);

    char out1, out2;
    stream_state_t state1, state2;

    while (true) {
        state1 = is_peek_char(is, &out1);
        state2 = is_next_char(is, &out2);

        TEST_ASSERT_TRUE(state1 == state2);
        if (state1 != STREAM_SUCCESS) {
            break;
        }
        
        TEST_ASSERT_EQUAL_CHAR(out1, out2);
        printf("%c", out1);
    }

    delete_in_stream(is);
}

static void test_string_out_stream(void) {
    const char *s = "Hello World";

    string_t *builder = new_string();
    out_stream_t *os = new_out_stream_to_string(builder);

    TEST_ASSERT_NOT_NULL(os);
    stream_state_t state = os_puts(os, s);
    TEST_ASSERT_TRUE(state == STREAM_SUCCESS);
    delete_out_stream(os);

    TEST_ASSERT_EQUAL_STRING(s, s_get_cstr(builder));

    delete_string(builder);
}

#define TEST_WRITE_FILE_PATH "/Users/chathamabate/Desktop/Git-Workspaces/chlibs/chutil/test/write.txt"
static void test_file_out_stream(void) {
    out_stream_t *os = new_out_stream_to_file(TEST_WRITE_FILE_PATH, "w");  

    stream_state_t state = os_puts(os, "Hello World");
    TEST_ASSERT_TRUE(state == STREAM_SUCCESS);

    delete_out_stream(os);
}

void stream_tests(void) {
    RUN_TEST(test_string_in_stream);
    RUN_TEST(test_string_out_stream);

    (void)test_file_in_stream;
    //RUN_TEST(test_file_in_stream);

    (void)test_file_out_stream;
    //RUN_TEST(test_file_out_stream);
}


