//
//  arena.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 1/3/25.
//

#include "arena.h"
#include "utils.h"
#include <string.h>

struct arena_header {
    size_t size;
    int flags;
};

void arena_init(size_t const bytes, struct arena *b) {
    void *beg = ((void *)b) + sizeof(struct arena);
    b->start = beg;
    b->brk = beg;
    b->end = b->start + bytes;
    // b->byte_size = 0;
    // b->byte_cap = bytes;
    debug("arena from malloc: %p\n", b);
}
struct arena *arena_alloc(size_t const bytes) {
    void *block = malloc(bytes + sizeof(struct arena));
    if (block == NULL) {
        return NULL;
    }
    arena_init(bytes, block);
    return block;
}

struct arena* arena_calloc(size_t const bytes) {
    void *block = calloc(bytes + sizeof(struct arena), 1);
    if (block == NULL) {
        return NULL;
    }
    arena_init(bytes, block);
    return block;
}

void *arena_block_alloc(struct arena *arena, size_t bytes) {
    // round up to the next multiple of 8
    bytes = (bytes + 7) & ~7;
    bytes += sizeof(struct arena_header);

    if ((arena->brk + bytes) >= arena->end) {
#ifndef NDEBUG
        critical("Allocation is oversized for arena");
#endif
        return NULL;
        // NOTE: reallocation is not possible because existing pointers would
        // then be invalidated
    }

    void *bstart = arena->brk;
    arena->brk += bytes;
    require((arena->brk - bstart) == bytes, "arena block allocation failed");

    struct arena_header *header = bstart;
    header->size = bytes;
    header->flags = 0x1;
    debug("allocating %lu bytes at %p\n", bytes,
          bstart + sizeof(struct arena_header));
    return bstart + sizeof(struct arena_header);
}

void arena_block_free(struct arena *arena, void *block) {
    if (block == NULL) {
        debug("called arena_block_free() with NULL block");
        return ;
    }
    debug("freeing block: %p\n", block);
    // if it is the first block walk brk back
    struct arena_header *block_start = block - sizeof(struct arena_header);
    if (block_start == (arena->brk - block_start->size)) {
        arena->brk = block_start;
        debug("arena brk: %p\n", arena->brk);
    }

    // TODO: no op because contiguous block can't be freed unless its at the end
}

void arena_free(struct arena *arena) { free(arena); }
