#pragma once

#include "utils/bloom_filter.h"
#include <stdbool.h>

#define MAX_DICT_BUFFER_SIZE (64 * 1024 * 1024)

typedef struct dict dict_t;
struct dict {
    bool loaded;
    bloom_filter_t *bloom_filter;
    char buffer[MAX_DICT_BUFFER_SIZE];
};

dict_t *dict_init();
void dict_free(dict_t *dict);

bool dict_load_file(dict_t *dict, const char *filename);
bool dict_exist(const dict_t *dict, const char *word);
