#include "patterns/digits.h"
#include <assert.h>

int main() {
    assert(is_ascii_digits("12345") == 5);
    assert(is_cn_fw_digits("１２３") == 3);
    assert(is_cn_char_digits("九百八十二") == 5);
}
