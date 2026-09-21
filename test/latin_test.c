#include "patterns/latin.h"
#include <assert.h>

int main() {
    assert(is_latin("abcde") == 5);
    assert(is_latin("汉文") == -1);
    assert(is_latin("🧌") == -1);
}
