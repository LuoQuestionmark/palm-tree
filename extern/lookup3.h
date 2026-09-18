#pragma once

// function defined in `loopup3.c`, check that file for more details

#include <stddef.h>
#include <stdint.h>

void hashword2(const uint32_t *k, /* the key, an array of uint32_t values */
               size_t length,     /* the length of the key, in uint32_ts */
               uint32_t *pc,      /* IN: seed OUT: primary hash value */
               uint32_t *pb);     /* IN: more seed OUT: secondary hash value */
