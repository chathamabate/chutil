
#include "chutil/heap.h"
#include "chutil/debug.h"

#include <stdlib.h>
#include <string.h>

typedef struct _heap_val_header_t {
    uint32_t priority;
} heap_val_header_t;

static inline void *hvh_to_hv(heap_val_header_t *hvh) {
    return hvh + 1; // Just skip over the header.
}

heap_t *new_heap(size_t vs, heap_priority_ft pf) {
    if (vs == 0 || !pf) {
        return NULL;
    }

    heap_t *hp = safe_malloc(sizeof(heap_t));

    hp->cell_size = sizeof(heap_val_header_t) + vs;
    hp->val_size = vs;

    hp->cap = 1;
    hp->len = 0;

    hp->table = safe_malloc(hp->cell_size * hp->cap);

    hp->priority_func = pf;

    return hp;
}

void delete_heap(heap_t *hp) {
    safe_free(hp->table);
    safe_free(hp);
}

static inline heap_val_header_t *hp_get_header(heap_t *hp, size_t i) {
    if (i >= hp->len) {
        return NULL;
    }

    return (heap_val_header_t *)((uint8_t *)(hp->table) + (hp->cell_size * i));
}

void *hp_peek(heap_t *hp) {
    if (hp->len == 0) {
        return NULL;
    }

    return hvh_to_hv(hp_get_header(hp, 0));
}

// This call assumes the table inside hp is a valid heap
// from [0, e-1].
//
// This function assumes we are trying to add an element with
// priority p to our "subheap".
//
// It will shift our heap accordingly, and return the place i
// where our new element should be copied into.
//
// NOTE: This will probably write over the cell at index e.
static size_t hp_bubble_up(heap_t *hp, size_t e, size_t p) {
    size_t i = e;
    while (i > 0) {
        size_t parent = i / 2;

        // Calculate these headers everytime for simplicity.
        heap_val_header_t *curr_hdr = hp_get_header(hp, i);
        heap_val_header_t *parent_hdr = hp_get_header(hp, parent);

        if (parent_hdr->priority <= p) {
            break;
        }

        // If we have a higher priority than our parent,
        // we must bubble the parent down.

        memcpy(curr_hdr, parent_hdr, hp->cell_size);
        
        i = parent;
    }

    return i;
}

bool hp_pop(heap_t *hp, void *dest) {
    if (hp->len == 0) {
        return false;
    }

    // If we have elements, the root will always be what's copied.
    heap_val_header_t *root_hdr = hp_get_header(hp, 0);
    memcpy(dest, hvh_to_hv(root_hdr), hp->val_size);

    if (hp->len == 1) {
        hp->len--;
        return true;
    }

    // This is conceptually swapped into the root position.
    // After doing bubbling down, the correct position for this to be
    // copied into will be found.
    //
    // We won't do the copy until the very end.
    heap_val_header_t *last_hdr = hp_get_header(hp, hp->len - 1);
    size_t p = last_hdr->priority;

    // Shrink the valid area of our heap.
    hp->len--;

    size_t i = 0;

    while (true) {
        heap_val_header_t *curr_hdr = hp_get_header(hp, i);

        size_t left = (i * 2) + 1;
        heap_val_header_t *left_hdr = hp_get_header(hp, left);

        size_t right = (i * 2) + 2;
        heap_val_header_t *right_hdr = hp_get_header(hp, right);

        if (left_hdr && left_hdr->priority < p) {
            memcpy(curr_hdr, left_hdr, hp->cell_size); 
            i = left;
            continue;
        }

        if (right_hdr && right_hdr->priority < p) {
            memcpy(curr_hdr, right_hdr, hp->cell_size); 
            i = right;
            continue;
        }
        
        // If we make it here, there is either no left/right children.
        // Or our priority higher than our children.
        break;
    }

    // Now, we copy our "last" cell into i.
    heap_val_header_t *spot_hdr = hp_get_header(hp, i);
    memcpy(spot_hdr, last_hdr, hp->cell_size);

    return true;
}

void hp_push(heap_t *hp, const void *src) {
    // First check if a resize is needed.
    if (hp->len == hp->cap) {
        hp->cap = hp->cap * 2;
        hp->table = safe_realloc(hp->table, hp->cap);
    }

    size_t p = hp->priority_func(src);
    size_t i = hp_bubble_up(hp, hp->len, p);

    // i will point to the spot we've made available.
    heap_val_header_t *spot_hdr = hp_get_header(hp, i);
    spot_hdr->priority = p;
    memcpy(hvh_to_hv(spot_hdr), src, hp->val_size);

    hp->len++;
}

void *hp_next(heap_t *hp) {
    heap_val_header_t *hdr = hp_get_header(hp, hp->iter);

    if (hdr) {
        hp->iter++;
        return hvh_to_hv(hdr); 
    }

    return NULL;
}

void hp_re_heap(heap_t *hp) {
    void *val_buf = safe_malloc(hp->val_size);

    size_t e = 1;
    while (e < hp->len) {
        heap_val_header_t *end_hdr = hp_get_header(hp, e);

        size_t end_p = end_hdr->priority;
        memcpy(val_buf, hvh_to_hv(end_hdr), hp->val_size);

        size_t i = hp_bubble_up(hp, e, end_p);

        // Only copy if we need to!
        if (i != e) {
            heap_val_header_t *spot_hdr = hp_get_header(hp, i); 
            spot_hdr->priority = end_p;
            memcpy(hvh_to_hv(spot_hdr), val_buf, hp->val_size);
        }
    }

    safe_free(val_buf);
}

