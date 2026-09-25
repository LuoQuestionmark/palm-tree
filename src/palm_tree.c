#include "phrase.h"
#include "phrase_filter.h"
#include "postag/postag.h"
#include "utils/dict.h"
#include "words.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    dict_t *dict = dict_init();
    if (!dict_load_file(dict, "resource/dict.txt")) {
        fputs("fail to open dict.txt", stderr);
        exit(EXIT_FAILURE);
    }

    cat_dict_t *cat_dict = cat_dict_init();
    if (!cat_dict_load(cat_dict, "resource/cat_dict.txt")) {
        fputs("fail to open cat_dict.txt", stderr);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen("resource/vimtutor.txt", "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char input_text_buffer[3000 * 4] = { 0 };
    char chunk[1024]                 = { 0 };

    while ((fgets(chunk, sizeof(chunk), file))) {
        strncat(input_text_buffer, chunk,
                sizeof(input_text_buffer) - strlen(input_text_buffer) - 1);
    }
    fclose(file);

    phrase_list_t *ph_list = parse_paragraph(input_text_buffer);

    for (int i = 0; i < ph_list->count; i++) {
        phrase_t *p = ph_list->phrases[i];
        segmentation_t seg;

        phrase_filter_ordinal_number(p, &p->cn_char_seg, &seg);
        phrase_filter_cardinal_number(p, &seg, &seg);
        phrase_filter_latin(p, &seg, &seg);
        phrase_filter_dict_words(p, &seg, &seg, dict,
                                 PHRASE_DICT_FILTER_LONGEST);

        words_t *words = phrase_segment(p, &seg);

        postag_tag_dict_unique(words, cat_dict);
        postag_tag_cn_aux(words);
        postag_tag_num(words);
        postag_tag_latin(words);

        words_cprint(words);
        puts("");
        words_free(words);
    }

    phrase_list_free(ph_list);

    dict_free(dict);
    cat_dict_free(cat_dict);
}
