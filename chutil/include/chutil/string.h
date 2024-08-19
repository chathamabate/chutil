
#ifndef CHUTIL_STR_H
#define CHUTIL_STR_H

#include <stdlib.h>

typedef struct _string_t {
    size_t cap;  // how large data is.
    size_t len;  // number of non-null bytes in our string.
                 // data[len] should always point to '\0'.
    char *data;
} string_t;

string_t *new_string(void);
string_t *new_string_from_cstr(const char *cstr);
void delete_string(string_t *s);

static inline size_t s_len(const string_t *s) {
    return s->len;
}

// These both return entirely new strings!
// end is exclusive.
string_t *s_substring(const string_t *s, size_t start, size_t end);

static inline string_t *s_copy(const string_t *s) {
    return new_string_from_cstr(s->data);
}

void s_append_char(string_t *s, char c);
void s_append_cstr(string_t *s, const char *addition);

static inline void s_append_string(string_t *s, const string_t *addition) {
    s_append_cstr(s, addition->data);
}

// As you can see...these don't do safety checks. Be careful!
static inline const char *s_get_cstr(string_t *s) {
    return s->data;
}

static inline void s_set_char(string_t *s, size_t i, char c) {
    s->data[i] = c;
}

static inline char s_get_char(const string_t *s, size_t i) {
    return s->data[i];
}

#endif
