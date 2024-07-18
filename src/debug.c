
#include <chutil/debug.h>
#include <stdio.h>

static size_t malloc_count = 0;

void *_safe_malloc(size_t s) {
#ifdef CHUTIL_DEBUG
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
#else
    return NULL;
#endif
}

void _safe_free(void *mem) {
#ifdef CHUTIL_DEBUG
    if (malloc_count == 0) {
        fprintf(stderr, "Malloc underflow.\n");
    }

    malloc_count--;
    free(mem);
#endif
}

size_t get_malloc_count(void) {
    return malloc_count;
}
