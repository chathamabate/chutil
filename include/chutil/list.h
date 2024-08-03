
#ifndef CHUTIL_LIST_H
#define CHUTIL_LIST_H

#include <stdlib.h>
#include <string.h>

// Abstract List Types. (Really just reinventing C++ here.)

typedef void *(*list_constructor_ft)(size_t);
typedef void (*list_destructor_ft)(void *);
typedef size_t (*list_len_ft)(void *);
typedef size_t (*list_cell_size_ft)(void *);
typedef void *(*list_get_mut_ft)(void *, size_t);
typedef const void *(*list_get_ft)(void *, size_t);
typedef void (*list_get_copy_ft)(void *, size_t, void *);
typedef void (*list_set_ft)(void *, size_t, const void *);
typedef void (*list_push_ft)(void *, const void *);
typedef void (*list_pop_ft)(void *, void *);
typedef void (*list_poll_ft)(void *, void *);

typedef struct _list_impl_t {
    list_constructor_ft constructor;
    list_destructor_ft  destructor;
    list_len_ft         len;
    list_cell_size_ft   cell_size;
    list_get_mut_ft     get_mut;
    list_get_ft         get;
    list_get_copy_ft    get_copy;
    list_set_ft         set;
    list_push_ft        push;
    list_pop_ft         pop;
    list_poll_ft        poll;
} list_impl_t;

typedef struct _list_t {
    void *list;
    const list_impl_t *impl;
} list_t;

extern const list_impl_t *ARRAY_LIST_IMPL;
extern const list_impl_t *LINKED_LIST_IMPL;

list_t *new_list(const list_impl_t *impl, size_t cs);
void delete_list(list_t *l);

static inline size_t l_len(list_t *l) {
    return l->impl->len(l->list);
}

static inline size_t l_cell_size(list_t *l) {
    return l->impl->cell_size(l->list);
}

static inline void *l_get_mut(list_t *l, size_t i) {
    return l->impl->get_mut(l->list, i);
}

static inline const void *l_get_(list_t *l, size_t i) {
    return l->impl->get(l->list, i);
}

static inline void l_get_copy(list_t *l, size_t i, void *dest) {
    l->impl->get_copy(l->list, i, dest);
}

static inline void l_set(list_t *l, size_t i, const void *src) {
    l->impl->set(l->list, i, src);
}

static inline void l_push(list_t *l, const void *src) {
    l->impl->push(l->list, src);
}

static inline void l_pop(list_t *l, void *dest) {
    l->impl->pop(l->list, dest);
}

static inline void l_poll(list_t *l, void *dest) {
    l->impl->poll(l->list, dest);
}

// Concrete Arraylist

typedef struct _array_list_t {
    size_t cap;
    size_t len;
    size_t cell_size;
    void *arr; // Always will be non-NULL
} array_list_t;

array_list_t *new_array_list(size_t cs);

void delete_array_list(array_list_t *al);

static inline size_t al_len(array_list_t *al) {
    return al->len;
}

static inline size_t al_cap(array_list_t *al) {
    return al->cap;
}

static inline size_t al_cell_size(array_list_t *al) {
    return al->cell_size;
}

static inline void *al_get_mut(array_list_t *al, size_t i) {
    return (uint8_t *)(al->arr) + (i * al->cell_size);
}

static inline const void *al_get(array_list_t *al, size_t i) {
    return al_get_mut(al, i);
}

static inline void al_get_copy(array_list_t *al, size_t i, void *dest) {
    memcpy(dest, al_get(al, i), al->cell_size);
}

static inline void al_set(array_list_t *al, size_t i, const void *src) {
    memcpy(al_get_mut(al, i), src, al->cell_size);
}

void al_push(array_list_t *al, const void *src);
void al_pop(array_list_t *al, void *dest);
void al_poll(array_list_t *al, void *dest);

// Concrete Linked List

typedef struct _linked_list_node_hdr_t {
    struct _linked_list_node_hdr_t *prev;
    struct _linked_list_node_hdr_t *next;
} linked_list_node_hdr_t;

static inline void *llnh_get_cell(linked_list_node_hdr_t *llnh) {
    return llnh + 1;
}

typedef struct _linked_list_t {
    size_t cell_size;
    size_t len;

    linked_list_node_hdr_t *first;
    linked_list_node_hdr_t *last;
} linked_list_t;

linked_list_t *new_linked_list(size_t cs);
void delete_linked_list(linked_list_t *ll);

static inline size_t ll_len(linked_list_t *ll) {
    return ll->len;
}

static inline size_t ll_cell_size(linked_list_t *ll) {
    return ll->cell_size;
}

void *ll_get_mut(linked_list_t *ll, size_t i);

static inline const void *ll_get(linked_list_t *ll, size_t i) {
    return ll_get_mut(ll, i);
}

static inline void ll_get_copy(linked_list_t *ll, size_t i, void *dest) {
    memcpy(dest, ll_get(ll, i), ll->cell_size);
}

static inline void ll_set(linked_list_t *ll, size_t i, const void *src) {
    memcpy(ll_get_mut(ll, i), src, ll->cell_size);
}

void ll_push(linked_list_t *ll, const void *src);
void ll_pop(linked_list_t *ll, void *dest);
void ll_poll(linked_list_t *ll, void *dest);



#endif
