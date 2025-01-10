//
//  token.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#ifndef token_h
#define token_h

#include "arena.h"
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

/// Implementation note: token_tokenize` borrows s.
/// Implementation note: caller must free return value
struct token *token_tokenize(char const *s);

/// Implementation note: caller must free return value
struct token *token_ftokenize(FILE *stream);

/// Implementation note: if you pass NULL to tok, `token_ftokenize_r` will
/// allocate a new `struct token` and return it If you pass a valid pointer to a
/// `struct token`, `token_ftokenize_r` will fill the memory that you pass to
/// it. If the caller passes tok as NULL then use `token_free` to free the newly
/// allocated returned token If the caller passes memory it owns as tok then it
/// is responsible to deal with **however** the caller must free `tok.value`
/// which is the char data of the token
struct token *token_ftokenize_r(struct token *tok, FILE *stream);

struct token *token_ftokenize_ar(struct arena_block *arena, struct token *tok,
                                 FILE *stream);

/// Implementation note: `token_fill` will allocate and copy s. It will **not**
/// take ownership of the memory pointed to by s
void token_fill(struct token *tok, char const *s, size_t len);

void token_freeall(struct token *tokens);
void token_free(struct token *tokens);

#endif
