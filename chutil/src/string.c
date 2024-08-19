
#include "chutil/string.h"
#include "chutil/debug.h"
#include <stdlib.h>
#include <string.h>

string_t *new_string(void) {
    string_t *s = (string_t *)safe_malloc(sizeof(string_t));     

    s->cap = 1;
    s->len = 0;
    s->data = (char *)safe_malloc(sizeof(char) * s->cap);
    s->data[0] = '\0';

    return s;
}

string_t *new_string_from_cstr(const char *cstr) {
    if (!cstr) {
        return new_string();
    }

    string_t *s = (string_t *)safe_malloc(sizeof(string_t));     

    size_t len = strlen(cstr);
    s->cap = len + 1;
    s->len = len;
    s->data = (char *)safe_malloc(sizeof(char) * s->cap);

    strcpy(s->data, cstr);

    return s;
    
}

void delete_string(string_t *s) {
    safe_free(s->data);
    safe_free(s);
}

string_t *s_substring(const string_t *s, size_t start, size_t end) {
    if (end > s->len) {
        end = s->len;
    }

    if (end <= start) {
        return new_string();
    }

    size_t len = end - start;

    string_t *sub_s = (string_t *)safe_malloc(sizeof(string_t));
    sub_s->cap = len + 1;
    sub_s->len = len;
    sub_s->data = (char *)safe_malloc(sizeof(char) * sub_s->cap);

    memcpy(sub_s->data, &(s->data[start]), len);
    sub_s->data[sub_s->len] = '\0';
    
    return sub_s;
}

// Take s, and expand it to have a capacity of at least min_cap.
static void s_expand(string_t *s, size_t min_cap) {
    if (s->cap >= min_cap) {
        return;
    }

    // for now, we'll just do a simple double.
    s->cap = min_cap * 2;
    s->data = safe_realloc(s->data, sizeof(char) * s->cap);
}

void s_append_char(string_t *s, char c) {
    size_t min_cap = s->len + 2; // c + \0.
    s_expand(s, min_cap);

    s->len++;
    
    s->data[s->len - 1] = c;
    s->data[s->len] = '\0';
}

void s_append_cstr(string_t *s, const char *addition) {
    size_t add_len = strlen(addition);
    size_t min_cap = s->len + add_len + 1;
    s_expand(s, min_cap);

    strcpy(&(s->data[s->len]), addition);
    s->len = s->len + add_len;
}

