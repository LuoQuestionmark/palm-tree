#include "utils/utf8_utils.h"
#include <assert.h>

int utf8_char_len(const char *utf8_char) {
    assert(utf8_char);

    if (utf8_char[0] == '\0') return 0;

    static const char utf8_len1_mask  = 0b10000000;
    static const char utf8_len1_value = 0b00000000;

    static const char utf8_len2_mask  = 0b11100000;
    static const char utf8_len2_value = 0b11000000;

    static const char utf8_len3_mask  = 0b11110000;
    static const char utf8_len3_value = 0b11100000;

    static const char utf8_len4_mask  = 0b11111000;
    static const char utf8_len4_value = 0b11110000;

    static const char utf8_suite_mask  = 0b11000000;
    static const char utf8_suite_value = 0b10000000;

    if ((utf8_char[0] & utf8_len1_mask) == utf8_len1_value) {
        return 1;
    }

    if ((utf8_char[0] & utf8_len2_mask) == utf8_len2_value) {
        if ((utf8_char[1] & utf8_suite_mask) != utf8_suite_value) return -1;
        return 2;
    }

    if ((utf8_char[0] & utf8_len3_mask) == utf8_len3_value) {
        if ((utf8_char[1] & utf8_suite_mask) != utf8_suite_value) return -1;
        if ((utf8_char[2] & utf8_suite_mask) != utf8_suite_value) return -1;
        return 3;
    }

    if ((utf8_char[0] & utf8_len4_mask) == utf8_len4_value) {
        if ((utf8_char[1] & utf8_suite_mask) != utf8_suite_value) return -1;
        if ((utf8_char[2] & utf8_suite_mask) != utf8_suite_value) return -1;
        if ((utf8_char[3] & utf8_suite_mask) != utf8_suite_value) return -1;
        return 4;
    }

    return -1;
}
