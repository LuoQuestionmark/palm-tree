#pragma once

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

enum UTF8_IGNORE_OPTION {
    UTF8_IGNORE_EMPTY        = 0b0,
    UTF8_IGNORE_SPACE        = 0b1,
    UTF8_IGNORE_CONTROL_CHAR = 0b10,
    UTF8_IGNORE_SYMBOLS      = 0b100,
    UTF8_IGNORE_ALL          = UINT_MAX
};

int utf8_char_len(const char *utf8_char);
bool utf8_char_load_n(const char *src, char *dst, size_t dst_size, int len);
const char *utf8_char_consume(const char *src, char *utf_char_buf);
bool utf8_should_ignore_char(const char *src,
                             enum UTF8_IGNORE_OPTION ignore_option);
