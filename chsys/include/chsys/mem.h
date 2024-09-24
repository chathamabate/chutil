
#ifndef CHSYS_MEM_H
#define CHSYS_MEM_H

#include <stdlib.h>
#include <stdbool.h>

// All these calls require init_sys to be called.
// before being used.

void *safe_malloc_p(bool acquire_lock, size_t s);
static inline void *safe_malloc(size_t s) {
    return safe_malloc_p(true, s);
}

void *safe_realloc_p(bool acquire_lock, void *mem, size_t s);
static inline void *safe_realloc(void *mem, size_t s) {
    return safe_realloc_p(true, mem, s);
}

void safe_free_p(bool acquire_lock, void *mem);
static inline void safe_free(void *mem) {
    safe_free_p(true, mem);
}

#endif
