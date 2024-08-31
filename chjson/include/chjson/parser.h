
#ifndef CHJSON_PARSER_H
#define CHJSON_PARSER_H

#include "chjson/json.h"
#include "chjson/json_helpers.h"
#include "chutil/stream.h"

typedef enum _parser_state_t {
    PARSER_SUCCESS = 0,
    PARSER_INPUT_STREAM_ERROR,
    PARSER_OUTPUT_STREAM_ERROR,
    PARSER_SYNTAX_ERROR
} parser_state_t;

const char *parser_state_to_literal(parser_state_t ps);

// On success, PARSER_SUCCESS will be returned and the created json_t *
// will be written to dest.
//
// On error, nothing is written to dest, corresponding error code is returned.
//
// NOTE: This parser does NOT require the input stream to be empty after
// parsing the json! If the input stream gives a number followed by a space,
// followed by jibberish, this is ok! The first time you call this function,
// the number will successfully be parsed.
parser_state_t json_from_in_stream(in_stream_t *is, json_t **dest);


#endif
