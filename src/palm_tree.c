#include "phrase.h"
#include "phrase_filter.h"
#include "utils/dict.h"
#include "words.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *file = fopen("resource/vimtutor.txt", "rb");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    dict_t *dict = dict_init();
    if (!dict_load_file(dict, "resource/dict.txt")) {
        exit(EXIT_FAILURE);
    }

    char buffer[3000 * 4] = { 0 };
    char chunk[1024]      = { 0 };

    while ((fgets(chunk, sizeof(chunk), file))) {
        strncat(buffer, chunk, sizeof(buffer) - strlen(buffer) - 1);
    }
    fclose(file);

    phrase_list_t *ph_list = parse_paragraph(buffer);

    for (int i = 0; i < ph_list->count; i++) {
        phrase_t *p = ph_list->phrases[i];
        segmentation_t seg;

        phrase_filter_ordinal_number(p, &p->cn_char_seg, &seg);
        phrase_filter_cardinal_number(p, &seg, &seg);
        phrase_filter_dict_words(p, &seg, &seg, dict,
                                 PHRASE_DICT_FILTER_LONGEST);

        words_t *words = phrase_segment(p, &seg);
        words_fprint(stdout, words);
        puts("");
        words_free(words);
    }

    phrase_list_free(ph_list);
}
