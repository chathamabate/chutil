
#include <chutil/list.h>

struct _array_list_t {
    size_t cell_size;
    size_t capacity;
    size_t len;
    void *arr;
};

array_list_t *new_array_list_w_cap(size_t cs, size_t cp) {
    return NULL;
}

