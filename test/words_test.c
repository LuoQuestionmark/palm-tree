// ATTENTION: This file should be regarded as encoding with UTF-8 !

#include "phrase.h"
#include "words.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {
    phrase_t *p1        = phrase_init("测试句子");
    segmentation_t *seg = &p1->word_seg;
    segmentation_init(seg);
    segmentation_add(seg, 6);

    words_t *words = phrase_segment(p1, &p1->word_seg);
    assert(words->count == 2);
    assert(strcmp(words->word_strings[0], "测试") == 0);
    assert(strcmp(words->word_strings[1], "句子") == 0);

    words->pos_tags[0] = POS_TAG_VERB;
    words->pos_tags[1] = POS_TAG_NOUN;

    char buffer[1024] = { 0 };
    FILE *buffer_ptr  = fmemopen(buffer, sizeof(buffer), "w");
    words_fprint(buffer_ptr, words);
    assert(strcmp(buffer, "测试<VV> 句子<NN>") == 0);

    words_free(words);
    phrase_free(p1);
}
