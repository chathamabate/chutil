

#include "chutil/heap.h"
#include "chutil/debug.h"

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

}

