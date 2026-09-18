#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct bloom_filter bloom_filter_t;
struct bloom_filter {
    uint32_t filter_bits;
};

bloom_filter_t *bloom_init();
void bloom_free(bloom_filter_t *bloom);
void bloom_add(bloom_filter_t *bloom, const char *utf8_word);
bool bloom_exist(bloom_filter_t *bloom, const char *utf8_word);
