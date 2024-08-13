
#include "chutil/list.h"
#include "unity/unity_internals.h"
#include "unity/unity.h"
#include <stdbool.h>

static void l_cell_size_test(const list_impl_t *impl) {
    list_t *l = new_list(impl, sizeof(uint32_t));
    TEST_ASSERT_EQUAL(sizeof(uint32_t), l_cell_size(l));
    delete_list(l);
}

static void l_push_test(const list_impl_t *impl) {
    list_t *l = new_list(impl, sizeof(uint32_t));

    TEST_ASSERT_EQUAL_size_t(0, l_len(l));

    uint32_t num = 5;
    l_push(l, &num);
    TEST_ASSERT_EQUAL(1, l_len(l));

    for (size_t i = 0; i < 5; i++) {
        l_push(l, &num);
    }
    TEST_ASSERT_EQUAL(6, l_len(l));

    delete_list(l);
}

static void l_set_get_test(const list_impl_t *impl) {
    list_t *l = new_list(impl, sizeof(uint64_t));

    uint64_t in, out;
    const uint64_t *out_ptr;
    uint64_t *mut_out_ptr;

    in = 16;
    l_push(l, &in);

    out_ptr = (const uint64_t *)l_get(l, 0);
    TEST_ASSERT_EQUAL_UINT64(in, *out_ptr);

    l_push(l, &in);
    l_push(l, &in);

    in = 14;
    l_set(l, 1, &in);
    l_get_copy(l, 1, &out);
    TEST_ASSERT_EQUAL_UINT64(in, out);

    l_push(l, &in);
    mut_out_ptr = l_get_mut(l, 3);
    in = 25;
    *mut_out_ptr = in;
    l_get_copy(l, 3, &out);

    TEST_ASSERT_EQUAL_UINT64(25, out);

    delete_list(l);
}

typedef struct _coord_t {
    uint32_t x, y, z; 
} coord_t;

static inline bool c_eq(const coord_t *c1, const coord_t *c2) {
    return c1->x == c2->x && c1->y == c2->y && c1->z == c2->z;
}

static void l_pop_test(const list_impl_t *impl) {
    list_t *l = new_list(impl, sizeof(coord_t));

    coord_t in;
    for (size_t i = 0; i < 10; i++) {
        in.x = i; in.y = 2*i; in.z = 3*i;
        l_push(l, &in);
    }

    coord_t out;
    for (size_t i = 0; i < 10; i++) {
        size_t j = 9 - i;
        l_pop(l, &out);

        in.x = j; in.y = 2*j; in.z = 3*j;
        TEST_ASSERT_TRUE(c_eq(&in, &out));
    }

    delete_list(l);
}

static void l_poll_test(const list_impl_t *impl) {
    list_t *l = new_list(impl, sizeof(coord_t));

    coord_t in;
    for (size_t i = 0; i < 10; i++) {
        in.x = i; in.y = 2*i; in.z = 3*i;
        l_push(l, &in);
    }

    coord_t out;
    for (size_t i = 0; i < 10; i++) {
        l_poll(l, &out);

        in.x = i; in.y = 2*i; in.z = 3*i;
        TEST_ASSERT_TRUE(c_eq(&in, &out));
    }
    
    delete_list(l);
}

static void l_poll_pop_test(const list_impl_t *impl) {
    list_t *l = new_list(impl, sizeof(uint32_t)); 

    uint32_t in, out;
    
    // Push on 0->99
    for (size_t i = 0; i < 100; i++) {
        in = i;
        l_push(l, &in);
    }

    // Poll out 0->49
    for (size_t i = 0; i < 50; i++) {
        l_poll(l, &out);
        TEST_ASSERT_EQUAL_UINT32(i, out);
    }

    // Push on 100->149
    for (size_t i = 100; i < 150; i++) {
        in = i;
        l_push(l, &in);
    }

    // Pop out 149->100
    for (size_t i = 149; i >= 100; i--) {
        l_pop(l, &out);
        TEST_ASSERT_EQUAL_UINT32(i, out);
    }

    // Should have 50->99 left on list.
    for (size_t i = 0; i < 25; i++) {
        size_t s = 50 + i;
        l_poll(l, &out); 
        TEST_ASSERT_EQUAL_UINT32(s, out);
        
        size_t e = 99 - i;
        l_pop(l, &out);
        TEST_ASSERT_EQUAL(e, out);
    }

    TEST_ASSERT_EQUAL_size_t(0, l_len(l));

    delete_list(l);
}

static void l_test(const list_impl_t *impl) {
    l_cell_size_test(impl);
    l_push_test(impl);
    l_set_get_test(impl);
    l_pop_test(impl);
    l_poll_test(impl);
    l_poll_pop_test(impl);
}

static void array_list_tests(void) {
    l_test(ARRAY_LIST_IMPL); 
}

static void linked_list_tests(void) {
    l_test(LINKED_LIST_IMPL);
}

void list_tests(void) {
    RUN_TEST(array_list_tests);
    RUN_TEST(linked_list_tests);
}
