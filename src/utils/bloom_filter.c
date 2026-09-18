#include "utils/bloom_filter.h"
#include "utils/utf8_utils.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lookup3.h"

bloom_filter_t *bloom_init() {
    bloom_filter_t *bloom = calloc(1, sizeof(bloom_filter_t));
    return bloom;
}

void bloom_free(bloom_filter_t *bloom) {
    if (bloom == NULL) return;
    free(bloom);
}

void bloom_add(bloom_filter_t *bloom, const char *utf8_word) {
    assert(bloom && utf8_word);
    char current[4]  = { 0 };
    char word[4 * 8] = { 0 };

    for (const char *next = utf8_word; next != NULL;
         next             = utf8_char_consume(next, current)) {
        strncat(word, current, sizeof(word) - strlen(word) - 1);
    }

    uint32_t index1, index2;
    hashword2((uint32_t *)word, strlen(word) / sizeof(uint32_t), &index1,
              &index2);

    index1 = (index1 % (8 * sizeof(bloom->filter_bits)));
    index2 = (index2 % (8 * sizeof(bloom->filter_bits)));

    bloom->filter_bits |= (0b1 << index1);
    bloom->filter_bits |= (0b1 << index2);
}

bool bloom_exist(bloom_filter_t *bloom, const char *utf8_word) {
    assert(bloom && utf8_word);
    char current[4]  = { 0 };
    char word[4 * 8] = { 0 };

    for (const char *next = utf8_word; next != NULL;
         next             = utf8_char_consume(next, current)) {
        strncat(word, current, sizeof(word) - strlen(word) - 1);
    }

    uint32_t index1, index2;
    hashword2((uint32_t *)word, strlen(word) / sizeof(uint32_t), &index1,
              &index2);

    index1 = (index1 % (8 * sizeof(bloom->filter_bits)));
    index2 = (index2 % (8 * sizeof(bloom->filter_bits)));

    if ((bloom->filter_bits & (0b1 << index1)) == 0) return false;
    if ((bloom->filter_bits & (0b1 << index2)) == 0) return false;

    return true;
}
