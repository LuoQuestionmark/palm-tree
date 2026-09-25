#pragma once

#include "utils/dict.h"
#include "words.h"

// assign pos tag to each word with only one available postag
void postag_tag_dict_unique(words_t *words, cat_dict_t *cat_dict);

void postag_tag_num(words_t *words);
void postag_tag_latin(words_t *words);

// assign pos tag to chinese auxilary verbs, according to the guideline on
// https://universaldependencies.org/zh/index.html
void postag_tag_cn_aux(words_t *words);
