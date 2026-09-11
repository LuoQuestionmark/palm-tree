#include "phrase.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void segmentation_init(segmentation_t *segmentation) {
    assert(segmentation);

    // set the first bit of the first segmentation to 1, others to 0
    memset(segmentation, 0, sizeof(segmentation_t));
    segmentation->seg[0] = SEG_0;
}

void segmentation_add(segmentation_t *segmentation, const int offset) {
    assert(segmentation);
    assert(offset >= 0 && offset < (int)(MAX_SEGMENTATION * 64));

    int seg_data_number = offset / 64;
    int seg_reminder    = offset % 64;

    segmentation->seg[seg_data_number] |= (SEG_0 >> seg_reminder);
}

void segmentation_del(segmentation_t *segmentation, const int offset) {
    assert(segmentation);
    assert(offset >= 0 && offset < (int)(MAX_SEGMENTATION * 64));
    int seg_offset   = offset / 64;
    int seg_reminder = offset % 64;

    segmentation->seg[seg_offset] &= ~(SEG_0 >> seg_reminder);
}

int segmentation_count(const segmentation_t *segmentation) {
    assert(segmentation);

    int count = 0;
    for (int i = 0; i < MAX_SEGMENTATION; i++) {
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

    if (offset < 0 || offset >= 8 * 64) {
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
    for (int i = seg_offset; i < MAX_SEGMENTATION; i++) {
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

    if (index < 0 || index > MAX_SEGMENTATION * 64) return false;

    int remains = index;
    bool found  = false;
    for (int i = 0; i < MAX_SEGMENTATION; i++) {
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
        *len = (MAX_SEGMENTATION * 64) - *offset;
        return true;
    }
    return false;
}

phrase_t *phrase_init(const char *phrase_string) {
    phrase_t *phrase = calloc(1, sizeof(phrase_t));
    if (phrase == NULL) {
        perror("calloc");
        abort();
    }

    phrase->length = strlen(phrase_string);
    phrase->phrase = strdup(phrase_string);

    return phrase;
}

void phrase_free(phrase_t *phrase) {
    if (phrase == NULL) return;
    if (phrase->phrase) free(phrase->phrase);

    free(phrase);
}

void phrase_snprint(const phrase_t *phrase, enum phrase_snprint_type print_type,
                    char *dst, size_t max_len) {
    assert(phrase != NULL && dst != NULL);

    switch (print_type) {
    case PHRASE_SNPRINT_ORIGINAL:
        strncpy(dst, phrase->phrase, max_len);
        break;
    case PHRASE_SNPRINT_PER_CN_CHAR:
        if (phrase->cn_char_seg.seg[0] == 0) {
            fprintf(stderr, "cannot generate phrase with character "
                            "segmentation, segmentation is not defined");
            return;
        }

        // TODO
        break;
    case PHRASE_SNPRINT_PER_CN_WORD:
        if (phrase->cn_char_seg.seg[0] == 0) {
            fprintf(stderr, "cannot generate phrase with word segmentation, "
                            "segmentation is not defined");
            return;
        }
        // TODO
        break;
    }
}
