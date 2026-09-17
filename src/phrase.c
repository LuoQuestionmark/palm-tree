#include "phrase.h"
#include "phrase_filter.h"
#include "utils/dict.h"
#include "utils/utf8_utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void phrase_snprint_per_cn_char(const phrase_t *phrase, char *dst,
                                       size_t maxlen) {
    assert(phrase != NULL && dst != NULL);

    char buffer[maxlen + 1];

    memset(dst, 0, maxlen);
    memset(buffer, 0, maxlen + 1);

    int seg_len = 0;
    int buffer_remain_size;
    for (size_t offset = 0; offset < phrase->length; offset++) {
        if (segmentation_at(&phrase->cn_char_seg, offset, &seg_len)) {
            buffer_remain_size = maxlen - strlen(buffer) - 1;
            if (seg_len > buffer_remain_size) {
                // buffer will overflow with this operation
                strncat(buffer, phrase->phrase + offset, buffer_remain_size);
                break;
            } else {
                // otherwise just copy the word, then a space to separate
                // chinese char
                strncat(buffer, phrase->phrase + offset, seg_len);
                strncat(buffer, " ", buffer_remain_size - 1);
                offset += seg_len - 1;
            }
        }
    }

    if (strlen(buffer) - 1 > maxlen) {
        fprintf(stderr,
                "Warning: buffer size is not big enough, output truncated\n");
        strncpy(dst, buffer, maxlen);
    } else {
        strncpy(dst, buffer, strlen(buffer) - 1);
    }
}

static void phrase_snprint_per_cn_word(const phrase_t *phrase, char *dst,
                                       size_t maxlen) {
    assert(phrase != NULL && dst != NULL);

    char buffer[maxlen + 1];

    memset(dst, 0, maxlen);
    memset(buffer, 0, maxlen + 1);

    int seg_len = 0;
    int buffer_remain_size;
    for (size_t offset = 0; offset < phrase->length; offset++) {
        if (segmentation_at(&phrase->word_seg, offset, &seg_len)) {
            buffer_remain_size = maxlen - strlen(buffer) - 1;
            if (seg_len > buffer_remain_size) {
                // buffer will overflow with this operation
                strncat(buffer, phrase->phrase + offset, buffer_remain_size);
                break;
            } else {
                // otherwise just copy the word, then a space to separate
                // chinese char
                strncat(buffer, phrase->phrase + offset, seg_len);
                strncat(buffer, " ", buffer_remain_size - 1);
                offset += seg_len - 1;
            }
        }
    }

    if (strlen(buffer) - 1 > maxlen) {
        fprintf(stderr,
                "Warning: buffer size is not big enough, output truncated\n");
        strncpy(dst, buffer, maxlen);
    } else {
        strncpy(dst, buffer, strlen(buffer) - 1);
    }
}

void segmentation_init(segmentation_t *segmentation) {
    assert(segmentation);

    // set the first bit of the first segmentation to 1, others to 0
    memset(segmentation, 0, sizeof(segmentation_t));
    segmentation->seg[0] = SEG_0;
}

void segmentation_add(segmentation_t *segmentation, const int offset) {
    assert(segmentation);
    assert(offset >= 0 && offset < MAX_SEGMENTATION_BYTE);

    int seg_data_number = offset / 64;
    int seg_reminder    = offset % 64;

    segmentation->seg[seg_data_number] |= (SEG_0 >> seg_reminder);
}

void segmentation_del(segmentation_t *segmentation, const int offset) {
    assert(segmentation);
    assert(offset >= 0 && offset < MAX_SEGMENTATION_BYTE);
    int seg_offset   = offset / 64;
    int seg_reminder = offset % 64;

    segmentation->seg[seg_offset] &= ~(SEG_0 >> seg_reminder);
}

