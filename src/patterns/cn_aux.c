#include "patterns/cn_aux.h"
#include "utils/utf8_utils.h"
#include "words.h"
#include <assert.h>
#include <string.h>

static const char cn_aux_grp1[][8] = { "能",   "会",   "會", "可以",
                                       "应该", "應該", "肯", "敢" };

static const char cn_aux_grp2[][8] = {
    "有", "没有", "沒有", "是", "为", "爲",
};

static const char cn_aux_grp3[][8] = {
    "了", "著", "着", "过", "過",
};

int is_chinese_auxiliary(const char *utf8_string,
                         const enum POS_TAG previous_pos) {
    assert(utf8_string);

    for (size_t i = 0; i < sizeof(cn_aux_grp1) / sizeof(cn_aux_grp1[0]); i++) {
        if (strncmp(utf8_string, cn_aux_grp1[i], strlen(cn_aux_grp1[i])) == 0) {
            return utf8_char_len(utf8_string);
        }
    }

    for (size_t i = 0; i < sizeof(cn_aux_grp2) / sizeof(cn_aux_grp2[0]); i++) {
        if (strncmp(utf8_string, cn_aux_grp2[i], strlen(cn_aux_grp2[i])) == 0) {
            return utf8_char_len(utf8_string);
        }
    }

    if (previous_pos == POS_TAG_VERB) {
        for (size_t i = 0; i < sizeof(cn_aux_grp3) / sizeof(cn_aux_grp3[0]);
             i++) {
            if (strncmp(utf8_string, cn_aux_grp3[i], strlen(cn_aux_grp3[i])) ==
                0) {
                return utf8_char_len(utf8_string);
            }
        }
    }

    return -1;
}
