
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
    memcpy(dest, al_get(al, i), al->cell_size);
}

static inline void al_set(array_list_t *al, size_t i, const void *src) {
    memcpy(al_get_mut(al, i), src, al->cell_size);
}

void al_push(array_list_t *al, const void *src);
void al_pop(array_list_t *al, void *dest);
void al_poll(array_list_t *al, void *dest);

typedef struct _linked_list_node_hdr_t {
    struct _linked_list_node_hdr_t *prev;
    struct _linked_list_node_hdr_t *next;
} linked_list_node_hdr_t;

static inline void *llnh_get_cell(linked_list_node_hdr_t *llnh) {
    return llnh + 1;
}

typedef struct _linked_list_t {
    size_t cell_size;
    size_t len;

    linked_list_node_hdr_t *first;
    linked_list_node_hdr_t *last;
} linked_list_t;

linked_list_t *new_linked_list(size_t cs);
void delete_linked_list(linked_list_t *ll);

static inline size_t ll_len(linked_list_t *ll) {
    return ll->len;
}

static inline size_t ll_cell_size(linked_list_t *ll) {
    return ll->cell_size;
}

void *ll_get_mut(linked_list_t *ll, size_t i);

static inline const void *ll_get(linked_list_t *ll, size_t i) {
    return ll_get_mut(ll, i);
}

static inline void ll_get_copy(linked_list_t *ll, size_t i, void *dest) {
    memcpy(dest, ll_get(ll, i), ll->cell_size);
}

static inline void ll_set(linked_list_t *ll, size_t i, const void *src) {
    memcpy(ll_get_mut(ll, i), src, ll->cell_size);
}

void ll_push(linked_list_t *ll, const void *src);
void ll_pop(linked_list_t *ll, void *dest);
void ll_poll(linked_list_t *ll, void *dest);





#endif
