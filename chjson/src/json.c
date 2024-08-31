
#include "chjson/json.h"
#include "chutil/debug.h"
#include "chutil/map.h"
#include "chutil/string.h"
#include <stdarg.h>
#include <stdio.h>

static json_t _NULL_JSON = {
    .type = CHJSON_NULL,
};
static json_t *NULL_JSON = &_NULL_JSON;

json_t *new_json_object(void) {
    hash_map_t *hm = new_hash_map(sizeof(string_t *), sizeof(json_t *),
           (hash_map_hash_ft)s_indirect_hash, (hash_map_key_eq_ft)s_indirect_equals);

    json_t *json = safe_malloc(sizeof(json_t));

    json->type = CHJSON_OBJECT;
    json->object_ptr = hm;

    return json;
}

json_t *_new_json_object_from_kvps(int dummy, ...) {
    json_t *json = new_json_object();
    hash_map_t *hm = json->object_ptr;

    va_list arg_ptr;
    va_start(arg_ptr, dummy);

    while (true) {
        string_t *key = va_arg(arg_ptr, string_t *);
        if (!key) {
            break;
        }

        json_t *val = va_arg(arg_ptr, json_t *);
        if (!val) {
            break;
        }

        hm_put(hm, &key, &val);
    }

    va_end(arg_ptr);

    return json; 
}

json_t *new_json_list(void) {
    list_t *l = new_list(ARRAY_LIST_IMPL, sizeof(json_t *));

    json_t *json = safe_malloc(sizeof(json_t));
    json->type = CHJSON_LIST;
    json->list_ptr = l;

    return json;
}

json_t *_new_json_list_from_eles(int dummy, ...) {
    json_t *json = new_json_list();
    list_t *l = json->list_ptr;


    va_list arg_ptr;
    va_start(arg_ptr, dummy);

    json_t *ele = va_arg(arg_ptr, json_t *);
    while (ele) {
        l_push(l, &ele);
        ele = va_arg(arg_ptr, json_t *);
    }

    va_end(arg_ptr);

    return json;
}

// NULL will result in an empty json string.
json_t *new_json_string(string_t *s) {
    json_t *json = safe_malloc(sizeof(json_t));
    json->type = CHJSON_STRING;
    json->string_ptr = s;

    return json;
}

json_t *new_json_number(double n) {
    json_t *json = safe_malloc(sizeof(json_t));

    json->type = CHJSON_NUMBER;
    json->number_val = n;

    return json;
}

json_t *new_json_boolean(bool b) {
    json_t *json = safe_malloc(sizeof(json_t));

    json->type = CHJSON_BOOLEAN;
    json->bool_val = b;

    return json;
}

json_t *json_null(void) {
    return NULL_JSON;
}

void delete_json(json_t *json) {
    hash_map_t *hm;
    list_t *l;
    string_t *s;
    
    key_val_pair_t kvp;

    string_t **key_ptr;
    json_t **val_ptr;

    switch (json->type) {
    case CHJSON_OBJECT:
        hm = json_as_object(json);
        hm_reset_iterator(hm);
        while ((kvp = hm_next_kvp(hm)) != HASH_MAP_EXHAUSTED) {
            key_ptr = (string_t **)kvp_key(hm, kvp);
            val_ptr = (json_t **)kvp_val(hm, kvp);

            delete_string(*key_ptr);
            delete_json(*val_ptr);
        }
        delete_hash_map(hm);
        break;

    case CHJSON_LIST:
        l = json_as_list(json);
        l_reset_iterator(l);
        while ((val_ptr = (json_t **)l_next(l))) {
            delete_json(*val_ptr);
        }
        delete_list(l);
        break;

    case CHJSON_STRING:
        s = json_as_string(json);
        delete_string(s);
        break;

    case CHJSON_NUMBER:
    case CHJSON_BOOLEAN:
        break;

    case CHJSON_NULL:
        return;
    }

    safe_free(json);
}

hash_map_t *json_as_object(json_t *json) {
    if (json->type == CHJSON_OBJECT) {
        return json->object_ptr;
    }

    return NULL;
}

list_t *json_as_list(json_t *json) {
    if (json->type == CHJSON_LIST) {
        return json->list_ptr;
    }

    return NULL;
}

string_t *json_as_string(json_t *json) {
    if (json->type == CHJSON_STRING) {
        return json->string_ptr;
    }

    return NULL;
}

double *json_as_number(json_t *json) {
    if (json->type == CHJSON_NUMBER) {
        return &(json->number_val);
    }

    return NULL;
}

bool *json_as_bool(json_t *json) {
    if (json->type == CHJSON_BOOLEAN) {
        return &(json->bool_val);
    }

    return NULL;
}
