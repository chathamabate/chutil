
#ifndef CHSYS_MEM_H
#define CHSYS_MEM_H

#include <stdlib.h>

// All these calls require init_sys to be called.
// before being used.

void *safe_malloc(size_t s);
void *safe_realloc(void *mem, size_t s);
void safe_free(void *mem);

#endif
