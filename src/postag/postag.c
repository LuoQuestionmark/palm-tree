#include "postag/postag.h"
#include "patterns/cn_aux.h"
#include "patterns/digits.h"
#include "patterns/latin.h"
#include "utils/dict.h"
#include "words.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void postag_tag_dict_unique(words_t *words, cat_dict_t *cat_dict) {
    assert(words);
    assert(cat_dict);

    for (int i = 0; i < words->count; i++) {
        if (words->pos_tags[i] != POS_TAG_NULL) continue;

        const char *word = words->word_strings[i];
        if (word == NULL) continue;

        enum POS_TAG postag = POS_TAG_NULL;
        if (cat_dict_lookup_unique(cat_dict, word, &postag)) {
            words->pos_tags[i] = postag;
        }
    }
}

void postag_tag_num(words_t *words) {
    assert(words);
    for (int i = 0; i < words->count; i++) {
        if (words->pos_tags[i] != POS_TAG_NULL) continue;

        const char *word = words->word_strings[i];
        if (word == NULL) continue;

        if (is_ascii_digits(word) == (int)strlen(word) ||
            is_cn_char_digits(word) == (int)strlen(word) ||
            is_cn_fw_digits(word) == (int)strlen(word) ||
            is_ordinal_cn_char_digits(word) == (int)strlen(word)) {

            words->pos_tags[i] = POS_TAG_NUM;
        }
    }
}

void postag_tag_latin(words_t *words) {
    assert(words);
    for (int i = 0; i < words->count; i++) {
        if (words->pos_tags[i] != POS_TAG_NULL) continue;

        const char *word = words->word_strings[i];
        if (word == NULL) continue;

        if (is_latin(word) == (int)(strlen(word))) {
            words->pos_tags[i] = POS_TAG_X;
        }
    }
}

void postag_tag_cn_aux(words_t *words) {
    assert(words);

    for (int i = 0; i < words->count; i++) {
        // skip is already decided
        if (words->pos_tags[i] != POS_TAG_NULL) continue;

        const char *word = words->word_strings[i];

        // check if word is not valid, trigger only when the program does not
        // work correctly
        assert(word != NULL);

        enum POS_TAG previous_pos =
            i > 0 ? words->pos_tags[i - 1] : POS_TAG_NULL;

        if (is_chinese_auxiliary(word, previous_pos) > 0) {
            words->pos_tags[i] = POS_TAG_AUX;
        }
    }
}
