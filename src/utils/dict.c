#include "utils/dict.h"
#include "lookup3.h"
#include "utils/bloom_filter.h"
#include "utils/utf8_utils.h"
#include "words.h"
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

static void regex_sanitize(const char *src, char *dst, size_t len) {
    assert(src);
    assert(dst && len > 0);

    static const char special_chars[] = { '#', '.', '[', ']', '{', '}',
                                          '^', '?', '+', '|', '*' };

    memset(dst, 0, len);
    for (size_t i = 0, j = 0; i < strlen(src) && j < len; i++, j++) {
        for (size_t k = 0; k < sizeof(special_chars); k++) {
            if (src[i] == special_chars[k]) {
                dst[j++] = '\\';
                break;
            }
        }
        dst[j] = src[i];
    }
}

static void wc_list_raw_append(wc_cell_t *wc_list, char *word, int postags) {
    assert(wc_list);
    assert(word);

    wc_cell_t *current;
    for (current = wc_list; current->next != NULL; current = current->next) {
        if (current->word == NULL) break;
    }
    // post condition: (current->next == NULL) or (current->word == NULL)

    if (current->word == NULL) {
        current->word            = word;
        current->word_categories = postags;
        return;
    }
    // post condition: (current->word != NULL) and (current->next = NULL)

    current->next                  = wc_list_init();
    current->next->word            = word;
    current->next->word_categories = postags;
    return;
}

static uint32_t wc_hash(const char *word) {
    char buffer[256] = { 0 };
    strncpy(buffer, word, sizeof(buffer));

    uint32_t index =
        hashlittle(buffer, (strlen(buffer) / 4 + 1) * 4, HASH_SEED);

    // for (size_t i = 0; i < strlen(word); i++) {
    //     index = hashlittle(word + i, 1, index);
    // }
    return index;
}

static void wc_hashtable_rehash(int width, wc_cell_t **raw_table, char *word,
                                int postags) {
    uint32_t index = wc_hash(word);
    if (raw_table[index % width] == NULL) {
        raw_table[index % width] = wc_list_init();
    }
    wc_list_raw_append(raw_table[index % width], word, postags);
}

dict_t *dict_init() {
    dict_t *dict       = calloc(1, sizeof(dict_t));
    dict->bloom_filter = bloom_init();

    return dict;
}

void dict_free(dict_t *dict) {
    if (dict == NULL) return;
    if (dict->bloom_filter) {
        bloom_free(dict->bloom_filter);
    }

    free(dict);
}

bool dict_load_file(dict_t *dict, const char *filename) {
    assert(dict);
    assert(filename);
    bool ret = true;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return false;
    }

    size_t chunk_size =
        fread(dict->buffer, sizeof(char), MAX_DICT_BUFFER_SIZE - 1, file);

    if (ferror(file)) {
        perror("fread");
        ret = false;
        goto dict_clean;
    }

    dict->buffer[chunk_size] = '\0';
    dict->loaded             = true;

    char line_buffer[1024];
    rewind(file);
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        line_buffer[strlen(line_buffer) - 1] = '\0';
        bloom_add(dict->bloom_filter, line_buffer);
    }

dict_clean:
    fclose(file);
    return ret;
}

bool dict_exist(const dict_t *dict, const char *word) {
    assert(dict != NULL);
    assert(dict->loaded);

    if (word == NULL || strlen(word) == 0) return false;

    if (!bloom_exist(dict->bloom_filter, word)) {
        return false;
    }

    // comment out the old solution with regex matching, which is slow!
    bool ret = true;

    char word_sanitized[1024];
    regex_sanitize(word, word_sanitized, sizeof(word_sanitized));

    char regex_pattern[1024] = { 0 };
    snprintf(regex_pattern, sizeof(regex_pattern), "^%s\n", word_sanitized);

    regex_t regex;

    if (regcomp(&regex, regex_pattern, REG_NOSUB | REG_NEWLINE) != 0) {
        perror("regcomp");
        ret = false;
        goto reg_clean;
    };

    if (regexec(&regex, dict->buffer, 0, NULL, 0) != 0) {
        // no match
        ret = false;
        goto reg_clean;
    }

reg_clean:
    regfree(&regex);
    return ret;
}

