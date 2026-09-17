#include "phrase.h"
#include "phrase_filter.h"
#include "utils/dict.h"
#include "words.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test_one() {
    phrase_t *p = phrase_init("亚马逊苹果");
    segmentation_t seg;
    dict_t *dict = dict_init();

    if (!dict_load_file(dict, "resource/cedict_ts.u8")) {
        exit(EXIT_FAILURE);
    }

    phrase_filter_dict_words(p, &p->cn_char_seg, &seg, dict,
                             PHRASE_DICT_FILTER_LONGEST);

    words_t *words = phrase_segment(p, &seg);
    assert(words->count == 2);

    words_free(words);
    dict_free(dict);
    phrase_free(p);
}

void test_two() {
    phrase_t *p = phrase_init("一百三十二人");
    segmentation_t seg;

    phrase_filter_cardinal_number(p, &p->cn_char_seg, &seg);

    words_t *words = phrase_segment(p, &seg);

    char buffer[1024] = { 0 };
    words_snprint(buffer, sizeof(buffer), words);

    assert(strncmp(buffer, "一百三十二<UNDEFINED> 人<UNDEFINED>",
                   sizeof(buffer)) == 0);
    assert(words->count == 2);

    words_free(words);
    phrase_free(p);
}

void test_three() {
    phrase_t *p = phrase_init("第九十九苹果");
    segmentation_t seg;

    phrase_filter_ordinal_number(p, &p->cn_char_seg, &seg);

    words_t *words = phrase_segment(p, &seg);

    char buffer[1024] = { 0 };
    words_snprint(buffer, sizeof(buffer), words);

    assert(strncmp(buffer, "第九十九<UNDEFINED> 苹<UNDEFINED> 果<UNDEFINED>",
                   sizeof(buffer)) == 0);
    assert(words->count == 3);

    words_free(words);
    phrase_free(p);
}

int main() {
    test_one();
    test_two();
    test_three();
}
