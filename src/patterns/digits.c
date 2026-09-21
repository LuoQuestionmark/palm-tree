#include "patterns/digits.h"
#include "utils/utf8_utils.h"
#include <stdbool.h>
#include <string.h>

static bool is_cn_fw_digit(const char *utf8_char) {
    if (utf8_char == NULL || utf8_char[0] == '0') return false;

    static const char numbers[10][4] = { "０", "１", "２", "３", "４",
                                         "５", "６", "７", "８", "９" };
    for (int i = 0; i < 10; i++) {
        if (strncmp(utf8_char, numbers[i], sizeof(numbers[i])) == 0)
            return true;
    }

    return false;
}

static bool is_cn_char_digit(const char *utf8_char) {
    if (utf8_char == NULL || utf8_char[0] == '0') return false;

    static const char numbers[11][4] = { "〇", "一", "二", "三", "四", "五",
                                         "六", "七", "八", "九", "零" };
    for (int i = 0; i < 11; i++) {
        if (strncmp(utf8_char, numbers[i], sizeof(numbers[i])) == 0)
            return true;
    }

    return false;
}

int is_ascii_digits(const char *utf8_string) {
    if (utf8_string == NULL) return -1;

    bool has_decimal_separator = false;

    int len = 0;
    for (int offset = 0; true; offset++) {
        char current = utf8_string[offset];
        if (current >= '0' && current <= '9') {
            len += utf8_char_len(utf8_string + offset);
            continue;
        }
        if (current == '.' && offset != 0 && !has_decimal_separator) {
            // TODO: fix when "." is the last char, e.g. "1."
            has_decimal_separator = true;
            len += utf8_char_len(utf8_string + offset);
            continue;
        }
        break;
    }
    return len;
}

int is_cn_fw_digits(const char *utf8_string) {
    if (utf8_string == NULL) return -1;

    int len = 0;
    char current[4];

    for (const char *head = utf8_string;;) {
        head = utf8_char_consume(head, current);
        if (head == NULL) break;
        if (!is_cn_fw_digit(current)) break;
        len += utf8_char_len(current);
    }
    return len;
}

int is_cn_char_digits(const char *utf8_string) {
    if (utf8_string == NULL) return -1;

    int len = 0;
    char current[4];

    // 亿 万 千 百 十
    bool special_allow[5] = { true, true, true, true, true };

    for (const char *head = utf8_string;;) {
        head = utf8_char_consume(head, current);
        if (head == NULL) break;

        if (is_cn_char_digit(current)) {
            len += utf8_char_len(current);
            continue;
        }

        if (special_allow[0] && strncmp(current, "亿", sizeof("亿")) == 0) {
            special_allow[0] = false;
            memset(special_allow + 1, true, 4 * sizeof(bool));
            len += utf8_char_len(current);
            continue;
        }

        if (special_allow[1] && strncmp(current, "万", sizeof("万")) == 0) {
            special_allow[1] = false;
            memset(special_allow + 2, true, 3 * sizeof(bool));
            len += utf8_char_len(current);
            continue;
        }

        if (special_allow[2] && strncmp(current, "千", sizeof("千")) == 0) {
            special_allow[2] = false;
            len += utf8_char_len(current);
            continue;
        }

        if (special_allow[3] && strncmp(current, "百", sizeof("百")) == 0) {
            special_allow[3] = false;
            len += utf8_char_len(current);
            continue;
        }

        if (special_allow[4] && strncmp(current, "十", sizeof("十")) == 0) {
            special_allow[4] = false;
            len += utf8_char_len(current);
            continue;
        }

        break;
    }
    return len;
}

int is_ordinal_cn_char_digits(const char *utf8_string) {
    char buffer[4];
    const char *next = utf8_char_consume(utf8_string, buffer);
    if (strncmp(buffer, "第", sizeof("第")) != 0) {
        return -1;
    }

    int len = is_cn_char_digits(next);
    if (len > 0) {
        // minus one since static char end with '\0', which takes one extra byte
        return len + sizeof("第") - 1;
    }

    return -1;
}
