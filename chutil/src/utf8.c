
#include "chutil/stream.h"
#include <chutil/utf8.h>

unicode_t unicode_from_cstr(const char *cstr) {
    unicode_t res = 0;

    char c;
    uint8_t dec;
    for (size_t i = 0; i < 4; i++) {
        res <<= 4; 

        c = cstr[i];
        if ('0' <= c && c <= '9') {
            dec = c - '0';
        } else if ('a' <= c && c <= 'f') {
            dec = c - 'a' + 10;
        } else if ('A' <= c && c <= 'F') {
            dec = c - 'A' + 10;
        } else {
            dec = 0;
        }

        res |= dec;
    }

    return res;
}

#define TRY_CALL(stream_exp) \
    do { \
        stream_state_t __ss = stream_exp; \
        if (__ss != STREAM_SUCCESS) { \
            return __ss; \
        } \
    } while (0)

stream_state_t unicode_to_utf8(out_stream_t *os, unicode_t uc) {
    uint8_t parts[4] = {
        (0x000F & uc),
        (0x00F0 & uc) >> 4,
        (0x0F00 & uc) >> 8,
        (0xF000 & uc) >> 12,
    };

    if (uc < 0x0080) {
        TRY_CALL(os_putc(os, (char)(
            (parts[1] << 4) | parts[0] // Inefficient, but true to the wiki
        )));
    } else if (uc < 0x0800) {
        TRY_CALL(os_putc(os, (char)(
            0xC0 | (parts[2] << 2) | (parts[1] >> 2)
        )));
        TRY_CALL(os_putc(os, (char)(
            0x80 | ((0x3 & parts[1]) << 4) | parts[0]
        )));
    } else {
        TRY_CALL(os_putc(os, (char)(
            0xE0 | parts[3]
        )));
        TRY_CALL(os_putc(os, (char)(
            0x80 | (parts[2] << 2) | (parts[1] >> 2)
        )));
        TRY_CALL(os_putc(os, (char)(
            0x80 | ((0x3 & parts[1]) << 4) | parts[0]
        )));
    }

    return STREAM_SUCCESS;
}


stream_state_t unicode_from_utf8(in_stream_t *is, unicode_t *uc) {
    // Kinda a reverse of above.
    // bytes[0] will be the leading byte.
    uint8_t bytes[3];
    TRY_CALL(is_next_char(is, (char *)(&(bytes[0]))));

    // Kinda complex way to say starts with a 0.
    if (!((bytes[0] >> 7) ^ 0x0)) {
        *uc = bytes[0];
        return STREAM_SUCCESS; 
    }

    // 2 character UTF-8 Case. (3 hex characters)
    if (!((bytes[0] >> 5) ^ 0x6)) {
        TRY_CALL(is_next_char(is, (char *)(&(bytes[1]))));
        if (((bytes[1] >> 6) ^ 0x2)) {
            *uc = (unicode_t)' ';
            return STREAM_SUCCESS;
        }

        *uc = (bytes[1] & 0x3F) |
            ((bytes[0] & 0x1F) << 6);

        return STREAM_SUCCESS;
    }

    if (!((bytes[0] >> 4) ^ 0xE)) {
        TRY_CALL(is_next_char(is, (char *)(&(bytes[1]))));
        TRY_CALL(is_next_char(is, (char *)(&(bytes[2]))));

        if (((bytes[1] >> 6) ^ 0x2) || ((bytes[2] >> 6) ^ 0x2)) {
            *uc = (unicode_t)' ';
            return STREAM_SUCCESS;
        }

        *uc = (bytes[2] & 0x3F) |
            ((bytes[1] & 0x3F) << 6) |
            ((bytes[0] & 0xF) << 12);

        return STREAM_SUCCESS;
    }


    *uc = (unicode_t)' ';
    return STREAM_SUCCESS;
}
