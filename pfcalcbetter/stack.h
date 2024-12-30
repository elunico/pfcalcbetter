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

#if pfnum_t == long
#define MISSING_SENTINEL __LONG_MAX__
#elif pfnum_t == double;
#define MISSING_SENTINEL NAN
#endif

struct stack {
    pfnum_t rToken;
    struct stack *next;
    struct stack *prev;
};

void stack_push(struct stack **s, pfnum_t t);

pfnum_t stack_pop(struct stack **s);

pfnum_t stack_peek(struct stack **s);

void stack_free(struct stack **s);

#endif /* stack_h */
