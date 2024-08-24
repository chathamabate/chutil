
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
        TEST_ASSERT_TRUE(state == STREAM_SUCCES);
        TEST_ASSERT_EQUAL_CHAR(s[i], out);

        state = is_next_char(is, &out);
        TEST_ASSERT_TRUE(state == STREAM_SUCCES);
        TEST_ASSERT_EQUAL_CHAR(s[i], out);
    }

    TEST_ASSERT_TRUE(STREAM_EMPTY == is_peek_char(is, NULL));
    TEST_ASSERT_TRUE(STREAM_EMPTY == is_next_char(is, NULL));
    
    delete_in_stream(is);
}

// Not going to run this everytime.
// Will just point to a absolute path on my machine.
#define TEST_FILE_PATH "/Users/chathamabate/Desktop/Git-Workspaces/chlibs/chutil/test/dummy.txt"
static void test_file_in_stream(void) {
    in_stream_t *is; 

    is = new_in_stream_from_file("NOT A FILE");
    TEST_ASSERT_NULL(is);

    is = new_in_stream_from_file(TEST_FILE_PATH);
    TEST_ASSERT_NOT_NULL(is);

    char out1, out2;
    stream_state_t state1, state2;

    while (true) {
        state1 = is_peek_char(is, &out1);
        state2 = is_next_char(is, &out2);

        TEST_ASSERT_TRUE(state1 == state2);
        if (state1 != STREAM_SUCCES) {
            break;
        }
        
        TEST_ASSERT_EQUAL_CHAR(out1, out2);
        printf("%c", out1);
    }

    delete_in_stream(is);
}

void stream_tests(void) {
    RUN_TEST(test_string_in_stream);

    (void)test_file_in_stream;
    //RUN_TEST(test_file_in_stream);
}


