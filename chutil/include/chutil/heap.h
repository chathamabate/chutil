
#ifndef CHUTIL_HEAP_H
#define CHUTIL_HEAP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// What is a heap again?
// It's a structure with log(n) pushing and poppping
// where popping removes 

typedef uint32_t (*heap_priority_ft)(const void *);

typedef struct _heap_t {

} heap_t;

heap_t *new_heap(size_t cs, heap_priority_ft pf);
void delete_heap(heap_t *hp);

size_t hp_len(heap_t *hp);

static inline bool hp_empty(heap_t *hp) {
    return hp_len(hp) == 0;
}

void hp_push(heap_t *hp, const void *src);

// Returns true if something was popped, false otherwise.
bool hp_pop(heap_t *hp, void *dest);

#endif
