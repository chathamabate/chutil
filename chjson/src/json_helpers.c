
#include "chjson/json.h"
#include "chjson/json_helpers.h"
#include "chutil/map.h"
#include "chutil/string.h"
#include "chutil/list_helpers.h"

// Just going to use 2 spaces as a tab for now!

#define OS_PUTC(os, c) \
    state = os_putc(os, c); \
    if (state != STREAM_SUCCESS) { \
        return state; \
    }

#define OS_PUTS(os, s) \
    state = os_puts(os, s); \
    if (state != STREAM_SUCCESS) { \
        return state; \
    }

#define CHJSON_STRING_TAB "  "
#define TAB_OUT(os, tabs) \
    for (size_t i = 0; i < tabs; i++) { \
        OS_PUTS(os, CHJSON_STRING_TAB) \
    }


// If spaced is false, tabs is ignored.
static stream_state_t json_to_stream_helper(json_t *json, out_stream_t *os, 
        bool spaced, size_t tabs) {
    hash_map_t *hm;
    list_t *l;
    bool first;
    char num_buf[CHJSON_NUMBER_MAX_STR_WIDTH];
    stream_state_t state;

    switch (json->type) {
    case CHJSON_OBJECT: 
        hm = json->object_ptr;

        OS_PUTC(os, '{');
    
        // We don't append a comma on the first iteration!
        first = true;

        key_val_pair_t kvp;
        hm_reset_iterator(hm);
        while ((kvp = hm_next_kvp(hm)) != HASH_MAP_EXHAUSTED) {
            string_t *key = *(string_t **)kvp_key(hm, kvp);
            json_t *val = *(json_t **)kvp_val(hm, kvp);

            if (!first) {
                OS_PUTC(os, ',');
            } else {
                first = false;
            }

            if (spaced) {
                OS_PUTC(os, '\n');
                TAB_OUT(os, tabs + 1);
            }

            OS_PUTC(os, '\"');
            OS_PUTS(os, s_get_cstr(key));
            OS_PUTC(os, '\"');

            if (spaced) {
                OS_PUTC(os, ' ');
            }
            
            json_to_stream_helper(val, os, spaced, tabs + 1);
        }

        if (spaced) {
            OS_PUTC(os, '\n');
            TAB_OUT(os, tabs);
        }
        OS_PUTC(os, '}');
    
        break;

    case CHJSON_LIST:
        l = json->list_ptr;

        OS_PUTC(os, '[');

        first = true;
        json_t **ele_ptr;
        l_reset_iterator(l);
        while ((ele_ptr = l_next(l))) {
            if (!first) {
                OS_PUTC(os, ',');
            } else {
                first = false;
            }

            if (spaced) {
                OS_PUTC(os, '\n');
                TAB_OUT(os, tabs + 1);
            }

            json_to_stream_helper(*ele_ptr, os, spaced, tabs + 1);
        }
        
        if (spaced) {
            OS_PUTC(os, '\n');
            TAB_OUT(os, tabs);
        }
        OS_PUTC(os, ']');
        break;

    case CHJSON_STRING:
        OS_PUTC(os, '\"');
        OS_PUTS(os, s_get_cstr(json->string_ptr));
        OS_PUTC(os, '\"');
        break;

    case CHJSON_NUMBER:
        snprintf(num_buf, CHJSON_NUMBER_MAX_STR_WIDTH, "%.15g", json->number_val);
        OS_PUTS(os, num_buf);
        break;

    case CHJSON_BOOLEAN:
        OS_PUTS(os, json->bool_val ? "true" : "false");
        break;

    case CHJSON_NULL:
        OS_PUTS(os, "null");
        break;
    }

    return STREAM_SUCCESS;
}

stream_state_t json_to_stream(json_t *json, out_stream_t *os, bool spaced) {
    return json_to_stream_helper(json, os, spaced, 0);
}

string_t *json_to_string(json_t *json, bool spaced) {
    string_t *s = new_string();
    out_stream_t *os = new_out_stream_to_string(s);

    // Using a string builder should never fail!
    json_to_stream_helper(json, os, spaced, 0);
    delete_out_stream(os);

    return s;
}

static bool json_indirect_equals(json_t **json1, json_t **json2) {
    return json_equals(*json1, *json2);
}

bool json_equals(json_t *json1, json_t *json2) {
    if (json1->type != json2->type) {
        return false;
    }

    switch (json1->type) {
    case CHJSON_OBJECT:
        return hm_equals(json1->object_ptr, json2->object_ptr, 
                (hash_map_val_eq_ft)json_indirect_equals);
    case CHJSON_LIST:
        return l_equals(json1->list_ptr, json2->list_ptr, 
                (list_cell_equals_ft)json_indirect_equals);
    case CHJSON_STRING:
        return s_equals(json1->string_ptr, json2->string_ptr);
    case CHJSON_NUMBER:
        return json1->number_val == json2->number_val;
    case CHJSON_BOOLEAN:
        return json1->bool_val == json2->bool_val;
    case CHJSON_NULL:
        return true;
    };
}
