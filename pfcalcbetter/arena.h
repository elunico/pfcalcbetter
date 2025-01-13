//
//  arena.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 1/3/25.
//

#ifndef arena_h
#define arena_h

#include "utils.h"
#include <stdlib.h>

struct arena {
    void *start;
    void *brk;
    void *end;
    // size_t byte_size;
    // size_t byte_cap;
};

struct arena *arena_alloc(size_t bytes);
struct arena *arena_calloc(size_t bytes);

void *arena_block_alloc(struct arena *, size_t bytes);

void arena_block_free(struct arena *, void *block);

void arena_free(struct arena *arena);

#endif /* arena_h */
