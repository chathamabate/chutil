

#include "chsys/mem.h"
#include "chsys/log.h"
#include "chsys/sys.h"
#include <stdlib.h>


void *safe_malloc(size_t s) {
    void *mem = malloc(s);
    if (!mem) {
        log_fatal(true, "Failed to malloc");
    }
    sys_inc_malloc_count(true);

    return mem;
}

void *safe_realloc(void *mem, size_t s) {
    if (!mem) {
        return safe_malloc(s);
    }

    void *new_mem = realloc(mem, s);
    if (!new_mem) {
        log_fatal(true, "Failed to realloc");
    }

    return new_mem;
}

void safe_free(void *mem) {
    sys_dec_malloc_count(true);
    free(mem);
}
