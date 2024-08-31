
#ifndef CHUTIL_DEBUG_H
#define CHUTIL_DEBUG_H
#include <stdlib.h>
#include <stdio.h>

#define CHUTIL_DEBUG

#ifdef CHUTIL_DEBUG
#define safe_malloc(s) _safe_malloc(s)
#define safe_realloc(mem, s) _safe_realloc(mem, s)
#define safe_free(mem) _safe_free(mem)
#else
#define safe_malloc(s) malloc(s)
#define safe_realloc(mem, s) realloc(mem, s)
#define safe_free(mem) free(mem)
#endif


void *_safe_malloc(size_t s);
void *_safe_realloc(void *mem, size_t s);
void _safe_free(void *mem);

size_t get_malloc_count(void); 
void reset_malloc_count(void);

#endif
