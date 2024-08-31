
#include "chjson/parser.h"
#include "chjson/json.h"
#include "chjson/json_helpers.h"
#include "chutil/map.h"
#include "chutil/utf8.h"
#include <assert.h>
#include <stdlib.h>

const char *parser_state_to_literal(parser_state_t ps) {
    switch (ps) {
    case PARSER_SUCCESS:
        return "Parser Success";
    case PARSER_INPUT_STREAM_ERROR:
        return "Parser Input Stream Error";
    case PARSER_SYNTAX_ERROR:
        return "Parser Syntax Error";
    default:
        return "Unknown Error";
    }
}

#define ASSERT_NOT_EMPTY(ss) \
    do { \
        if ((ss) == STREAM_EMPTY) { \
            return PARSER_SYNTAX_ERROR; \
        } \
        if ((ss) != STREAM_SUCCESS) { \
            return PARSER_INPUT_STREAM_ERROR; \
        } \
    } while (0)

#define ASSERT_CHAR_FOUND(ss, expected, actual) \
    do { \
        ASSERT_NOT_EMPTY(ss); \
        if ((expected) != (actual)) { \
            return PARSER_SYNTAX_ERROR; \
        } \
    } while (0)

#define ASSERT_VALID_PARSE(ps) \
    do { \
        if ((ps) != PARSER_SUCCESS) { \
            return (ps); \
        } \
    } while (0)

// Use this macro when you have previously peeked a character
// and now want to consume it.
// The input stream should NOT return anything other than STREAM_SUCCESS
// since we peeked a character earlier.
#define CONSUME_LAH(is) \
    do { \
        stream_state_t __ss = is_next_char(is, NULL); \
        if (__ss != STREAM_SUCCESS) { \
            return PARSER_INPUT_STREAM_ERROR; \
        } \
    } while (0)

// This macro uses the trim_ws function below.
// basically propegating up the error if there is one!
#define TRIM_WS(is) \
    do { \
        parser_state_t __ps = trim_ws(is); \
        if (__ps != PARSER_SUCCESS) { \
            return __ps; \
        } \
    } while (0)

// Defining these more important function signatures up here...
// _ denotes a wrapped function to help with error handling.
static parser_state_t _string_from_in_stream_no_trim(in_stream_t *is, string_t *builder);
static parser_state_t string_from_in_stream_no_trim(in_stream_t *is, string_t **dest);
static parser_state_t json_string_from_in_stream_no_trim(in_stream_t *is, json_t **dest);

static parser_state_t json_number_from_stream_no_trim(in_stream_t *is, json_t **dest);

static parser_state_t _json_list_from_stream_no_trim(in_stream_t *is, list_t *l);
static parser_state_t json_list_from_stream_no_trim(in_stream_t *is, json_t **dest);

static parser_state_t _json_kvp_from_stream_no_trim(in_stream_t *is, string_t *k_builder, json_t **v_dest);
static parser_state_t json_kvp_from_stream_no_trim(in_stream_t *is, hash_map_t *hm);

static parser_state_t _json_object_from_stream_no_trim(in_stream_t *is, hash_map_t *hm);
static parser_state_t json_object_from_stream_no_trim(in_stream_t *is, json_t **dest);

static parser_state_t json_from_in_stream_no_trim(in_stream_t *is, json_t **dest);

// Definitions --------------------------------

static parser_state_t expect_cstr(in_stream_t *is, const char *cstr) {
    size_t len = strlen(cstr); 
    stream_state_t ss;
    char c;

    for (size_t i = 0; i < len; i++) {
        ss = is_next_char(is, &c);
        ASSERT_CHAR_FOUND(ss, cstr[i], c);
    }

    return PARSER_SUCCESS;
}

