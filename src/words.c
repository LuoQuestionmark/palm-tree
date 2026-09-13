#include "words.h"
#include "phrase.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

words_t *phrase_segment(phrase_t *phrase, segmentation_t *segmentation) {
    assert(phrase && segmentation);

    words_t *words      = calloc(1, sizeof(words_t));
    int words_count     = segmentation_count(segmentation);
    words->count        = words_count;
    words->word_strings = calloc(words_count, sizeof(char *));

    int offset, len;
    char buffer[1024];
    for (int i = 0; i < words_count; i++) {
        if (segmentation_get(segmentation, i, &offset, &len)) {
            memset(buffer, 0, sizeof(buffer));
            strncpy(buffer, phrase->phrase + offset, len);
            words->word_strings[i] = strdup(buffer);
        }
    }

    return words;
}

void words_free(words_t *words) {
    if (words == NULL) return;
    assert(words->word_strings); // problematic initialization

    for (int i = 0; i < words->count; i++) {
        free(words->word_strings[i]);
    }
    free(words->word_strings);
    free(words);
}