void segmentation_pop(segmentation_t *segmentation, const int offset) {
    assert(segmentation);
    assert(offset >= 0 && offset < MAX_SEGMENTATION_BYTE);

    int seg_offset   = offset / 64;
    int seg_reminder = offset % 64;

    for (int i = seg_offset; i < MAX_SEGMENTATION_SEG; i++) {
        uint64_t seg = segmentation->seg[i];
        for (int j = 0; j < 64; j++) {
            if (i == seg_offset && j < seg_reminder) continue;
            if (seg & (SEG_0 >> j)) {
                segmentation_del(segmentation, seg_offset * 64 + j);
                return;
            }
        }
    }
}

void segmentation_pop_n(segmentation_t *segmentation, const int offset, int n) {
    for (int i = 0; i < n; i++) {
        segmentation_pop(segmentation, offset);
    }
}

int segmentation_count(const segmentation_t *segmentation) {
    assert(segmentation);

    int count = 0;
    for (int i = 0; i < MAX_SEGMENTATION_SEG; i++) {
        uint64_t seg = segmentation->seg[i];
        for (int j = 0; j < 64; j++) {
            if (seg & 0b1) {
                count += 1;
            }
            seg = (seg >> 1);
        }
    }

    return count;
}

bool segmentation_at(const segmentation_t *segmentation, const int offset,
                     int *len) {
    assert(segmentation);

    bool value = false;

    if (offset < 0 || offset >= MAX_SEGMENTATION_BYTE) {
        return false;
    }

    int seg_offset   = offset / 64;
    int seg_reminder = offset % 64;

    value = (segmentation->seg[seg_offset]) & (SEG_0 >> seg_reminder);

    if (!len || value == 0) {
        // if len if NULL or the offset is not a segmentation point, then return
        // the result directly, otherwise the program still calculate the size
        // of this segmentation
        return value;
    }

    *len = 1;
    for (int i = seg_offset; i < MAX_SEGMENTATION_SEG; i++) {
        uint64_t seg = segmentation->seg[i];
        for (int j = 0; j < 64; j++) {
            if (i == seg_offset && j <= seg_reminder) continue;
            if (seg & (SEG_0 >> j)) {
                return value;
            }
            *len += 1;
        }
    }

    return value;
}

bool segmentation_get(const segmentation_t *segmentation, const int index,
                      int *offset, int *len) {
    assert(segmentation);
    assert(offset && len);

    if (index < 0 || index > MAX_SEGMENTATION_BYTE) return false;

    int remains = index;
    bool found  = false;
    for (int i = 0; i < MAX_SEGMENTATION_SEG; i++) {
        uint64_t seg = segmentation->seg[i];
        for (int j = 0; j < 64; j++) {
            // printf("%064lb, %064lb\n", seg & (SEG_0 >> j), (SEG_0 >> j));
            if (seg & (SEG_0 >> j)) {
                if (!found && remains > 0) {
                    remains -= 1;
                    continue;
                }
                if (!found && remains == 0) {
                    found   = true;
                    *offset = i * 64 + j;
                    continue;
                }
                if (found) {
                    *len = (i * 64 + j) - *offset;
                    return true;
                }
            }
        }
    }
    if (found) {
        *len = (MAX_SEGMENTATION_BYTE) - *offset;
        return true;
    }
    return false;
}

phrase_t *phrase_init(const char *phrase_string) {
    phrase_t *phrase = calloc(1, sizeof(phrase_t));
    if (phrase == NULL) {
        perror("calloc");
        return NULL;
    }

    phrase->length = strlen(phrase_string);
    phrase->phrase = strdup(phrase_string);

    bool character_seg_ok = phrase_utf8_char_segmentation(phrase);
    if (!character_seg_ok) {
        fprintf(stderr, "input string is not a valid utf8 string: %s\n",
                phrase_string);
        return NULL;
    }

    return phrase;
}

void phrase_free(phrase_t *phrase) {
    if (phrase == NULL) return;
    if (phrase->phrase) free(phrase->phrase);

    free(phrase);
}

