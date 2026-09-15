#include "phrase_filter.h"
#include "phrase.h"
#include "utils/dict.h"
#include "utils/utf8_utils.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void phrase_filter_dict_longest(phrase_t *phrase, dict_t *dict,
                                       segmentation_t *seg) {
    assert(phrase && seg);

    char buffer[MAX_CN_WORD_LEN * 4 + 1] = { 0 };

    for (int i = 0; i < segmentation_count(seg); i++) {
        int offset, len;
        if (!segmentation_get(seg, i, &offset, &len)) {
            // unexpected branch, reached only when `seg_count` or `seg_get`
            // does not word correctly
            exit(EXIT_FAILURE);
        }

        for (int j = MAX_CN_WORD_LEN; j >= MIN_CN_WORD_LEN; j--) {
            memset(buffer, 0, sizeof(buffer));
            if (!utf8_char_load_n(phrase->phrase + offset, buffer,
                                  sizeof(buffer), j)) {
                // unexpected branch, only if the phrase is not a valid utf8
                // string or program does not work as expected
                exit(EXIT_FAILURE);
            }

            if (!dict_exist(dict, buffer)) continue;

            // if a word consisting j utf8 detected at given offset, then set
            // the next (j - 1) segmentation point to zero

            segmentation_pop_n(seg, offset + 1, (j - 1));

            break;
        }
    }
}

void phrase_filter_dict_words(phrase_t *phrase, const segmentation_t *seg_in,
                              segmentation_t *seg_out, dict_t *dict,
                              const enum PHRASE_DICT_FILTER_STRATEGY strategy) {
    assert(phrase);
    assert(dict);

    *seg_out = *seg_in;

    // if (dict == NULL) {
    //     dict = dict_init();
    //     if (!dict_load_file(dict, "resource/cedict_ts.u8")) {
    //         exit(EXIT_FAILURE);
    //     }
    // }

    switch (strategy) {
    case PHRASE_DICT_FILTER_LONGEST:
        phrase_filter_dict_longest(phrase, dict, seg_out);
    default:
        break;
    }
}

void phrase_filter_ordinal_number(phrase_t *phrase,
                                  const segmentation_t *seg_in,
                                  segmentation_t *seg_out) {
    // TODO
    *seg_out = *seg_in;
}

void phrase_filter_cardinal_number(phrase_t *phrase,
                                   const segmentation_t *seg_in,
                                   segmentation_t *seg_out) {
    // TODO
    *seg_out = *seg_in;
}
