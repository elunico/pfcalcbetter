//
//  arena.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 1/3/25.
//

#include "arena.h"
#include "utils.h"

struct arena_header {
    size_t size;
    int flags;
};

struct arena_block *arena_create(size_t bytes) {
    void *block = malloc(bytes + sizeof(struct arena_block));

    struct arena_block *b = (struct arena_block *)block;
    void *beg = ((void *)b) + sizeof(struct arena_block);
    b->beginning = beg;
    b->brk = beg;
    b->byte_size = 0;
    b->byte_cap = bytes;
    debug("arena from malloc: %p\n", b);
    return b;
}

void *arena_block_alloc(struct arena_block *arena, size_t bytes) {
    // round up to the next multiple of 8
    bytes = (bytes + 7) & ~7;
    bytes += sizeof(struct arena_header);

    if ((arena->byte_size + bytes) > arena->byte_cap) {
#ifndef NDEBUG
        fprintf(stderr, "Allocation is oversized for arena\n");
#endif
        return NULL;
        // NOTE: reallocation is not possible because existing pointers would
        // then be invalidated
        //        debug("arena to be realloc: %p\n", (void*)*arena);
        //
        //        struct arena_block * newarena = realloc((void*)*arena,
        //        b->byte_cap
        //        * 2); newarena->byte_cap *= 2; newarena->beginning = ((void*)
        //        newarena) + sizeof(struct arena_header); newarena->brk =
        //        ((void *) newarena) + newarena->byte_size;
        //
        //        *arena = newarena;
        //        b = *arena;
    }

    void *bstart = arena->brk;
    arena->brk += bytes;
    arena->byte_size += bytes;
    require((arena->brk - bstart) == bytes, "arena block allocation failed");

    struct arena_header *header = (struct arena_header *)bstart;
    header->size = bytes;
    header->flags = 0x1;
    debug("allocating %lu bytes at %p\n", bytes,
          bstart + sizeof(struct arena_header));
    return bstart + sizeof(struct arena_header);
}

void arena_block_free(struct arena_block *arena, void *block) {
    debug("freeing block: %p\n", block);
    // if it is the first block walk brk back
    struct arena_header *block_start =
        (struct arena_header *)(block - sizeof(struct arena_header));
    if (block_start == (arena->brk - block_start->size)) {
        arena->byte_size -= block_start->size;
        arena->brk = block_start;
        debug("arena brk: %p\n", arena->brk);
    }

    // TODO: no op because contiguous block can't be freed unless its at the end
}

void arena_free(struct arena_block *arena) { free(arena); }
