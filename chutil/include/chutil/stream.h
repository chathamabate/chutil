

#ifndef CHUTIL_STREAM_H
#define CHUTIL_STREAM_H

#include <stdio.h>
#include "chutil/string.h"

typedef enum _stream_state_t {
    STREAM_SUCCES,
    STREAM_EMPTY,
    STREAM_ERROR

    // Maybe add more to these someday.
} stream_state_t;

// The given character pointer can be NULL.
typedef stream_state_t (*in_stream_next_char_ft)(void *, char *);
typedef stream_state_t (*in_stream_peek_char_ft)(void *, char *);
typedef void (*stream_destructor_ft)(void *);

typedef struct _in_stream_impl_t {
    in_stream_peek_char_ft peek_char;
    in_stream_next_char_ft next_char; 
    stream_destructor_ft destructor;
} in_stream_impl_t;

typedef struct _in_stream_t {
    void *data;
    const in_stream_impl_t *impl;
} in_stream_t;

// NOTE: the create in_stream will own s!
// s will be deleted when destructing the in stream.
in_stream_t *new_in_stream_from_string(string_t *s);

// Returns NULL if there was an error openning the file.
in_stream_t *new_in_stream_from_file(const char *fn);
void delete_in_stream(in_stream_t *is);

static inline stream_state_t is_peek_char(in_stream_t *is, char *out) {
    return is->impl->peek_char(is->data, out); 
}

static inline stream_state_t is_next_char(in_stream_t *is, char *out) {
    return is->impl->next_char(is->data, out); 
}

typedef struct _string_in_stream_t {
    size_t i;
    string_t *s;
} string_in_stream_t;

string_in_stream_t *new_string_in_stream(string_t *s);
void delete_string_in_stream(string_in_stream_t *sis);
stream_state_t sis_peek_char(string_in_stream_t *sis, char *out);
stream_state_t sis_next_char(string_in_stream_t *sis, char *out);

typedef struct _file_in_stream_t {
    FILE *fp;
    int lah;   // look-ahead (Will be EOF when we've finished the file)
} file_in_stream_t;

// Returns NULL if error when openning file.
file_in_stream_t *new_file_in_stream(const char *fn);
void delete_file_in_stream(file_in_stream_t *fis);
stream_state_t fis_peek_char(file_in_stream_t *fis, char *out);
stream_state_t fis_next_char(file_in_stream_t *fis, char *out);

#endif
