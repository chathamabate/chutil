
#ifndef CHJSON_JSON_H
#define CHJSON_JSON_H

#include <stdbool.h>
#include <stdarg.h>

#include "chutil/map.h"
#include "chutil/list.h"
#include "chutil/string.h"

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

// Anyway to make the below end points even easier to use???

json_t *new_json_object(void);

// This below function and macro expect a non zero number of alternating
// keys and values.
//
// Each key should be a string_t * which will be owned by the resulting json object.
// Each value should be a json_t * which will be owned by the resulting json object.

json_t *_new_json_object_from_kvps(int dummy, ...);
#define new_json_object_from_kvps(...) _new_json_object_from_kvps(0, __VA_ARGS__, NULL)

json_t *new_json_list(void);
json_t *_new_json_list_from_eles(int dummy, ...);
#define new_json_list_from_eles(...) _new_json_list_from_eles(0, __VA_ARGS__, NULL)

// The new json object will own the given string.
json_t *new_json_string(string_t *s);

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

#endif
