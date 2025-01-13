//
//  stack.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include "stack.h"
#include "arena.h"

#if defined(PF_NUM_LONG)
int ISEMPTY(pfnum_t stackresult) { return ((stackresult) == __LONG_MAX__); }
#else
int ISEMPTY(pfnum_t stackresult) { return isnan(stackresult); }
#endif

static void stack_push_alloca(struct stack **s, pfnum_t const res, struct stack *result) {
    if (*s == NULL) {
        *s = result;
        (*s)->rToken = res;
        (*s)->next = NULL;
        (*s)->prev = NULL;
    } else {
        (*s)->next = result;
        (*s)->next->rToken = res;
        (*s)->next->prev = *s;
        *s = (*s)->next;
    }
}

void stack_push(struct stack **s, pfnum_t const t) {
    void *result = malloc(sizeof(struct stack));
    return stack_push_alloca(s, t, result);
}

void stack_push_a(struct arena *arena, struct stack **s, pfnum_t const t) {
    void *result = arena_block_alloc(arena, sizeof(struct stack));
    return stack_push_alloca(s, t, result);
}

static pfnum_t stack_pop_inout(struct stack **s, struct stack **tofree) {
    if (*s == NULL)
        return MISSING_SENTINEL;

    pfnum_t ret = (*s)->rToken;
    struct stack *root = (*s)->prev;
    // free(*s);
    *tofree = *s;
    *s = root;
    return ret;
}

pfnum_t stack_pop(struct stack **s) {
    struct stack *tofree = NULL;
    pfnum_t ret = stack_pop_inout(s, &tofree);
    free(tofree);
    return ret;
}

pfnum_t stack_pop_a(struct arena *arena, struct stack **s) {
    struct stack *tofree;
    pfnum_t ret = stack_pop_inout(s, &tofree);
    arena_block_free(arena, tofree);
    return ret;
}

pfnum_t stack_peek(struct stack const *const *const s) {
    if (*s == NULL) {
        return MISSING_SENTINEL;
    }
    return (*s)->rToken;
}

void stack_free(struct stack **s) {
    while (!ISEMPTY(stack_pop(s)))
        ;
}
