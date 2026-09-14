#include "utils/dict.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    dict_t *dict = dict_init();

    if (!dict_load_file(dict, "resource/cedict_ts.u8")) {
        fputs("fail to load dictionary", stderr);
        exit(EXIT_FAILURE);
    }

    assert(dict_exist(dict, "苹果"));
    assert(!dict_exist(dict, "风风风"));

    dict_free(dict);
}
