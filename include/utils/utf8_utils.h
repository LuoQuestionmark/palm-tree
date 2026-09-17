#pragma once

#include <stdbool.h>
#include <stddef.h>

int utf8_char_len(const char *utf8_char);
bool utf8_char_load_n(const char *src, char *dst, size_t dst_size, int len);
const char *utf8_char_consume(const char *src, char *utf_char_buf);
