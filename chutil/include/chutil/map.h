
#ifndef CHUTIL_MAP_H
#define CHUTIL_MAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Only hashmap implementation for now!

typedef bool (*hash_map_key_eq_ft)(const void *, const void *);
typedef bool (*hash_map_val_eq_ft)(const void *, const void *);
typedef uint32_t (*hash_map_hash_ft)(const void *);

typedef void *key_val_pair_t;
#define HASH_MAP_EXHAUSTED NULL

// Key Value chains form singly linked lists.
typedef struct _key_val_header_t {
    struct _key_val_header_t *next;
    uint32_t hash_val;
} key_val_header_t;

typedef struct _hash_map_t {
    size_t key_size;
    size_t value_size;

    hash_map_hash_ft hash_func;
    hash_map_key_eq_ft eq_func;

    size_t num_keys;

    size_t chains_cap;
    key_val_header_t **chains;

    size_t iter_chain_ind;
    key_val_header_t *iter; // header of next pair to return from next_kvp.
                            // NULL means empty.
} hash_map_t;

hash_map_t *new_hash_map(size_t ks, size_t vs, 
        hash_map_hash_ft hf, hash_map_key_eq_ft ef);

void delete_hash_map(hash_map_t *hm);

// There is no key_mut since keys should never change!
static inline const void *kvp_key(hash_map_t *hm, key_val_pair_t kvp) {
    (void)hm;
    return kvp;
}

static inline void *kvp_val(hash_map_t *hm, key_val_pair_t kvp) {
    return (uint8_t *)kvp + hm->key_size;   // Skip over key.
}

static inline size_t hm_num_keys(hash_map_t *hm) {
    return hm->num_keys;
}

// These 2 functions are for iterating over the key_value pairs of a map.
// NOTE: These calls are meant to be isolated from the others.
// While iterating DO NOT call any function which could modify the map
// for example (hm_put or hm_remove)
//
// Before looping, always call hm_reset_iterator.
void hm_reset_iterator(hash_map_t *hm);

// This will return HASH_MAP_EXHAUSTED when done.
key_val_pair_t hm_next_kvp(hash_map_t *hm);

void hm_put(hash_map_t *hm, const void *key, const void *value);
void *hm_get(hash_map_t *hm, const void *key);
bool hm_remove(hash_map_t *hm, const void *key);

static inline bool hm_get_copy(hash_map_t *hm, const void *key, void *dest) {
    void *val = hm_get(hm, key);
    if (!val) {
        return false;
    }

    memcpy(dest, val, hm->value_size);
    return true;
}

static inline bool hm_contains(hash_map_t *hm, const void *key) {
    if (hm_get(hm, key)) {
        return true;
    }

    return false;
}

// I decided not to make a helper functions class for maps
// since there is only one map implementation.
bool hm_equals(hash_map_t *hm1, hash_map_t *hm2, hash_map_val_eq_ft val_eq);

#endif
