
#include <chutil/debug.h>
#include <stdio.h>
#include <stdlib.h>

static size_t malloc_count = 0;

void *_safe_malloc(size_t s) {
    void *mem = malloc(s);
    if (!mem) {
        fprintf(stderr, "Malloc failure.\n");
        exit(1);
    }
    malloc_count++;

    if (malloc_count == 0) {
        fprintf(stderr, "Malloc overflow.\n");
        exit(1);
    }
    return mem;
}

void *_safe_realloc(void *mem, size_t s) {
    void *new_mem = realloc(mem, s);
    if (!new_mem) {
        fprintf(stderr, "Realloc failure.\n");
        exit(1);
    }
    return new_mem;
}

void _safe_free(void *mem) {
    if (malloc_count == 0) {
        fprintf(stderr, "Malloc underflow.\n");
    }

    malloc_count--;
    free(mem);
}

size_t get_malloc_count(void) {
    return malloc_count;
}

void reset_malloc_count(void) {
    malloc_count = 0;
}
