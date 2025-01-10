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

struct arena_block {
    void *beginning;
    void *brk;
    size_t byte_size;
    size_t byte_cap;
};

struct arena_block *arena_create(size_t bytes);

void *arena_block_alloc(struct arena_block *, size_t bytes);

void arena_block_free(struct arena_block *, void *block);

void arena_free(struct arena_block *arena);

#endif /* arena_h */
