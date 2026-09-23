#pragma once

#include "utils/dict.h"
#include "words.h"

// assign pos tag to each word with unique possibility
void postag_tag_dict_unique(words_t *words, cat_dict_t *cat_dict);
