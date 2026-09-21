#pragma once

// return number of chars, not actual utf8 chars
// (as each utf8 is composed by one or more char)
int is_ascii_digits(const char *utf8_string);
int is_cn_fw_digits(const char *utf8_string);
int is_cn_char_digits(const char *utf8_string);

int is_ordinal_cn_char_digits(const char *utf8_string);