wc_cell_t *wc_list_init() {
    return calloc(1, sizeof(wc_cell_t));
}

void wc_list_free(wc_cell_t *wc_list, bool free_word) {
    if (wc_list == NULL) return;

    if (free_word && wc_list->word) {
        free(wc_list->word);
    }

    if (wc_list->next) {
        wc_list_free(wc_list->next, free_word);
    }
    free(wc_list);
}

int wc_list_count(wc_cell_t *wc_list) {
    if (wc_list == NULL) return -1;

    int count = 0;
    for (wc_cell_t *current = wc_list; current != NULL;
         current            = current->next) {
        if (current->word) {
            count += 1;
        }
    }

    return count;
}

void wc_list_append(wc_cell_t *wc_list, const char *word, enum POS_TAG postag) {
    assert(wc_list);
    assert(word);
    assert(postag != POS_TAG_NULL);

    wc_cell_t *current;
    for (current = wc_list; current->next != NULL; current = current->next) {
        // case 1: current is a cell with no word
        if (current->word == NULL) break;

        // case 2: current is a cell with the exact word, and next is not null
        if (strncmp(current->word, word, strlen(current->word)) == 0) {
            current->word_categories |= postag;
            return;
        }
    }

    // case 3: next is NULL and current cell is empty
    if (current->word == NULL) {
        current->word            = strdup(word);
        current->word_categories = postag;
        return;
    }

    // case 4: next is NULL but current cell is the exact word
    if (strncmp(current->word, word, strlen(current->word)) == 0) {
        current->word_categories |= postag;
        return;
    }

    // case 5: next is NULL, current is neither NULL nor the exact word
    current->next                  = wc_list_init();
    current->next->word            = strdup(word);
    current->next->word_categories = postag;
    return;
}

int32_t wc_list_get(wc_cell_t *wc_list, const char *word) {
    assert(word);
    if (wc_list == NULL) return 0;

    // this function's ret value is specified with int32 type, as the word
    // categories is of such length (bit array of enum). moreover, the value 0
    // is exact the "NULL" word category, a.k.a. "not found"

    int32_t ret = 0;

    for (wc_cell_t *current = wc_list; current != NULL;
         current            = current->next) {
        if (!current->word) continue;

        if (strncmp(current->word, word, strlen(current->word)) == 0) {
            ret = current->word_categories;
            break;
        }
    }

    return ret;
}

wc_hashtable_t *wc_hashtable_init() {
    wc_hashtable_t *hashtable = calloc(1, sizeof(wc_hashtable_t));

    if (hashtable == NULL) {
        perror("calloc");
        return NULL;
    }

    hashtable->hashtable_width = WC_HASHTABLE_DEFAULT_WIDTH;
    hashtable->hashtable =
        calloc(WC_HASHTABLE_DEFAULT_WIDTH, sizeof(wc_cell_t *));

    if (hashtable->hashtable == NULL) {
        perror("calloc");
        free(hashtable);
        return NULL;
    }

    return hashtable;
}

void wc_hashtable_free(wc_hashtable_t *hashtable) {
    if (hashtable == NULL) return;

    assert(hashtable->hashtable);
    for (int i = 0; i < hashtable->hashtable_width; i++) {
        if (hashtable->hashtable[i] == NULL) continue;

        wc_list_free(hashtable->hashtable[i], true);
    }
    free(hashtable->hashtable);

    free(hashtable);
}

// double the width
wc_hashtable_t *wc_hashtable_resize(wc_hashtable_t *hashtable) {
    assert(hashtable);

    int new_width         = 2 * hashtable->hashtable_width;
    wc_cell_t **new_table = calloc(new_width, sizeof(wc_cell_t *));

    for (int i = 0; i < hashtable->hashtable_width; i++) {
        if (hashtable->hashtable[i] == NULL) continue;
        wc_cell_t *current = hashtable->hashtable[i];
        while (true) {
            wc_hashtable_rehash(new_width, new_table, current->word,
                                current->word_categories);

            if (current->next == NULL) break;
            current = current->next;
        }
    }

    for (int i = 0; i < hashtable->hashtable_width; i++) {
        if (hashtable->hashtable[i] == NULL) continue;
        // during this operation, no word string is freed, since they are
        // "moved" directly to the new table, when the hashtable is finally
        // freed, all strings will be freed eventually
        wc_list_free(hashtable->hashtable[i], false);
    }
    free(hashtable->hashtable);

    hashtable->hashtable_width = new_width;
    hashtable->hashtable       = new_table;

    return hashtable;
}

