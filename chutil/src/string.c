
#include "chutil/string.h"
#include "chutil/debug.h"
#include <stdlib.h>
#include <string.h>

// Versatile constructor for shared_string.
// If cstr = NULL, len will be set to 0, and cap to 1.
// Otherwise, if cap < len + 1, cap will be set to len + 1.
static shared_string_t *new_shared_string(const char *cstr, size_t len, size_t cap) {
    if (!cstr) {
        cap = 1;
        len = 0;
    } else if (cap < len + 1) {
        cap = len + 1;
    }

    shared_string_t *ss = (shared_string_t *)safe_malloc(sizeof(shared_string_t));
    ss->ref_count = 1;
    ss->cap = cap;
    ss->len = len;
    ss->buf = (char *)safe_malloc(sizeof(char) * ss->cap);

    if (len > 0) {
        memcpy(ss->buf, cstr, len);
    }
    ss->buf[ss->len] = '\0';

    return ss;
}

string_t *new_string(void) {
    string_t *s = (string_t *)safe_malloc(sizeof(string_t));     
    s->from_literal = false;
    s->ss = new_shared_string(NULL, 0, 1);

    return s;
}

string_t *new_string_from_cstr(const char *cstr) {
    if (!cstr) {
        return new_string();
    }

    string_t *s = (string_t *)safe_malloc(sizeof(string_t));     
    s->from_literal = false;

    size_t cstr_len = strlen(cstr);
    s->ss = new_shared_string(cstr, cstr_len, cstr_len + 1);

    return s;
}

string_t *new_string_from_literal(const char *literal) {
    if (!literal) {
        return new_string();
    }

    // Should be the only place we construct a shared_literal.
    shared_literal_t *sl = (shared_literal_t *)safe_malloc(sizeof(shared_literal_t));
    sl->ref_count = 1;
    sl->len = strlen(literal);
    sl->literal = literal;

    string_t *s = (string_t *)safe_malloc(sizeof(string_t));
    s->from_literal = true;
    s->sl = sl;

    return s;
}

static void s_release_shared(string_t *s) {
    if (s->from_literal) {
        s->sl->ref_count--;
        if (s->sl->ref_count == 0) {
            safe_free(s->sl);
        }
    } else {
        s->ss->ref_count--;
        if (s->ss->ref_count == 0) {
            safe_free(s->ss->buf);
            safe_free(s->ss);
        }
    }
}

void delete_string(string_t *s) {
    s_release_shared(s);
    safe_free(s);
}

bool s_equals(const string_t *s1, const string_t *s2) {
    return strcmp(s_get_cstr(s1), s_get_cstr(s2)) == 0;
}

uint32_t s_hash(const string_t *s) {
    uint32_t hash_val = 23;    
    for (size_t i = 0; i < s_len(s); i++) {
        hash_val = (hash_val * 31) + s_get_char(s, i);
    }

    return hash_val;
}

string_t *s_substring(const string_t *s, size_t start, size_t end) {
    size_t cstr_len = s_len(s);
    const char *cstr = s_get_cstr(s);

    if (end > cstr_len) {
        end = cstr_len;
    }

    if (end <= start) {
        return new_string();
    }

    if (start == 0 && end == cstr_len) {
        return s_copy(s);
    }

    size_t len = end - start;
    shared_string_t *ss = new_shared_string(cstr, len, len + 1);

    string_t *sub_s = (string_t *)safe_malloc(sizeof(string_t));
    sub_s->from_literal = false;
    sub_s->ss = ss;
    
    return sub_s;
}

string_t *s_copy(const string_t *s) {
    string_t *copy = (string_t *)safe_malloc(sizeof(string_t));
    copy->from_literal = s->from_literal;

    if (s->from_literal) {
        copy->sl = s->sl;
        s->sl->ref_count++;
    } else {
        copy->ss = s->ss;
        s->ss->ref_count++;
    }

    return copy;
}

// Basically, we are going to modify the characters pointed to by s.
// After this call, we need to make sure s points to a buffer which is
// not pointed to by other strings, the capacity of said buffer is at least
// min_cap, AND s does NOT point to a string literal.
static void s_prepare_modify(string_t *s, size_t min_cap) {
    size_t len;
    shared_string_t *ss;

    // Length of underyling. 
    len = s_len(s);

    // Our minimum capacity will always be rounded up to
    // the length of the underlying string + 1.
    if (min_cap < len + 1) {
        min_cap = len + 1;
    }

    if (s->from_literal) {
        // We are preparing to our first modification off of a 
        // string literal, usage of the min_cap as is will be fine.
        ss = new_shared_string(s->sl->literal, len, min_cap);
        s_release_shared(s);

        s->from_literal = false;
        s->ss = ss;

        return;
    } 

    if (s->ss->ref_count > 1) {
        // Similar to string literal, if we must create a new
        // shared string, just use min_cap as is.
        ss = new_shared_string(s->ss->buf, len, min_cap);
        s_release_shared(s);
        s->ss = ss;

        return;
    }

    // If we make it here, we have ownership of an underlying buffer!
    // check if a resize is needed!

    if (s->ss->cap >= min_cap) {
        return;
    }

    // RESIZE!!

    // Try a simple double, otherwise just use the given min_cap.
    size_t new_cap = s->ss->cap * 2;
    if (min_cap > new_cap) {
        new_cap = min_cap;
    }

    s->ss->buf = (char *)safe_realloc(s->ss->buf, new_cap);
    s->ss->cap = new_cap;
}

void s_append_char(string_t *s, char c) {
    size_t min_cap = s_len(s) + 2;
    s_prepare_modify(s, min_cap);

    // After a prepare modify, we will never be pointing to 
    // a literal.

    s->ss->buf[s->ss->len] = c;
    s->ss->buf[s->ss->len + 1] = '\0';

    s->ss->len++;
}

void s_append_cstr(string_t *s, const char *addition) {
    size_t len = s_len(s);
    size_t add_len = strlen(addition);
    size_t min_cap = len + add_len + 1;
    s_prepare_modify(s, min_cap);

    strcpy(&(s->ss->buf[len]), addition);
    s->ss->len = len + add_len;
}

void s_set_char(string_t *s, size_t i, char c) {
    size_t len = s_len(s);   
    s_prepare_modify(s, len + 1);
    s->ss->buf[i] = c;
}

void s_print_debug(string_t *s) {
    if (s->from_literal) {
        printf("Literal @ %p (RC = %zu, Len = %zu): %s\n", 
                (void *)(s->sl), s->sl->ref_count, s->sl->len, s->sl->literal);
    } else {
        printf("Shared @ %p (RC = %zu, Len = %zu, Cap = %zu): %s\n", 
                (void *)(s->ss), s->ss->ref_count, s->ss->len, s->ss->cap, s->ss->buf);
    }
}
