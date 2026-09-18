#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BLOOM_FILTER_BITS (1 << 30)
#define BLOOM_FILTER_BYTES (BLOOM_FILTER_BITS >> 3)

#define BLOOM_HASH_SEED1 42
#define BLOOM_HASH_SEED2 451

typedef struct bloom_filter bloom_filter_t;
struct bloom_filter {
    char *filter_bits;
};

bloom_filter_t *bloom_init();
void bloom_free(bloom_filter_t *bloom);
void bloom_add(bloom_filter_t *bloom, const char *utf8_word);
bool bloom_exist(bloom_filter_t *bloom, const char *utf8_word);
