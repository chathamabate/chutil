#include "chutil/list.h"
#include "chutil/debug.h"
#include <string.h>

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

void al_poll(array_list_t *al, void *dest) {
    if (al->len == 0) {
        return;
    }

    if (dest) {
        memcpy(dest, al->arr, al->cell_size);
    }

    uint8_t *curr = al_get_mut(al, 0);
    uint8_t *next = al_get_mut(al, 1);
    const uint8_t *end = al_get_mut(al, al->len - 1);

    while (next <= end) {
        memcpy(curr, next, al->cell_size);
        curr = next;
        next += al->cell_size;
    }

    al->len--;
}

linked_list_t *new_linked_list(size_t cs) {
    if (cs == 0) {
        return NULL;
    }

    linked_list_t *ll = safe_malloc(sizeof(linked_list_t));
    ll->cell_size = cs;
    ll->len = 0;
    ll->first = NULL;
    ll->last = NULL;

    return ll;
}

void delete_linked_list(linked_list_t *ll) {
    linked_list_node_hdr_t *curr = ll->first;
    linked_list_node_hdr_t *next;

    while (curr) {
        next = curr->next;
        safe_free(curr);

        curr = next;
    }

    safe_free(ll);
}

void *ll_get_mut(linked_list_t *ll, size_t i) {
    linked_list_node_hdr_t *node = ll->first;
    size_t cnt = 0;

    while (node && i < cnt) {
        node = node->next;
        cnt++;
    }

    if (node) {
        return llnh_get_cell(node);
    }

    return NULL;
}

void ll_push(linked_list_t *ll, const void *src) {
    linked_list_node_hdr_t *node = 
        safe_malloc(sizeof(linked_list_node_hdr_t) + ll->cell_size);

    node->next = NULL;
    node->prev = ll->last;

    if (ll->last) {
        ll->last->next = node;
    } else {
        ll->first = node;
    }

    ll->last = node;
    ll->len++;

    void *cell = llnh_get_cell(node);
    memcpy(cell, src, ll->cell_size);
}

void ll_pop(linked_list_t *ll, void *dest) {
    if (ll->len == 0) {
        return;
    }

    if (dest) {
        // Last cannot be NULL here.
        void *cell = llnh_get_cell(ll->last);
        memcpy(dest, cell, ll->cell_size);
    }

    ll->last = ll->last->prev;
    if (ll->last) {
        ll->last->next = NULL;
    }

    ll->len--;
}

void ll_poll(linked_list_t *ll, void *dest) {
    if (ll->len == 0) {
        return;
    }

    if (dest) {
        void *cell = llnh_get_cell(ll->first);
        memcpy(dest, cell, ll->cell_size);
    }

    ll->first = ll->first->next;
    if (ll->first) {
        ll->first->prev = NULL;
    }

    ll->len--;
}



