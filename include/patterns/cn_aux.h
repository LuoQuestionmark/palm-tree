#pragma once

// return number of chars, not actual utf8 chars
// (as each utf8 is composed by one or more char)

#include "words.h"

// previous: some auxiliary verbs should be identified regarded the previous
// word, ignored if NULL is given

int is_chinese_auxiliary(const char *utf8_string,
                         const enum POS_TAG previous_pos);
