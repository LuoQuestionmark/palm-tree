#include "utils/dict.h"
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dict_t *dict_init() {
    return calloc(1, sizeof(dict_t));
}

void dict_free(dict_t *dict) {
    free(dict);
}

bool dict_load_file(dict_t *dict, const char *filename) {
    assert(dict);
    assert(filename);

    FILE *file = fopen(filename, "r");
    size_t chunk_size =
        fread(dict->buffer, sizeof(char), MAX_DICT_BUFFER_SIZE - 1, file);

    if (ferror(file)) {
        perror("fread");
        return false;
    }

    dict->buffer[chunk_size] = '\0';
    fclose(file);
    return true;
}

bool dict_exist(const dict_t *dict, const char *word) {
    if (dict == NULL) return false;
    if (word == NULL || strlen(word) == 0) return false;

    bool ret = true;

    char regex_pattern[128] = { 0 };
    snprintf(regex_pattern, sizeof(regex_pattern), "^.*%s.*$", word);

    regex_t regex;

    if (regcomp(&regex, regex_pattern, REG_NOSUB) != 0) {
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
