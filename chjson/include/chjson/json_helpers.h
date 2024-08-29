
#ifndef CHJSON_JSON_HELPERS_H
#define CHJSON_JSON_HELPERS_H

#include "chjson/json.h"
#include "chutil/stream.h"

#define CHJSON_NUMBER_MAX_STR_WIDTH 26

// Returns STREAM_SUCCESS on success.
stream_state_t json_to_stream(json_t *json, out_stream_t *os, bool spaced);

string_t *json_to_string(json_t *json, bool spaced);

bool json_equals(json_t *json1, json_t *json2);

#endif
