#pragma once

#include "phrase.h"

typedef struct words words_t;
struct words {
    int count;
    char **word_strings;
};

words_t *phrase_segment(phrase_t *phrase, segmentation_t *segmentation);
void words_free(words_t *words);
