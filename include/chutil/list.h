
#ifndef CHUTIL_LIST_H
#define CHUTIL_LIST_H

#include <stdlib.h>

struct _array_list_t;
typedef struct _array_list_t array_list_t;

array_list_t *new_array_list_w_cap(size_t cs, size_t cp);

static inline array_list_t *new_array_list(size_t cs) {
    return new_array_list_w_cap(cs, 1);
}

void delete_array_list(array_list_t *al);

size_t al_len(array_list_t *al);
size_t al_cap(array_list_t *al);

void al_push(array_list_t *al, const void *src);
void al_pop(array_list_t *al, void *dest);

void al_get(array_list_t *al, size_t i, void *dest);
void al_set(array_list_t *al, size_t i, const void *src);
void *al_get_mut(array_list_t *al, size_t i);

#endif
