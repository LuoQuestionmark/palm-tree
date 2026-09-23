#pragma once

#include "utils/bloom_filter.h"
#include "words.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_DICT_BUFFER_SIZE (64 * 1024 * 1024) // 64MB

#define HASH_SEED 42
#define WC_HASHTABLE_DEFAULT_WIDTH 64
#define WC_HASHTABLE_MAX_LENGTH 8 // maximum length before rehash

/*
 * Dictionary for (word) existence test,
 * firstly check the bloom filter, if word (may) exists, then check the buffer,
 * which is just the whole file loaded in one chunk
 */
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

/*
 * Dictionary for (word) categories, this time with a hashtable
 */
typedef struct cat_dict cat_dict_t;
typedef struct wc_cell wc_cell_t;
typedef struct wc_hashtable wc_hashtable_t;

struct wc_cell {
    wc_cell_t *next;
    char *word;
    int32_t word_categories;
};

struct wc_hashtable {
    int hashtable_width;
    wc_cell_t **hashtable;
};

struct cat_dict {
    bool loaded;
    bloom_filter_t *bloom_filter;
    wc_hashtable_t *word_categories_table;
};

wc_cell_t *wc_list_init();
void wc_list_free(wc_cell_t *wc_list, bool free_word);
int wc_list_count(wc_cell_t *wc_list);
void wc_list_append(wc_cell_t *wc_list, const char *word, enum POS_TAG postag);
int32_t wc_list_get(wc_cell_t *wc_list, const char *word);

wc_hashtable_t *wc_hashtable_init();
void wc_hashtable_free(wc_hashtable_t *hashtable);
// double the width
wc_hashtable_t *wc_hashtable_resize(wc_hashtable_t *hashtable);
void wc_hashtable_append(wc_hashtable_t *hashtable, const char *word,
                         enum POS_TAG postag);
int32_t wc_hashtable_get(const wc_hashtable_t *hashtable, const char *word);
