//
//  stack.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include "stack.h"

#if defined(PF_NUM_LONG)
#define MISSING_SENTINEL __LONG_MAX__
#else
#define MISSING_SENTINEL NAN
#endif

#if defined(PF_NUM_LONG)
int ISEMPTY(pfnum_t stackresult) { return ((stackresult) == __LONG_MAX__); }
#else
int ISEMPTY(pfnum_t stackresult) { return isnan(stackresult); }
#endif

void stack_push(struct stack **s, pfnum_t t) {
    
    if (*s == NULL) {
        *s = malloc(sizeof(struct stack));
        (*s)->rToken = t;
        (*s)->next = NULL;
        (*s)->prev = NULL;
    } else {
        (*s)->next = malloc(sizeof(struct stack));
        (*s)->next->rToken = t;
        (*s)->next->prev = *s;
        *s = (*s)->next;
    }
}

pfnum_t stack_pop(struct stack **s) {
    if (*s == NULL)
        return MISSING_SENTINEL;

    pfnum_t ret = (*s)->rToken;
    struct stack *root = (*s)->prev;
    free(*s);
    *s = root;
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
