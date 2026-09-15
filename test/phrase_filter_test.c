#include "phrase.h"
#include "phrase_filter.h"
#include "utils/dict.h"
#include "words.h"
#include <stdlib.h>

int main() {
    phrase_t *p = phrase_init("亚马逊苹果");
    segmentation_t seg;
    dict_t *dict = dict_init();

    if (!dict_load_file(dict, "resource/cedict_ts.u8")) {
        exit(EXIT_FAILURE);
    }

    phrase_filter_dict_words(p, &p->cn_char_seg, &seg, dict,
                             PHRASE_DICT_FILTER_LONGEST);

    words_t *words = phrase_segment(p, &seg);
    words_fprint(stdout, words);

    words_free(words);
    dict_free(dict);
    phrase_free(p);
}
