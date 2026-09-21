#include "patterns/latin.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

int is_latin(const char *utf8_string) {
    assert(utf8_string);
    if (!isalpha(utf8_string[0])) return -1;

    int len = 0;
    for (size_t offset = 0; offset < strlen(utf8_string); offset++) {
        if (isalpha(utf8_string[offset]) == 0) break;
        len += 1;
    }

    return len;
}
