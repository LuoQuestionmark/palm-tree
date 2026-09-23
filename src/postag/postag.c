#include "postag/postag.h"
#include "utils/dict.h"
#include "words.h"
#include <assert.h>

void postag_tag_dict_unique(words_t *words, cat_dict_t *cat_dict) {
    assert(words);
    assert(cat_dict);

    for (int i = 0; i < words->count; i++) {
        const char *word = words->word_strings[i];
        if (word == NULL) continue;

        enum POS_TAG postag = POS_TAG_NULL;
        if (cat_dict_lookup_unique(cat_dict, word, &postag)) {
            words->pos_tags[i] = postag;
        }
    }
}
