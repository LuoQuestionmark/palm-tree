#include "phrase.h"
#include <assert.h>

int main() {
    segmentation_t seg1, seg2;
    segmentation_init(&seg1);
    segmentation_init(&seg2);

    assert(seg1.seg[0] == (unsigned long long)SEG_0);

    int offset1 = 27;
    int offset2 = 65;

    segmentation_add(&seg1, offset1);
    segmentation_add(&seg2, offset2);
    segmentation_del(&seg1, offset2);

    assert(seg1.seg[0] != seg2.seg[0]);

    int offset, len;
    segmentation_get(&seg1, 0, &offset, &len);
    assert(offset == 0);

    segmentation_get(&seg1, 1, &offset, &len);
    assert(offset == offset1);
    assert(offset + len == 64 * 8);

    segmentation_add(&seg1, offset2);
    segmentation_get(&seg1, 1, &offset, &len);
    assert(len == offset2 - offset1);

    assert(segmentation_at(&seg1, offset1, NULL));
    assert(segmentation_at(&seg1, offset1, &len));
    assert(len == offset2 - offset1);
}
