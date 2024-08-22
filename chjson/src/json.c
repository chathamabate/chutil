
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
        const char *key = va_arg(arg_ptr, const char *);
        if (!key) {
            break;
        }

        json_t *val = va_arg(arg_ptr, json_t *);
        if (!val) {
            break;
        }

        string_t *key_string = new_string_from_cstr(key);
        hm_put(hm, &key_string, &val);
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
json_t *new_json_string(const char *s) {
    string_t *st = s ? new_string_from_cstr(s) : new_string();

    json_t *json = safe_malloc(sizeof(json_t));
    json->type = CHJSON_STRING;
    json->string_ptr = st;

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
        break;

    case CHJSON_LIST:
        l = json_as_list(json);
        l_reset_iterator(l);
        while ((val_ptr = (json_t **)l_next(l))) {
            delete_json(*val_ptr);
        }
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


// Just going to use 2 spaces as a tab for now!
#define CHJSON_STRING_TAB "  "
static inline void tab_out(string_t *out, size_t tabs) {
    for (size_t i = 0; i < tabs; i++) {
        s_append_cstr(out, CHJSON_STRING_TAB);
    }
}

// If spaced is false, tabs is ignored.
#define CHJSON_NUMBER_MAX_STR_WIDTH 26
static void json_to_string_helper(json_t *json, string_t *out, 
        bool spaced, size_t tabs) {
    hash_map_t *hm;
    list_t *l;
    bool first;
    char num_buf[CHJSON_NUMBER_MAX_STR_WIDTH];

    switch (json->type) {
    case CHJSON_OBJECT: 
        hm = json->object_ptr;

        s_append_cstr(out, "{");
    
        // We don't append a comma on the first iteration!
        first = true;

        key_val_pair_t kvp;
        hm_reset_iterator(hm);
        while ((kvp = hm_next_kvp(hm)) != HASH_MAP_EXHAUSTED) {
            string_t *key = *(string_t **)kvp_key(hm, kvp);
            json_t *val = *(json_t **)kvp_val(hm, kvp);

            if (!first) {
                s_append_cstr(out, ",");
            } else {
                first = false;
            }

            if (spaced) {
                s_append_cstr(out, "\n");
                tab_out(out, tabs + 1);
            }

            s_append_cstr(out, "\"");
            s_append_string(out, key);
            s_append_cstr(out, "\":");

            if (spaced) {
                s_append_cstr(out, " ");
            }
            
            json_to_string_helper(val, out, spaced, tabs + 1);
        }

        if (spaced) {
            s_append_cstr(out, "\n");
            tab_out(out, tabs);
        }
        s_append_cstr(out, "}");
    
        break;

    case CHJSON_LIST:
        l = json->list_ptr;

        s_append_cstr(out, "[");

        first = true;
        json_t **ele_ptr;
        l_reset_iterator(l);
        while ((ele_ptr = l_next(l))) {
            if (!first) {
                s_append_cstr(out, ",");
            } else {
                first = false;
            }

            if (spaced) {
                s_append_cstr(out, "\n");
                tab_out(out, tabs + 1);
            }

            json_to_string_helper(*ele_ptr, out, spaced, tabs + 1);
        }
        
        if (spaced) {
            s_append_cstr(out, "\n");
            tab_out(out, tabs);
        }
        s_append_cstr(out, "]");
        break;

    case CHJSON_STRING:
        s_append_cstr(out, "\"");
        s_append_string(out, json->string_ptr);
        s_append_cstr(out, "\"");
        break;

    case CHJSON_NUMBER:
        snprintf(num_buf, CHJSON_NUMBER_MAX_STR_WIDTH, "%.15g", json->number_val);
        s_append_cstr(out, num_buf);
        break;

    case CHJSON_BOOLEAN:
        s_append_cstr(out, json->bool_val ? "true" : "false");
        break;

    case CHJSON_NULL:
        s_append_cstr(out, "null");
        break;
    }
}

void json_to_string_spaced(json_t *json, string_t *out) {
    json_to_string_helper(json, out, true, 0);
}

void json_to_string(json_t *json, string_t *out) {
    json_to_string_helper(json, out, false, 0);
}