void phrase_snprint(const phrase_t *phrase, enum phrase_snprint_type print_type,
                    char *dst, size_t maxlen) {
    assert(phrase != NULL && dst != NULL);
    memset(dst, 0, maxlen);

    switch (print_type) {
    case PHRASE_SNPRINT_ORIGINAL:
        strncpy(dst, phrase->phrase, maxlen);
        break;
    case PHRASE_SNPRINT_PER_CN_CHAR:
        if (phrase->cn_char_seg.seg[0] == 0) {
            fprintf(stderr, "cannot generate phrase with character "
                            "segmentation, segmentation is not defined\n");
            return;
        }
        phrase_snprint_per_cn_char(phrase, dst, maxlen);
        break;
    case PHRASE_SNPRINT_PER_CN_WORD:
        if (phrase->cn_char_seg.seg[0] == 0) {
            fprintf(stderr, "cannot generate phrase with word segmentation, "
                            "segmentation is not defined\n");
            return;
        }
        phrase_snprint_per_cn_word(phrase, dst, maxlen);
        break;
    }
}

bool phrase_utf8_char_segmentation(phrase_t *phrase) {
    assert(phrase);
    if (phrase->phrase == NULL) return false;
    if (strlen(phrase->phrase) == 0) return false;

    segmentation_init(&phrase->cn_char_seg);

    int phrase_size = (int)strlen(phrase->phrase);

    for (int i = 0; i < MAX_SEGMENTATION_BYTE && i < phrase_size;
         /*i += offset, already implemented*/) {
        char *current = phrase->phrase + i;
        int offset    = utf8_char_len(current);
        if (offset < 0) return false;

        segmentation_add(&phrase->cn_char_seg, i);
        i += offset;
    }

    return true;
}

void phrase_base_word_segmentation(phrase_t *phrase, dict_t *dict, int seg_c,
                                   segmentation_t *seg_v[seg_c]) {
    assert(phrase && dict);
    if (seg_v == NULL) return;
    if (seg_c < 1) return;

    // TODO: complete filter process

    segmentation_t *seg = calloc(1, sizeof(segmentation_t));

    phrase_filter_dict_words(phrase, &phrase->cn_char_seg, seg, dict,
                             PHRASE_DICT_FILTER_LONGEST);

    seg_v[0] = seg;
}

phrase_list_t *phrase_list_init() {
    phrase_list_t *ph_list = calloc(1, sizeof(phrase_list_t));
    ph_list->size          = 1;
    ph_list->count         = 0;
    ph_list->phrases       = calloc(1, sizeof(phrase_t *));

    return ph_list;
}

void phrase_list_free(phrase_list_t *ph_list) {
    if (ph_list == NULL) return;
    if (ph_list->phrases != NULL) {
        for (int i = 0; i < ph_list->count; i++) {
            phrase_free(ph_list->phrases[i]);
        }
        free(ph_list->phrases);
    }
    free(ph_list);
}

void phrase_list_append(phrase_list_t *ph_list, phrase_t *phrase) {
    assert(ph_list && phrase);

    if (ph_list->count == ph_list->size) {
        ph_list->size *= 2;
        ph_list->phrases =
            realloc(ph_list->phrases, ph_list->size * sizeof(phrase_t *));
    }

    ph_list->phrases[ph_list->count++] = phrase;
}

phrase_list_t *parse_paragraph(const char *phrase_strings) {
    if (phrase_strings == NULL) return NULL;

    phrase_list_t *ph_list = phrase_list_init();

    char buffer[PHRASE_BUFFER_SIZE] = { 0 };
    char next[4];

    for (const char *head = phrase_strings; head != NULL;
         head             = utf8_char_consume(head, next)) {

        if (utf8_should_ignore_char(next, UTF8_IGNORE_ALL)) {
            continue;
        }
        if (utf8_end_of_cn_phrase(next)) {
            strncat(buffer, next, sizeof(buffer) - strlen(buffer) - 1);
            phrase_list_append(ph_list, phrase_init(buffer));
            memset(buffer, 0, sizeof(buffer));
        } else {
            strncat(buffer, next, sizeof(buffer) - strlen(buffer) - 1);
        }
    }

    return ph_list;
}
