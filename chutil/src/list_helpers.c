
#include "chutil/list_helpers.h"
#include "chutil/list.h"

bool l_equals(list_t *l1, list_t *l2, list_cell_equals_ft eq) {
    if (l_len(l1) != l_len(l2)) {
        return false;
    }

    const void *cell1;
    const void *cell2;

    l_reset_iterator(l1);
    l_reset_iterator(l2);

    // NOTE: we really only need to check one of these not being NULL
    // because we know the lengths are equal, but whatevs.
    while ((cell1 = l_next(l1)) && (cell2 = l_next(l2))) {
        if (!eq(cell1, cell2)) {
            return false;
        }
    }

    return true;
}
