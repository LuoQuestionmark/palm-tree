#include "utils/dict.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int ret = 0;

    dict_t *dict = dict_init();

    if (!dict_load_file(dict, "resource/dict.txt")) {
        fputs("fail to load dictionary", stderr);
        exit(EXIT_FAILURE);
    }

    assert(dict_exist(dict, "苹果"));
    assert(!dict_exist(dict, "风风风"));

    FILE *file = fopen("resource/dict.txt", "r");
    if (!file) {
        fputs("cannot open dictionary", stderr);
        ret = -1;
        goto free_dict;
    }

    float success = 0, total = 0;
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if (!fgets(buffer, sizeof(buffer), file)) break;
        buffer[strlen(buffer) - 1] = '\0';

        // if (dict_exist(dict, buffer)) {
        //     success += 1;
        // } else {
        //     printf("fail to match: %s\n", buffer);
        // }
        // total += 1;

        strcat(buffer, "加加好");
        if (dict_exist(dict, buffer)) {
            printf("false match: %s\n", buffer);
        } else {
            success += 1;
        }
        total += 1;
    }

    if (success / total < 0.95) {
        fprintf(stderr, "the dict have accuracy of %f, less than95%%\n",
                success / total);
        ret = -1;
        goto free_dict;
    }

    if (dict_exist(dict, "键应")) {
        fprintf(stderr, "false positive \"键应\"\n");
    }
    if (dict_exist(dict, "是一")) {
        fprintf(stderr, "false positive \"是一\"\n");
    }

free_dict:
    dict_free(dict);

    return ret;
}
