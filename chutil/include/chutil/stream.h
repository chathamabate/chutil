

#ifndef CHUTIL_STREAM_H
#define CHUTIL_STREAM_H

#include <stdio.h>
#include "chutil/string.h"
#include "chutil/debug.h"

typedef enum _stream_state_t {
    STREAM_SUCCESS,
    STREAM_EMPTY,
    STREAM_ERROR

    // Maybe add more to these someday.
} stream_state_t;

typedef void (*stream_destructor_ft)(void *);

// input stream.....

// The given character pointer can be NULL.
typedef stream_state_t (*in_stream_next_char_ft)(void *, char *);
typedef stream_state_t (*in_stream_peek_char_ft)(void *, char *);

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

// Now for output stream.....

typedef stream_state_t (*out_stream_putc_ft)(void *, char c);

typedef struct _out_stream_impl_t {
    out_stream_putc_ft putc;
    stream_destructor_ft destructor;
} out_stream_impl_t;

typedef struct _out_stream_t {
    void *data;
    const out_stream_impl_t *impl;
} out_stream_t;

// The "builder' will NOT be owned by the created stream.
// It will simply be appended to.
out_stream_t *new_out_stream_to_string(string_t *builder);

// Returns NULL on error.
// attrs should be how the file will be openned.
// Probably "w" or "a". (i.e. do we want to create a new file or add to an 
// existing one)
out_stream_t *new_out_stream_to_file(const char *fn, const char *attrs);
void delete_out_stream(out_stream_t *os);

static inline stream_state_t os_putc(out_stream_t *os, char c) {
    return os->impl->putc(os->data, c);
}

stream_state_t os_puts(out_stream_t *os, const char *s);

typedef struct _string_out_stream_t {
    string_t *builder;
} string_out_stream_t;

string_out_stream_t *new_string_out_stream(string_t *builder);
static inline void delete_string_out_stream(string_out_stream_t *sos) {
    safe_free(sos);
}
stream_state_t sos_putc(string_out_stream_t *sos, char c);

typedef struct _file_out_stream_t {
    FILE *fp;
} file_out_stream_t;

file_out_stream_t *new_file_out_stream(const char *fn, const char *mode);
void delete_file_out_stream(file_out_stream_t *fos);
stream_state_t fos_putc(file_out_stream_t *fos, char c);

#endif
