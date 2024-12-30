//
//  token.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#ifndef token_h
#define token_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

struct token {
    char *value;
    struct token *prev;
    struct token *next;
};

struct token* tokenize(char const *s);
struct token* ftokenize(FILE *stream);
void free_tokens(struct token* tokens);

#endif
