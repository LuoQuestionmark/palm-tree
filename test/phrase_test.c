// ATTENTION: This file should be regarded as encoding with UTF-8 !

#include "phrase.h"
#include <assert.h>
#include <string.h>

int main() {
    phrase_t *p1        = phrase_init("测试句子");
    segmentation_t *seg = &p1->cn_char_seg;

    segmentation_init(seg);
    segmentation_add(seg, 3);
    segmentation_add(seg, 6);
    segmentation_add(seg, 9);
    // segmentation_add(seg, 12);

    seg = &p1->word_seg;
    segmentation_init(seg);
    segmentation_add(seg, 6);

    char buffer[1024] = { 0 };

    phrase_snprint(p1, PHRASE_SNPRINT_ORIGINAL, buffer, sizeof(buffer) - 1);
    assert(strcmp(buffer, "测试句子") == 0);

    phrase_snprint(p1, PHRASE_SNPRINT_PER_CN_CHAR, buffer, sizeof(buffer) - 1);
    assert(strcmp(buffer, "测试句子") != 0);
    assert(strcmp(buffer, "测 试 句 子") == 0);

    phrase_snprint(p1, PHRASE_SNPRINT_PER_CN_WORD, buffer, sizeof(buffer) - 1);
    assert(strcmp(buffer, "测试 句子") == 0);

    phrase_free(p1);
}