void wc_hashtable_append(wc_hashtable_t *hashtable, const char *word,
                         enum POS_TAG postag) {
    assert(hashtable);
    assert(hashtable->hashtable);
    assert(word);
    assert(utf8_char_len(word) > 0);

    uint32_t index = wc_hash(word);
    if (hashtable->hashtable[index % hashtable->hashtable_width] &&
        wc_list_count(
            hashtable->hashtable[index % hashtable->hashtable_width]) >
            WC_HASHTABLE_MAX_LENGTH) {
        wc_hashtable_resize(hashtable);

        // TODO: under extreme situation, it is possible to trigger resize each
        // time a new member is added
    }

    if (hashtable->hashtable[index % hashtable->hashtable_width] == NULL) {
        hashtable->hashtable[index % hashtable->hashtable_width] =
            wc_list_init();
    }
    wc_list_append(hashtable->hashtable[index % hashtable->hashtable_width],
                   word, postag);
}

int32_t wc_hashtable_get(const wc_hashtable_t *hashtable, const char *word) {
    assert(hashtable);
    assert(word);

    uint32_t index = wc_hash(word);

    int val = wc_list_get(
        hashtable->hashtable[index % hashtable->hashtable_width], word);
    return val;
}

cat_dict_t *cat_dict_init() {
    cat_dict_t *ret            = calloc(1, sizeof(cat_dict_t));
    ret->bloom_filter          = bloom_init();
    ret->word_categories_table = wc_hashtable_init();

    return ret;
}

void cat_dict_free(cat_dict_t *cat_dict) {
    if (cat_dict == NULL) return;

    bloom_free(cat_dict->bloom_filter);
    wc_hashtable_free(cat_dict->word_categories_table);

    free(cat_dict);
}

bool cat_dict_load(cat_dict_t *cat_dict, const char *filename) {
    assert(cat_dict);
    assert(filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return false;
    }

    char line_buffer[1024]   = { 0 };
    char tc_word_buffer[256] = { 0 };
    char sc_word_buffer[256] = { 0 };
    char postag_buffer[16]   = { 0 };

    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        if (sscanf(line_buffer, "%s %s %s\n", tc_word_buffer, sc_word_buffer,
                   postag_buffer) == 0) {
            continue;
        }

        assert(utf8_char_len(tc_word_buffer) > 0);
        assert(utf8_char_len(sc_word_buffer) > 0);

        enum POS_TAG postag = POS_TAG_parse(postag_buffer);
        if (postag == POS_TAG_NULL) continue;

        cat_dict_add(cat_dict, sc_word_buffer, postag);
        cat_dict_add(cat_dict, tc_word_buffer, postag);
    }

    cat_dict->loaded = true;

    fclose(file);
    return true;
}

void cat_dict_add(cat_dict_t *cat_dict, const char *word, enum POS_TAG postag) {
    bloom_add(cat_dict->bloom_filter, word);
    wc_hashtable_append(cat_dict->word_categories_table, word, postag);
}

int32_t cat_dict_lookup(cat_dict_t *cat_dict, const char *word) {
    assert(cat_dict);
    if (word == NULL || word[0] == '\0') return 0;
    if (!bloom_exist(cat_dict->bloom_filter, word)) return 0;

    int32_t ret = wc_hashtable_get(cat_dict->word_categories_table, word);
    return ret;
}

bool cat_dict_lookup_unique(cat_dict_t *cat_dict, const char *word,
                            enum POS_TAG *postag) {
    assert(cat_dict);
    assert(postag);

    int32_t categories = cat_dict_lookup(cat_dict, word);
    if (!powerof2(categories)) return false;

    *postag = (enum POS_TAG)categories;

    return true;
}