static parser_state_t trim_ws(in_stream_t *is) {
    stream_state_t ss;
    char c;

    // WhiteSpace: 0x20, 0x0A, 0x0D, 0x09

    while (true) {
        ss = is_peek_char(is, &c);
        if (ss == STREAM_EMPTY) {
            return PARSER_SUCCESS;
        }
        if (ss != STREAM_SUCCESS) {
            return PARSER_INPUT_STREAM_ERROR;
        }
        if (c != 0x20 && c != 0x0A && c != 0x0D && c != 0x09) {
            return PARSER_SUCCESS;
        }

        // We should ALWAYS expect a character here as we peeked one above,
        // anything other than success... we'll through an input stream error.
        ss = is_next_char(is, NULL);
        if (ss != STREAM_SUCCESS) {
            return PARSER_INPUT_STREAM_ERROR;
        }
    }
}

// We've read a backslash, now let's just expect what must be after.
// Append everything to builder.
static parser_state_t expect_control_suffix(in_stream_t *is, string_t *builder) {
    stream_state_t ss;
    char c;
    char hex_digits[4];

    ss = is_next_char(is, &c);
    ASSERT_NOT_EMPTY(ss);

    switch (c) {
    case '\\':
        s_append_char(builder, '\\');
        return PARSER_SUCCESS;

    case '/':
        // slash is not a c control character.
        s_append_char(builder, '/');
        return PARSER_SUCCESS;

    case 'b':
        s_append_char(builder, '\b');
        return PARSER_SUCCESS;

    case 'f':
        s_append_char(builder, '\f');
        return PARSER_SUCCESS;

    case 'n':
        s_append_char(builder, '\n');
        return PARSER_SUCCESS;

    case 'r':
        s_append_char(builder, '\r');
        return PARSER_SUCCESS;

    case 't':
        s_append_char(builder, '\t');
        return PARSER_SUCCESS;

    case 'u':
        // unicode case!
        for (size_t i = 0; i < 4; i++) {
            ss = is_next_char(is, &c);
            ASSERT_NOT_EMPTY(ss);

            bool is_hex_digit = 
                ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'f') ||
                ('A' <= c && c <= 'F');

            if (!is_hex_digit) {
                return PARSER_SYNTAX_ERROR;
            }

            hex_digits[i] = c;
        }

        unicode_t uc = unicode_from_cstr(hex_digits);

        // Little inefficient here... alas.
        out_stream_t *os = new_out_stream_to_string(builder);
        stream_state_t ss = unicode_to_utf8(os, uc); 
        delete_out_stream(os);

        // Since we're outputing to a string, the output stream
        // should never ever fail. This if statement isn't really needed.
        if (ss != STREAM_SUCCESS) {
            return PARSER_OUTPUT_STREAM_ERROR;
        }

        return PARSER_SUCCESS;

    default:
        return PARSER_SYNTAX_ERROR;
    }
}

static parser_state_t _string_from_in_stream_no_trim(in_stream_t *is, string_t *builder) {
    char c;
    stream_state_t ss;
    parser_state_t ps;

    ss = is_next_char(is, &c);
    ASSERT_CHAR_FOUND(ss, '\"', c);

    // Character: 0x20 ... (without \ or ")
    // Escape Chars: " \ / b f n r t uhhhh
    // Surrounded by ""
    
    while (true) {
        ss = is_next_char(is, &c);
        ASSERT_NOT_EMPTY(ss);

        // CHARACTERS ARE SIGNED!
        if (0x0 <= c && c < 0x20) {
            return PARSER_SYNTAX_ERROR;
        }

        if (c == '\"') {
            return PARSER_SUCCESS;
        }

        // NOTE: when the string is converted to a json object.
        // Escapced characters will be translated into their corresponding
        // C control characters.
        if (c == '\\') {
            ps = expect_control_suffix(is, builder);
            if (ps != PARSER_SUCCESS) {
                return ps;
            }
        } else {
            s_append_char(builder, c);
        }
    }
}

static parser_state_t string_from_in_stream_no_trim(in_stream_t *is, string_t **dest) {
    string_t *builder = new_string();

    parser_state_t ps = _string_from_in_stream_no_trim(is, builder);
    if (ps != PARSER_SUCCESS) {
        delete_string(builder);
        return ps;
    }

    *dest = builder;
    return PARSER_SUCCESS;
}

