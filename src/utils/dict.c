#include "utils/dict.h"
#include "utils/bloom_filter.h"
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void regex_sanitize(const char *src, char *dst, size_t len) {
    assert(src);
    assert(dst && len > 0);

    static const char special_chars[] = { '#', '.', '[', ']', '{', '}',
                                          '^', '?', '+', '|', '*' };

    memset(dst, 0, len);
    for (size_t i = 0, j = 0; i < strlen(src) && j < len; i++, j++) {
        for (size_t k = 0; k < sizeof(special_chars); k++) {
            if (src[i] == special_chars[k]) {
                dst[j++] = '\\';
                break;
            }
        }
        dst[j] = src[i];
    }
}

dict_t *dict_init() {
    dict_t *dict       = calloc(1, sizeof(dict_t));
    dict->bloom_filter = bloom_init();

    return dict;
}

void dict_free(dict_t *dict) {
    if (dict == NULL) return;
    if (dict->bloom_filter) {
        bloom_free(dict->bloom_filter);
    }

    free(dict);
}

bool dict_load_file(dict_t *dict, const char *filename) {
    assert(dict);
    assert(filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return false;
    }

    size_t chunk_size =
        fread(dict->buffer, sizeof(char), MAX_DICT_BUFFER_SIZE - 1, file);

    if (ferror(file)) {
        perror("fread");
        return false;
    }

    dict->buffer[chunk_size] = '\0';
    dict->loaded             = true;

    char line_buffer[1024];
    rewind(file);
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        line_buffer[strlen(line_buffer) - 1] = '\0';
        bloom_add(dict->bloom_filter, line_buffer);
    }

    fclose(file);
    return true;
}

bool dict_exist(const dict_t *dict, const char *word) {
    assert(dict != NULL);
    assert(dict->loaded);

    if (word == NULL || strlen(word) == 0) return false;

    if (!bloom_exist(dict->bloom_filter, word)) {
        return false;
    }

    // comment out the old solution with regex matching, which is slow!
    bool ret = true;

    char word_sanitized[1024];
    regex_sanitize(word, word_sanitized, sizeof(word_sanitized));

    char regex_pattern[1024] = { 0 };
    snprintf(regex_pattern, sizeof(regex_pattern), "^%s\n", word_sanitized);

    regex_t regex;

    if (regcomp(&regex, regex_pattern, REG_NOSUB | REG_NEWLINE) != 0) {
        perror("regcomp");
        ret = false;
        goto reg_clean;
    };

    if (regexec(&regex, dict->buffer, 0, NULL, 0) != 0) {
        // no match
        ret = false;
        goto reg_clean;
    }

reg_clean:
    regfree(&regex);
    return ret;
}
