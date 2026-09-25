#include "patterns/digits.h"
#include <assert.h>

int main() {
    assert(is_ascii_digits("12345") == 5);
    assert(is_cn_fw_digits("１２３") == 9);
    assert(is_cn_char_digits("九百八十二") == 15);
    assert(is_ordinal_cn_char_digits("九百八十二") == 0);
    assert(is_ordinal_cn_char_digits("第九百八十二") == 18);
    assert(is_cn_char_digits("一个") == 3);
}
