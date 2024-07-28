
#ifndef CHUTIL_LIST_H
#define CHUTIL_LIST_H

#include <stdlib.h>
#include <string.h>

typedef struct _array_list_t {
    size_t cap;
    size_t len;
    size_t cell_size;
    void *arr; // Always will be non-NULL
} array_list_t;

array_list_t *new_array_list_w_cap(size_t cs, size_t cp);

static inline array_list_t *new_array_list(size_t cs) {
    return new_array_list_w_cap(cs, 1);
}

void delete_array_list(array_list_t *al);

static inline size_t al_len(array_list_t *al) {
    return al->len;
}

static inline size_t al_cap(array_list_t *al) {
    return al->cap;
}

static inline size_t al_cell_size(array_list_t *al) {
    return al->cell_size;
}

static inline void *al_get_mut(array_list_t *al, size_t i) {
    return (uint8_t *)(al->arr) + (i * al->cell_size);
}

static inline const void *al_get(array_list_t *al, size_t i) {
    return al_get_mut(al, i);
}

static inline void al_get_copy(array_list_t *al, size_t i, void *dest) {
    memcpy(dest, al_get_mut(al, i), al->cell_size);
}

static inline void al_set(array_list_t *al, size_t i, const void *src) {
    memcpy(al_get_mut(al, i), src, al->cell_size);
}

void al_push(array_list_t *al, const void *src);
void al_pop(array_list_t *al, void *dest);

#endif
