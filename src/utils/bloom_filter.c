#include "utils/bloom_filter.h"
#include "lookup3.h"
#include "utils/utf8_utils.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bloom_filter_t *bloom_init() {
    bloom_filter_t *bloom = calloc(1, sizeof(bloom_filter_t));
    if (bloom == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    bloom->filter_bits = calloc(BLOOM_FILTER_BYTES, sizeof(char));
    if (bloom->filter_bits == NULL) {
        perror("calloc");
        fputs("size of bloom filter over the calloc limit", stderr);
        exit(EXIT_FAILURE);
    }

    return bloom;
}

void bloom_free(bloom_filter_t *bloom) {
    if (bloom == NULL) return;
    if (bloom->filter_bits) free(bloom->filter_bits);
    free(bloom);
}

void bloom_add(bloom_filter_t *bloom, const char *utf8_word) {
    assert(bloom && utf8_word);

    uint32_t index1 = BLOOM_HASH_SEED1;
    uint32_t index2 = BLOOM_HASH_SEED2;

    hashword2((uint32_t *)utf8_word, strlen(utf8_word) / sizeof(uint32_t),
              &index1, &index2);

    index1 = (index1 % BLOOM_FILTER_BITS);
    index2 = (index2 % BLOOM_FILTER_BITS);

    uint32_t byte_offset1      = index1 >> 3;
    uint32_t byte_offset2      = index2 >> 3;
    uint32_t extra_bit_offset1 = index1 % 8;
    uint32_t extra_bit_offset2 = index2 % 8;

    bloom->filter_bits[byte_offset1] |= (0b1 << extra_bit_offset1);
    bloom->filter_bits[byte_offset2] |= (0b1 << extra_bit_offset2);
}

bool bloom_exist(bloom_filter_t *bloom, const char *utf8_word) {
    assert(bloom && utf8_word);

    uint32_t index1 = BLOOM_HASH_SEED1;
    uint32_t index2 = BLOOM_HASH_SEED2;

    hashword2((uint32_t *)utf8_word, strlen(utf8_word) / sizeof(uint32_t),
              &index1, &index2);

    index1 = (index1 % BLOOM_FILTER_BITS);
    index2 = (index1 % BLOOM_FILTER_BITS);

    uint32_t byte_offset1      = index1 >> 3;
    uint32_t byte_offset2      = index2 >> 3;
    uint32_t extra_bit_offset1 = index1 % 8;
    uint32_t extra_bit_offset2 = index2 % 8;

    if ((bloom->filter_bits[byte_offset1] & (0b1 << extra_bit_offset1)) == 0) {
        return false;
    }
    if ((bloom->filter_bits[byte_offset2] & (0b1 << extra_bit_offset2)) == 0) {
        return false;
    }

    return true;
}
