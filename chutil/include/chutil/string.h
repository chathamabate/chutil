
#ifndef CHUTIL_STRING_H
#define CHUTIL_STRING_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct _shared_string_t {
    // The number of people using this string at the moment.
    size_t ref_count;

    size_t cap;
    size_t len;
    char *buf;
} shared_string_t;

// This makes sure the length is only calcuated once of a literal.
typedef struct _shared_literal_t {
    size_t ref_count;
    size_t len;
    const char *literal;
} shared_literal_t;

typedef struct _string_t {
    bool from_literal; // Which ever 
    union {
        shared_literal_t *sl;
        shared_string_t *ss; 
    };
} string_t;

string_t *new_string(void);
string_t *new_string_from_cstr(const char *cstr);
string_t *new_string_from_literal(const char *literal);
void delete_string(string_t *s);

bool s_equals(const string_t *s1, const string_t *s2);
uint32_t s_hash(const string_t *s);

// These "indirect functions are meant to help you when using hashmaps.
// For example in a map, we are likely to store string_t *'s, not string_t's.
// Thus, they will be passed around as string_t **'s
static inline bool s_indirect_equals(const string_t * const *s1, const string_t * const *s2) {
    return s_equals(*s1, *s2);
}

static inline uint32_t s_indirect_hash(const string_t * const *s) {
    return s_hash(*s);
}

static inline size_t s_len(const string_t *s) {
    return s->from_literal ? s->sl->len : s->ss->len;
}

// These both return entirely new strings!
// end is exclusive.
string_t *s_substring(const string_t *s, size_t start, size_t end);
string_t *s_copy(const string_t *s);

void s_append_char(string_t *s, char c);
void s_append_cstr(string_t *s, const char *addition);

// make sure you don't use the returned cstr for longer than s's lifetime!
// Also, do not modify the returned cstring! This function is meant for
// conveint reading only!
static inline const char *s_get_cstr(const string_t *s) {
    return s->from_literal ? s->sl->literal : s->ss->buf;
}

static inline void s_append_string(string_t *s, const string_t *addition) {
    s_append_cstr(s, s_get_cstr(addition));
}


// Neither of these check the index, so be careful!
void s_set_char(string_t *s, size_t i, char c);

static inline char s_get_char(const string_t *s, size_t i) {
    return s_get_cstr(s)[i];
}

#endif
