#pragma once

#include "phrase.h"
#include <stdio.h>

// The fact that UD only has 17 word categories is nice, by assign 1 bit to each
// category, it is possible to return the combination of multiple categories
// into a single return value
enum POS_TAG {
    POS_TAG_NULL  = 0,
    POS_TAG_ADJ   = 1,
    POS_TAG_ADV   = 1 << 1,
    POS_TAG_INTJ  = 1 << 2,
    POS_TAG_NOUN  = 1 << 3,
    POS_TAG_PROPN = 1 << 4,
    POS_TAG_VERB  = 1 << 5,
    POS_TAG_ADP   = 1 << 6,
    POS_TAG_AUX   = 1 << 7,
    POS_TAG_CCONJ = 1 << 8,
    POS_TAG_DET   = 1 << 9,
    POS_TAG_NUM   = 1 << 10,
    POS_TAG_PART  = 1 << 11,
    POS_TAG_PRON  = 1 << 12,
    POS_TAG_SCONJ = 1 << 13,
    POS_TAG_PUNCT = 1 << 14,
    POS_TAG_SYM   = 1 << 15,
    POS_TAG_X     = 1 << 16,
};

typedef struct words words_t;
struct words {
    int count;
    char **word_strings;
    enum POS_TAG *pos_tags;

    // segmentation info is copied, this make the training process more
    // accessible: as the evaluation is based on `words` structure, whereas the
    // segmentation object is more trivial for training
    segmentation_t segmentation;
};

const char *POS_TAG_str(enum POS_TAG pt);
enum POS_TAG POS_TAG_parse(const char *str);

words_t *phrase_segment(phrase_t *phrase, segmentation_t *segmentation);
void words_free(words_t *words);

void words_tagging(words_t *words); // assign tags to each separated words
int words_eval(words_t *words);     // evaluate current words separation

// helper function, print to help dev
void words_fprint(FILE *restrict stream, words_t *words);
void words_cprint(words_t *words);
void words_snprint(char str[restrict], size_t size, words_t *words);
