
#ifndef CHJSON_JSON_HELPERS_H
#define CHJSON_JSON_HELPERS_H

#include "chjson/json.h"
#include "chutil/stream.h"

#define CHJSON_NUMBER_MAX_STR_WIDTH 26

// NOTE: Control characters will be switched to their 
// escaped versions.
stream_state_t json_to_stream(json_t *json, out_stream_t *os, bool spaced);

string_t *json_to_string(json_t *json, bool spaced);

bool json_equals(json_t *json1, json_t *json2);

// The below three calls can accept NULL for the json element,
// in this case NULL is returned.
// This is to allow for chaining.

// NOTE: there is no key_literal here... the key cstr given will
// not be modified in anyway or stored anywhere. 
//
// It will temporarily be treated as a literal for efficiency.
json_t *json_lookup_key_cstr(json_t *json, const char *key_cstr);
json_t *json_lookup_key(json_t *json, string_t *key);
json_t *json_lookup_index(json_t *json, size_t ind);

#endif