static parser_state_t json_string_from_in_stream_no_trim(in_stream_t *is, json_t **dest) {
    string_t *res;

    parser_state_t ps = string_from_in_stream_no_trim(is, &res);
    ASSERT_VALID_PARSE(ps);

    *dest = new_json_string(res);
    return PARSER_SUCCESS;
}

static parser_state_t json_number_from_stream_no_trim(in_stream_t *is, json_t **dest) {
    char buf[CHJSON_NUMBER_MAX_STR_WIDTH + 1];
    memset(buf, 0, CHJSON_NUMBER_MAX_STR_WIDTH + 1);

    stream_state_t ss;
    char c;
    size_t i;

    // We are lucky there are no variables in JSON.
    // We can basically add as many number-like characters to our
    // buffer as we want, then rely on strtod to do the actual parsing!
    //
    // Either a comma or nothing much come after a number in JSON,
    // commas will never appear in a double!


    for (i = 0; i < CHJSON_NUMBER_MAX_STR_WIDTH; i++) {
        ss = is_peek_char(is, &c);
        if (ss == STREAM_EMPTY) {
            break;
        }

        if (ss != STREAM_SUCCESS) {
            return PARSER_INPUT_STREAM_ERROR;
        }

        // I Believe these are the only characters which can ever appear in a 
        // JSON number.
        bool is_number_like = 
            (c == 'e' || c =='E') ||
            ('0' <= c && c <= '9') ||
            (c == '+' || c == '-') ||
            (c == '.');

        if (!is_number_like) {
            break;
        }

        buf[i] = c;

        ss = is_next_char(is, NULL);
        if (ss != STREAM_SUCCESS) {
            // We know the stream isn't empty here,
            // since we peeked a character above.
            return PARSER_INPUT_STREAM_ERROR;
        }
    }

    // NOTE: when the above loop exits, i will be the index of the NULL
    // terminator!

    if (i == 0) {
        return PARSER_SYNTAX_ERROR;
    }

    char *endptr;
    double res = strtod(buf, &endptr);

    if (&(buf[i]) != endptr) {
        return PARSER_SYNTAX_ERROR;
    }
    
    *dest = new_json_number(res);
    return PARSER_SUCCESS;
}

static parser_state_t _json_list_from_stream_no_trim(in_stream_t *is, list_t *l) {
    stream_state_t ss; 
    parser_state_t ps;
    json_t *ele;

    char c;
    ss = is_next_char(is, &c);
    ASSERT_CHAR_FOUND(ss, '[', c);
    TRIM_WS(is);

    ss = is_peek_char(is, &c);
    ASSERT_NOT_EMPTY(ss);

    // Empty list case.
    if (c == ']') {
        CONSUME_LAH(is);
        return PARSER_SUCCESS;
    }

    // Otherwise, there must be a value!
    ps = json_from_in_stream_no_trim(is, &ele);
    ASSERT_VALID_PARSE(ps);

    l_push(l, &ele);


    // Now look for commas followed by values!
    while (true) {
        TRIM_WS(is);

        // There MUST be a character if we are here!
        ss = is_next_char(is, &c);
        ASSERT_NOT_EMPTY(ss);

        if (c == ']') {
            return PARSER_SUCCESS;
        }

        if (c != ',') {
            return PARSER_SYNTAX_ERROR;
        }

        TRIM_WS(is);

        ps = json_from_in_stream_no_trim(is, &ele);
        ASSERT_VALID_PARSE(ps);

        l_push(l, &ele);
    }
}

static parser_state_t json_list_from_stream_no_trim(in_stream_t *is, json_t **dest) {
    json_t *json = new_json_list();
    parser_state_t ps = _json_list_from_stream_no_trim(is, json->list_ptr);

    if (ps != PARSER_SUCCESS) {
        delete_json(json);
        return ps;
    }

    *dest = json;
    return PARSER_SUCCESS;
}

static parser_state_t _json_kvp_from_stream_no_trim(in_stream_t *is, string_t *k_builder, json_t **v_dest) {
    parser_state_t ps;
    stream_state_t ss;
    json_t *val;
    char c;

    ps = _string_from_in_stream_no_trim(is, k_builder);
    ASSERT_VALID_PARSE(ps);
    TRIM_WS(is);

    ss = is_next_char(is, &c);
    ASSERT_CHAR_FOUND(ss, ':', c);
    TRIM_WS(is);

    ps = json_from_in_stream_no_trim(is, &val);
    ASSERT_VALID_PARSE(ps);
    *v_dest = val;

    return PARSER_SUCCESS;
}

