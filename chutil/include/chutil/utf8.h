
#ifndef CHUTIL_UTF8_H
#define CHUTIL_UTF8_H

#include <stdint.h>
#include <chutil/stream.h>

// A unicode code point will be 16 bits.
// 4 bits for each hex digit in U+XXXX
typedef uint16_t unicode_t;

// cstr should have at LEAST 4 characters.
// Each of which will be 0-9, a-f, or A-F
unicode_t unicode_from_cstr(const char *cstr);

// Given a unicode type, this will write the corresponding 
// UTF-8 characters to the given output stream.
// In case of error parsing, a space is output.
// The stream state returned is only an error if there is an error
// reading/writing to the given streams. If the stream state returned
// is an error, disregard the unicode returned.
stream_state_t unicode_to_utf8(out_stream_t *os, unicode_t uc);
stream_state_t unicode_from_utf8(in_stream_t *is, unicode_t *uc);

#endif
