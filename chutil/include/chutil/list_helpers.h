
#ifndef CHUTIL_LIST_HELPERS_H
#define CHUTIL_LIST_HELPERS_H

#include "chutil/list.h"
#include <stdbool.h>

// The point of these end points is to expose
// helper functions for using any implementation
// of list_t.
//
// The below functions will only use given virtual
// functions and will assume nothing about underlying
// list structure.

typedef bool (*list_cell_equals_ft)(const void *cell1, const void *cell2);

// Compare whether or not give lists are equivelant.
bool l_equals(list_t *l1, list_t *l2, list_cell_equals_ft eq);

#endif