static parser_state_t json_kvp_from_stream_no_trim(in_stream_t *is, hash_map_t *hm) {
    string_t *k_builder = new_string();
    json_t *v_json = NULL;

    parser_state_t ps = _json_kvp_from_stream_no_trim(is, k_builder, &v_json);
    if (ps != PARSER_SUCCESS) {
        delete_string(k_builder);
        if (v_json) {
            delete_json(v_json);
        }
        return ps;
    }

    hm_put(hm, &k_builder, &v_json);
    return PARSER_SUCCESS;
}

static parser_state_t _json_object_from_stream_no_trim(in_stream_t *is, hash_map_t *hm) {
    parser_state_t ps;
    stream_state_t ss;
    char c;

    ss = is_next_char(is, &c);
    ASSERT_CHAR_FOUND(ss, '{', c);
    TRIM_WS(is);

    // Like list, we need to look for the first value, outside of our loop.
    ss = is_peek_char(is, &c);
    ASSERT_NOT_EMPTY(ss);

    if (c == '}') {
        CONSUME_LAH(is);
        return PARSER_SUCCESS;
    }

    ps = json_kvp_from_stream_no_trim(is, hm);
    ASSERT_VALID_PARSE(ps);

    while (true) {
        TRIM_WS(is);

        ss = is_next_char(is, &c);
        ASSERT_NOT_EMPTY(ss);

        if (c == '}') {
            return PARSER_SUCCESS;
        }

        if (c != ',') {
            return PARSER_SYNTAX_ERROR;
        }

        TRIM_WS(is);
        ps = json_kvp_from_stream_no_trim(is, hm);
        ASSERT_VALID_PARSE(ps);
    }
}

static parser_state_t json_object_from_stream_no_trim(in_stream_t *is, json_t **dest) {
    json_t *json = new_json_object();
    parser_state_t ps = _json_object_from_stream_no_trim(is, json->object_ptr);
    if (ps != PARSER_SUCCESS) {
        delete_json(json);
        return ps;
    }
    *dest = json;
    return PARSER_SUCCESS;
}

// This will NOT trim white space before looking for a json value.
static parser_state_t json_from_in_stream_no_trim(in_stream_t *is, json_t **dest) {
    parser_state_t ps;
    stream_state_t ss;
    char c;

    json_t *res;

    ss = is_peek_char(is, &c);
    ASSERT_NOT_EMPTY(ss);

    switch (c) {
    case '{':
        ps = json_object_from_stream_no_trim(is, &res);
        ASSERT_VALID_PARSE(ps);
        break;
    case '[':
        ps = json_list_from_stream_no_trim(is, &res);
        ASSERT_VALID_PARSE(ps);
        break;
    case '\"':
        ps = json_string_from_in_stream_no_trim(is, &res);
        ASSERT_VALID_PARSE(ps);
        break;
    case 'f':
        ps = expect_cstr(is, "false");
        ASSERT_VALID_PARSE(ps);
        res = new_json_boolean(false);
        break;
    case 't':
        ps = expect_cstr(is, "true");
        ASSERT_VALID_PARSE(ps);
        res = new_json_boolean(true);
        break;
    case 'n':
        ps = expect_cstr(is, "null");
        ASSERT_VALID_PARSE(ps);
        res = json_null();
        break;
    default:
        // By default we attempt to parse a number, this is because
        // checking if the character is a digit doesn't fit nicely into this
        // switch statement.
        ps = json_number_from_stream_no_trim(is, &res);
        ASSERT_VALID_PARSE(ps);
        break;
    }

    *dest = res;
    return PARSER_SUCCESS;
}

parser_state_t json_from_in_stream(in_stream_t *is, json_t **dest) {
    TRIM_WS(is);
    return json_from_in_stream_no_trim(is, dest);
}
