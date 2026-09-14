#pragma once

#include "phrase.h"

void phrase_filter_dict_words(phrase_t *phrase, const segmentation_t *seg_in,
                              segmentation_t *seg_out);
void phrase_filter_ordinal_number(phrase_t *phrase,
                                  const segmentation_t *seg_in,
                                  segmentation_t *seg_out);
void phrase_filter_cardinal_number(phrase_t *phrase,
                                   const segmentation_t *seg_in,
                                   segmentation_t *seg_out);
