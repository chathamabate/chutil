
#ifndef CHJSON_JSON_H
#define CHJSON_JSON_H

#include <stdbool.h>
#include <stdarg.h>

#include <chutil/map.h>
#include <chutil/list.h>
#include <chutil/string.h>

typedef enum _json_type_t {
    CHJSON_OBJECT,
    CHJSON_LIST,
    CHJSON_STRING,
    CHJSON_NUMBER,
    CHJSON_BOOLEAN,
    CHJSON_NULL,
} json_type_t;

typedef struct _json_t {
    json_type_t type;

    union {
        // Map: string_t * -> json_t *
        hash_map_t *object_ptr;
        // List: json_t *
        list_t *list_ptr;
        string_t *string_ptr;

        double number_val;
        bool bool_val;
    };
} json_t;

// Because the user will have direct address to the underlying types 
// of each json object, I will not be using dynamic dispatch.
//
// Only a few behaovirs needed, Dynamic dispatch was kinda overkill.

typedef struct _json_key_val_pair_t {
    const char *key;     // This will be copied into a new string_t
    json_t *val;
} json_key_val_pair_t;

static inline json_key_val_pair_t json_kvp(const char *key, json_t *val) {
    json_key_val_pair_t kvp = {
        .key = key, .val = val
    };
    return kvp;
}

json_t *new_json_object(void);
json_t *new_json_object_from_kvps(size_t num_pairs, ...);

json_t *new_json_list(void);
json_t *new_json_list_from_eles(size_t num_eles, ...);

// NULL will result in an empty json string.
json_t *new_json_string(const char *s);

json_t *new_json_number(double n);
json_t *new_json_boolean(bool b);

// There will be a singleton json null type.
// DON'T delete or modify this, it will live in static memory.
json_t *json_null(void);


// NOTE: IMPORTANT!!!!!!
// When you delete a json object (Excpet for the NULL object),
// EVERYTHING WILL BE DELETED. This object assumes ownership of
// all of its parts!
//
// So, be careful when using lists and objects.
// Make sure you don't put the same object in two different jsons!
// When putting a key into an object, always use a new string_t!
//
// NOTE: the null json can be passed into here, it will simply be ignored.
void delete_json(json_t *json);

static inline json_type_t json_type(json_t *json) {
    return json->type;
}

// These functions are for updating the underlying data
// found in a json object.
//
// They will return NULL if the given json object is not the
// correct type.

hash_map_t *json_as_object(json_t *json);
list_t *json_as_list(json_t *json);
string_t *json_as_string(json_t *json);
double *json_as_number(json_t *json);
bool *json_as_bool(json_t *json);

// The given json in string form will be appended to
// the given string out.
void json_to_string_spaced(json_t *json, string_t *out);
void json_to_string(json_t *json, string_t *out);

#endif
