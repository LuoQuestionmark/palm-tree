#include "utils/bloom_filter.h"
#include <assert.h>

int main() {
    bloom_filter_t *filter = bloom_init();
    bloom_add(filter, "测试词汇");

    bool is_ok = bloom_exist(filter, "测试词汇");
    assert(is_ok);

    is_ok = bloom_exist(filter, "词汇");
    assert(!is_ok);

    bloom_free(filter);
}
