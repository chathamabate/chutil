
#ifndef CHUTIL_HEAP_H
#define CHUTIL_HEAP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint32_t (*heap_priority_ft)(const void *);

typedef struct _heap_val_header_t {
    uint32_t priority;
} heap_val_header_t;

static inline void *hvh_to_hv(heap_val_header_t *hvh) {
    return hvh + 1; // Just skip over the header.
}

typedef struct _heap_t {
    size_t cell_size;
    size_t val_size;

    size_t cap;
    size_t len;
    void *table;

    heap_priority_ft priority_func;
} heap_t;

heap_t *new_heap(size_t vs, heap_priority_ft pf);
void delete_heap(heap_t *hp);

static inline size_t hp_len(heap_t *hp) {
    return hp->len;
}

static inline bool hp_empty(heap_t *hp) {
    return hp->len == 0;
}

void *hp_peek(heap_t *hp);
bool hp_pop(heap_t *hp, void *dest);

void hp_push(heap_t *hp, const void *src);

void hp_reset_iterator(heap_t *hp);
void *hp_next(heap_t *hp);
void hp_re_heap(heap_t *hp);

#endif
