
#include "chutil/map.h"
#include "chutil/debug.h"

static inline key_val_pair_t kvh_to_kvp(key_val_header_t *kvh) {
    return (uint8_t *)kvh + sizeof(key_val_header_t);
}

// Once the number of elements in the map is greater
// than (1 / HM_FILL_FACTOR) * chains_cap, resize! 
#define HM_FILL_FACTOR 2

static void hm_check_resize(hash_map_t *hm) {
    if (hm->num_keys * HM_FILL_FACTOR <= hm->chains_cap) {
        return;
    }

    size_t new_cap = hm->chains_cap * 2;
    key_val_header_t **new_chains = safe_malloc(sizeof(key_val_header_t *) * new_cap);
    for (size_t i = 0; i < new_cap; i++) {
        new_chains[i] = NULL;
    }

    for (size_t i = 0; i < hm->chains_cap; i++) {
        key_val_header_t *iter = hm->chains[i];
        key_val_header_t *next;

        while (iter) {
            next = iter->next;

            size_t new_ind = iter->hash_val % new_cap;
            iter->next = new_chains[new_ind];
            new_chains[new_ind] = iter;
            
            iter = next;
        }
    }

    // NOTE: we have moved all kvps over to the new chain...
    // so they themselves need not be freed as they still are in use.
    //
    // Only the actual table must be freed.
    safe_free(hm->chains);

    hm->chains_cap = new_cap;
    hm->chains = new_chains;
}

hash_map_t *new_hash_map(size_t ks, size_t vs, 
        hash_map_hash_ft hf, hash_map_eq_ft ef) {
    if (ks == 0 || hf == NULL || ef == NULL) {
        return NULL;
    }

    // NOTE: value size CAN be 0 (hash set)

    hash_map_t *hm = safe_malloc(sizeof(hash_map_t));

    hm->key_size = ks;
    hm->value_size = vs;
    hm->hash_func = hf;
    hm->eq_func = ef;

    hm->num_keys = 0;

    // Start with table size of 8, arb choice.
    hm->chains_cap = 8;

    hm->chains = safe_malloc(sizeof(key_val_header_t *) * hm->chains_cap);
    for (size_t i = 0; i < hm->chains_cap; i++) {
        hm->chains = NULL;
    }

    return hm;
}

void delete_hash_map(hash_map_t *hm) {
    for (size_t i = 0; i < hm->chains_cap; i++) {
        key_val_header_t *iter = hm->chains[i];
        key_val_header_t *next;

        while (iter) {
            next = iter->next;
            safe_free(iter);
            iter = next;
        }
    }

    safe_free(hm->chains);
    safe_free(hm);
}

void hm_reset_iterator(hash_map_t *hm);
key_val_pair_t hm_next_kvp(hash_map_t *hm);

void hm_put(hash_map_t *hm, const void *key, const void *value) {
    uint32_t hash_val = hm->hash_func(key);
    size_t chain_ind = hash_val % hm->chains_cap;

    key_val_header_t *iter = hm->chains[chain_ind];
    while (iter) {
        key_val_pair_t kvp = kvh_to_kvp(iter);
        if (iter->hash_val == hash_val && hm->eq_func(kvp_key(hm, kvp), key)) {
            memcpy(kvp_val(hm, kvp), value, hm->value_size);
            return; // We can just exit after an update.
        }
        
        iter = iter->next;
    }

    // No match... new kvp must be made...
    key_val_header_t *new_kvh = safe_malloc(sizeof(key_val_header_t) + 
            hm->key_size + hm->value_size);
    new_kvh->next = hm->chains[chain_ind]; 
    hm->chains[chain_ind] = new_kvh;

    // Finally, increase our number of keys and check resize!
    hm->num_keys++;
    hm_check_resize(hm);
}

void *hm_get(hash_map_t *hm, const void *key) {
    uint32_t hash_val = hm->hash_func(key);
    size_t chain_ind = hash_val % hm->chains_cap;

    key_val_header_t *iter = hm->chains[chain_ind];
    while (iter) {
        key_val_pair_t kvp = kvh_to_kvp(iter);
        if (iter->hash_val == hash_val && hm->eq_func(kvp_key(hm, kvp), key)) {
            return kvp_val(hm, kvp);
        }
        
        iter = iter->next;
    }

    return NULL;
}

bool hm_remove(hash_map_t *hm, const void *key);
