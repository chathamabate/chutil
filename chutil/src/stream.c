
#include "chutil/stream.h"
#include "chutil/debug.h"
#include "chutil/string.h"
#include <stdio.h>

static const in_stream_impl_t STRING_IN_STREAM_IMPL = {
    .peek_char = (in_stream_peek_char_ft)sis_peek_char,
    .next_char = (in_stream_next_char_ft)sis_next_char,
    .destructor = (stream_destructor_ft)delete_string_in_stream
};

static const in_stream_impl_t FILE_IN_STREAM_IMPL = {
    .next_char = (in_stream_next_char_ft)fis_next_char,
    .peek_char = (in_stream_peek_char_ft)fis_peek_char,
    .destructor = (stream_destructor_ft)delete_file_in_stream,
};

in_stream_t *new_in_stream_from_string(string_t *s) {
    string_in_stream_t *sis = new_string_in_stream(s);

    in_stream_t *is = (in_stream_t *)safe_malloc(sizeof(in_stream_t));
    is->data = sis;
    is->impl = &STRING_IN_STREAM_IMPL;

    return is;
}

in_stream_t *new_in_stream_from_file(const char *fn) {
    file_in_stream_t *fis = new_file_in_stream(fn);  
    if (!fis) {
        return NULL;
    }

    in_stream_t *is = (in_stream_t *)safe_malloc(sizeof(in_stream_t));
    is->data = fis;
    is->impl = &FILE_IN_STREAM_IMPL;

    return is;
}

void delete_in_stream(in_stream_t *is) {
    is->impl->destructor(is->data);
    safe_free(is);
}

string_in_stream_t *new_string_in_stream(string_t *s) {
    string_in_stream_t *sis = safe_malloc(sizeof(string_in_stream_t));
    sis->s = s;
    sis->i = 0;

    return sis;
}

void delete_string_in_stream(string_in_stream_t *sis) {
    delete_string(sis->s);
    safe_free(sis);
}

stream_state_t sis_peek_char(string_in_stream_t *sis, char *out) {
    if (sis->i >= s_len(sis->s)) {
        return STREAM_EMPTY;
    }

    if (out) {
        *out = s_get_char(sis->s, sis->i);
    }

    return STREAM_SUCCES;
}

stream_state_t sis_next_char(string_in_stream_t *sis, char *out) {
    if (sis->i >= s_len(sis->s)) {
        return STREAM_EMPTY;
    }

    if (out) {
        *out = s_get_char(sis->s, sis->i);
    }

    sis->i++;

    return STREAM_SUCCES;
}


file_in_stream_t *new_file_in_stream(const char *fn) {
    FILE *fp = fopen(fn, "r");
    if (!fp) {
        return NULL;
    }

    file_in_stream_t *fis = (file_in_stream_t *)safe_malloc(sizeof(file_in_stream_t));
    fis->fp = fp;
    fis->lah = fgetc(fp);

    return fis;
}

void delete_file_in_stream(file_in_stream_t *fis) {
    fclose(fis->fp);
    safe_free(fis);
}

stream_state_t fis_peek_char(file_in_stream_t *fis, char *out) {
    if (fis->lah == EOF) {
        return STREAM_EMPTY;
    }

    if (out) {
        *out = (char)(fis->lah);
    }

    return STREAM_SUCCES;
}

stream_state_t fis_next_char(file_in_stream_t *fis, char *out) {
    if (fis->lah == EOF) {
        return STREAM_EMPTY;
    }

    if (out) {
        *out = (char)(fis->lah);
    }

    fis->lah = fgetc(fis->fp);

    return STREAM_SUCCES;
}

