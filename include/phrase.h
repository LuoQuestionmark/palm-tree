#pragma once
#include "utils/dict.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_SEGMENTATION_SEG 8
#define MAX_SEGMENTATION_BYTE (MAX_SEGMENTATION_SEG * 64)
#define SEG_0 (uint64_t)(0b1ll << 63)

#define PHRASE_BUFFER_SIZE (1024 * 4)

struct segmentation {
    /* use 8 * 64 bits integer to save the segmentation; the n-th bit (counting
     * from the first one) is set to 1 if there is a segmentation at byte n.
     * with the length of 64 bytes (8*8), the segmentation struct can provide
     * segmentation info of a string of length less or equal to 256 bytes. for a
     * typical UTF8 Chinese string, each Chinese character has a length of 4
     * bytes, therefore one segmentation structure support at most 64 Chinese
     * character. */
    uint64_t seg[MAX_SEGMENTATION_SEG];
};

typedef struct segmentation segmentation_t;

struct phrase {
    size_t length;
    char *phrase;
    segmentation_t cn_char_seg;
    segmentation_t word_seg;
};
typedef struct phrase phrase_t;

struct phrase_list {
    int count;
    int size;
    phrase_t **phrases;
};
typedef struct phrase_list phrase_list_t;

enum phrase_snprint_type {
    PHRASE_SNPRINT_ORIGINAL    = 0,
    PHRASE_SNPRINT_PER_CN_CHAR = 1,
    PHRASE_SNPRINT_PER_CN_WORD = 2,
};

void segmentation_init(segmentation_t *segmentation);
void segmentation_add(segmentation_t *segmentation, const int offset);
void segmentation_del(segmentation_t *segmentation, const int offset);
void segmentation_pop(segmentation_t *segmentation, const int offset);
void segmentation_pop_n(segmentation_t *segmentation, const int offset, int n);

int segmentation_count(const segmentation_t *segmentation);
bool segmentation_at(const segmentation_t *segmentation, const int offset,
                     int *len);
bool segmentation_get(const segmentation_t *segmentation, const int index,
                      int *offset, int *len);

phrase_t *phrase_init(const char *phrase_string);
void phrase_free(phrase_t *phrase);
void phrase_snprint(const phrase_t *phrase, enum phrase_snprint_type print_type,
                    char *dst, size_t max_len);

// generate utf8 char segmentation (saved in `phrase_t->cn_char_seg`), return
// True if such segmentation is possible
bool phrase_utf8_char_segmentation(phrase_t *phrase);

// generate `seg_c` count (of less) of best segmentation, save value to `seg_v`,
// save the best segmentation to `phrase_t` (`phrase_t->word_seg`)
void phrase_base_word_segmentation(phrase_t *phrase, dict_t *dict, int seg_c,
                                   segmentation_t *seg_v[seg_c]);

phrase_list_t *phrase_list_init();
void phrase_list_free(phrase_list_t *ph_list);
void phrase_list_append(phrase_list_t *ph_list, phrase_t *phrase);

phrase_list_t *parse_paragraph(const char *phrase_strings);
