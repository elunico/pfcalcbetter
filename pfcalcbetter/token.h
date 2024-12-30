//
//  token.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#ifndef token_h
#define token_h

#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token {
    char *value;
    struct token *prev;
    struct token *next;
};

/// Implentation note: `tokenize` borrows s.
struct token *tokenize(char const *s);
struct token *ftokenize(FILE *stream);

/// Implementation note: if you pass NULL to tok, `ftokenize_r` will allocate a new `struct token` and return it
/// If you pass a valid pointer to a `struct token`, `ftokenize_r` will fill the memory that you pass to it.
struct token *ftokenize_r(struct token *tok, FILE *stream);

/// Implementation note: `token_fill` will allocate and copy s. It will **not** take ownership of the memory pointed to by s
void token_fill(struct token *tok, char const* s, size_t len);

void free_tokens(struct token *tokens);
void free_token(struct token *tokens);

#endif
