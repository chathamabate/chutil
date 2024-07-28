#include "chutil/list.h"
#include "chutil/debug.h"

array_list_t *new_array_list_w_cap(size_t cs, size_t cp) {
    if (cs == 0) {
        return NULL;
    }

    array_list_t *al = safe_malloc(sizeof(array_list_t));

    al->cap = cp == 0 ? 1 : cp;
    al->len = 0;
    al->cell_size = cs;

    al->arr = safe_malloc(al->cell_size * al->cap);

    return al;
}

void delete_array_list(array_list_t *al) {
    safe_free(al->arr);
    safe_free(al);
}

void al_push(array_list_t *al, const void *src) {
    if (al->len == al->cap) {
        // Resize time!
        size_t new_cap = al->cap * 2;
        al->arr = safe_realloc(al->arr, al->cell_size * new_cap);
        al->cap = new_cap;
    }

    al_set(al, al->len, src); 
    al->len++;
}

void al_pop(array_list_t *al, void *dest) {
    if (al->len == 0) {
        return;
    }
    
    if (dest) {
        memcpy(dest, al_get(al, al->len - 1), al->cell_size);
    }

    al->len--;
}

