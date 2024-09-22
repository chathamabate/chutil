
#ifndef CHSYS_MEM_H
#define CHSYS_MEM_H

#include <stdlib.h>
#include <stdbool.h>

// All these calls require init_sys to be called.
// before being used.

void *safe_malloc_p(bool acquire_lock, size_t s);
void *safe_realloc_p(bool acquire_lock, void *mem, size_t s);
void safe_free_p(bool acquire_lock, void *mem);

// When I made this header, the below calls were already in existence
// and used in many places. Unlike the logging calls..
// I decided to omit the acquire_lock argument from the default version
// of these functions. (This way I didn't need to go back and edit a bunch
// of prior usages)
#define safe_malloc(s) safe_malloc_p(true, s);
#define safe_realloc(mem, s) safe_realloc_p(true, mem, s);
#define safe_free(mem) safe_free_p(true, mem);

#endif
