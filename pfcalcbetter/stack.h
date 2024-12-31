//
//  stack.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#ifndef stack_h
#define stack_h

#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(PF_NUM_LONG)
#define MISSING_SENTINEL __LONG_MAX__
#else
#define MISSING_SENTINEL NAN
#endif

struct stack {
    pfnum_t rToken;
    struct stack *next;
    struct stack *prev;
};

/// Implementation note: s must not be NULL but `*s` maybe be NULL
void stack_push(struct stack **s, pfnum_t t);

/// Implementation note: s must not be NULL but `*s` maybe be NULL
pfnum_t stack_pop(struct stack **s);

/// Implementation note: s must not be NULL but `*s` maybe be NULL
pfnum_t stack_peek(struct stack const *const *const s);

/// Implementation note: s must not be NULL but `*s` maybe be NULL
void stack_free(struct stack **s);

#endif /* stack_h */
