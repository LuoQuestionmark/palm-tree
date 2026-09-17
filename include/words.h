#pragma once

#include "phrase.h"
#include <stdio.h>

enum POS_TAG {
    POS_TAG_UNDEFINED = 0,
    POS_TAG_AD, // adverb
    POS_TAG_CD, // cardinal number
    POS_TAG_NN, // common noun
    POS_TAG_VV  // other verb
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

words_t *phrase_segment(phrase_t *phrase, segmentation_t *segmentation);
void words_free(words_t *words);

void words_tagging(words_t *words); // assign tags to each separated words
int words_eval(words_t *words);     // evaluate current words separation

// helper function, print to help dev
void words_fprint(FILE *restrict stream, words_t *words);
void words_snprint(char str[restrict], size_t size, words_t *words);
