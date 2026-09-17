#include "utils/utf8_utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int utf8_char_len(const char *utf8_char) {
    assert(utf8_char);

    if (utf8_char[0] == '\0') return 0;

    static const char utf8_len1_mask  = 0b10000000;
    static const char utf8_len1_value = 0b00000000;

    static const char utf8_len2_mask  = 0b11100000;
    static const char utf8_len2_value = 0b11000000;

    static const char utf8_len3_mask  = 0b11110000;
    static const char utf8_len3_value = 0b11100000;

    static const char utf8_len4_mask   = 0b11111000;
    static const char utf8_len4_value  = 0b11110000;
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

bool utf8_char_load_n(const char *src, char *dst, size_t dst_size, int len) {
    assert(src && dst);
    assert(dst_size > 0 && len > 0);

    size_t tot_len = 0;
    int offset     = 0;

    for (int i = 0; i < len; i++) {
        offset = utf8_char_len(src);
        if (offset <= 0) return false;

        tot_len += offset;
    }

    if (tot_len > dst_size) {
        fprintf(stderr,
                "cannot copy full text from src to dst: dst has a size of %ld, "
                "while u8 char of length %d requires %ld bytes",
                dst_size, len, tot_len);
        return false;
    }

    strncpy(dst, src, tot_len);
    return true;
}

const char *utf8_char_consume(const char *src, char *utf8_char_buf) {
    if (src == NULL || src[0] == '\0') return NULL;

    memset(utf8_char_buf, 0, 4);

    int len = utf8_char_len(src);
    if (len <= 0) {
        return NULL;
    }

    if (utf8_char_buf) {
        strncpy(utf8_char_buf, src, len);
    }

    return src + len;
}

bool utf8_should_ignore_char(const char *src,
                             enum UTF8_IGNORE_OPTION ignore_option) {

    if (src == NULL || src[0] == '\0') return false;
    if (ignore_option & UTF8_IGNORE_SPACE) {
        if (isblank(src[0])) return true;
    }
    if (ignore_option & UTF8_IGNORE_CONTROL_CHAR) {
        if (iscntrl(src[0])) return true;
    }
    if (ignore_option & UTF8_IGNORE_SYMBOLS) {
        if (src[0] == '=') return true;
        if (src[0] == '/') return true;
        if (src[0] == '\\') return true;
        if (src[0] == '|') return true;
        if (src[0] == '*') return true;
        if (src[0] == '~') return true;
    }

    return false;
}

bool utf8_end_of_cn_phrase(const char *src) {
    char buffer[4] = { 0 };
    utf8_char_consume(src, buffer);
    if (strncmp(buffer, "。", sizeof("。")) == 0) return true;
    if (strncmp(buffer, "；", sizeof("；")) == 0) return true;
    if (strncmp(buffer, "！", sizeof("！")) == 0) return true;
    if (strncmp(buffer, "？", sizeof("？")) == 0) return true;

    return false;
}
