#pragma once

#include "phrase.h"

#define MIN_CN_WORD_LEN 2 // minimum chinese word length by character
#define MAX_CN_WORD_LEN 8 // maximum chinese word length by character

enum PHRASE_DICT_FILTER_STRATEGY {
    PHRASE_DICT_FILTER_UNDEFINED,
    PHRASE_DICT_FILTER_LONGEST,
};

void phrase_filter_dict_words(phrase_t *phrase, const segmentation_t *seg_in,
                              segmentation_t *seg_out,
                              const enum PHRASE_DICT_FILTER_STRATEGY);

void phrase_filter_ordinal_number(phrase_t *phrase,
                                  const segmentation_t *seg_in,
                                  segmentation_t *seg_out);
void phrase_filter_cardinal_number(phrase_t *phrase,
                                   const segmentation_t *seg_in,
                                   segmentation_t *seg_out);
