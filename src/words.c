#include "words.h"
#include "phrase.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char POS_TAG_literals[][6] = { "ADJ",  "ADV",  "INTJ", "NOUN",  "PROPN",
                                     "VERB", "ADP",  "AUX",  "CCONJ", "DET",
                                     "NUM",  "PART", "PRON", "SCONJ", "PUNCT",
                                     "SYM",  "X" };

const char *POS_TAG_str(enum POS_TAG pt) {
    if (pt == POS_TAG_NULL) return "";
    for (size_t i = 0;
         i < sizeof(POS_TAG_literals) / sizeof(POS_TAG_literals[0]); i++) {
        if (pt == (1 << i)) return POS_TAG_literals[i];
    }
    return NULL;
}

enum POS_TAG POS_TAG_parse(const char *str) {
    for (size_t i = 0;
         i < sizeof(POS_TAG_literals) / sizeof(POS_TAG_literals[0]); i++) {
        if (strncmp(POS_TAG_literals[i], str, strlen(POS_TAG_literals[i])) ==
            0) {
            return (enum POS_TAG)(1 << i);
        }
    }

    return POS_TAG_NULL;
}

words_t *phrase_segment(phrase_t *phrase, segmentation_t *segmentation) {
    assert(phrase && segmentation);

    words_t *words      = calloc(1, sizeof(words_t));
    int words_count     = segmentation_count(segmentation);
    words->count        = words_count;
    words->word_strings = calloc(words_count, sizeof(char *));
    words->pos_tags     = calloc(words_count, sizeof(enum POS_TAG));
    words->segmentation = *segmentation;

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
    assert(words->word_strings &&
           words->pos_tags); // problematic initialization

    for (int i = 0; i < words->count; i++) {
        free(words->word_strings[i]);
    }
    free(words->word_strings);
    free(words->pos_tags);
    free(words);
}

void words_tagging(words_t *words) {
    assert(words);
}

int words_eval(words_t *words) {
    assert(words);
    return -1;
}

void words_fprint(FILE *restrict stream, words_t *words) {
    assert(stream);
    assert(words);
    for (int i = 0; i < words->count - 1; i++) {
        fprintf(stream, "%s<%s> ", words->word_strings[i],
                POS_TAG_str(words->pos_tags[i]));
    }

    // print the last one separately, with no trailing space
    fprintf(stream, "%s<%s>", words->word_strings[words->count - 1],
            POS_TAG_str(words->pos_tags[words->count - 1]));

    fflush(stream);
}

void words_snprint(char str[restrict], size_t size, words_t *words) {
    assert(str);
    assert(words);

    char buffer[1024] = { 0 };

    memset(str, 0, size);

    for (int i = 0; i < words->count - 1; i++) {
        snprintf(buffer, sizeof(buffer), "%s<%s> ", words->word_strings[i],
                 POS_TAG_str(words->pos_tags[i]));
        strncat(str, buffer, size);
    }

    // print the last one separately, with no trailing space
    snprintf(buffer, sizeof(buffer), "%s<%s>",
             words->word_strings[words->count - 1],
             POS_TAG_str(words->pos_tags[words->count - 1]));
    strncat(str, buffer, size);
}
