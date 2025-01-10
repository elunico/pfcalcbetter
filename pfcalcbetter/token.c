//
//  token.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include "token.h"

void token_fill(struct token *tok, char const *start, size_t len) {
    tok->value = malloc((len + 1) * sizeof(char));
    memcpy(tok->value, start, len);
    tok->value[len] = '\0';
}

struct token *token_new(char const *start, size_t len) {
    struct token *nextwalker = calloc(1, sizeof(struct token));
    token_fill(nextwalker, start, len);
    return nextwalker;
}

void token_fill_a(struct arena_block *arena, struct token *tok, char const *s,
                  size_t len) {
    tok->value = arena_block_alloc(arena, sizeof(char) * (len + 1));
    if (tok->value == NULL) {
        abort();
    }
    memcpy(tok->value, s, len);
    tok->value[len] = '\0';
}

struct token *token_new_a(struct arena_block *arena, char const *s,
                          size_t len) {
    struct token *nextwalker = arena_block_alloc(arena, sizeof(struct token));
    // struct token* nextwalker = calloc(1, sizeof(struct token));
    token_fill_a(arena, nextwalker, s, len);
    return nextwalker;
}

struct token *token_append(struct token **list, struct token *newtok) {
    if (*list != NULL) {
        (*list)->next = newtok;
        newtok->prev = *list;
        *list = newtok;
        return newtok;
    } else {
        *list = newtok;
        newtok->prev = NULL;
        return newtok;
    }
}

struct token *token_ftokenize(FILE *stream) {
    struct token *root = NULL;
    struct token *walker = root;
    char buf[4096];
    int idx = 0;
    char c;
    while (!feof(stream) && !ferror(stream)) {
        while ((c = fgetc(stream)) != EOF && !isspace(c)) {
            buf[idx++] = c;
            if (idx == 4096) {
                fail("Dynamic allocation not supported");
            }
        }

        buf[idx] = 0;
        if (idx > 0) {
            struct token *next = token_new(buf, idx);
            if (root == NULL) {
                root = token_append(&walker, next);
            } else {
                token_append(&walker, next);
            }
        }
        idx = 0;
    }
    return root;
}

struct token *token_ftokenize_r(struct token *tok, FILE *stream) {
    char buf[4096];
    int idx = 0;
    char c;
    while (!feof(stream) && !ferror(stream)) {
        while ((c = fgetc(stream)) != EOF && !isspace(c)) {
            buf[idx++] = c;
            if (idx == 4096) {
                fail("Dynamic allocation not supported");
            }
        }

        buf[idx] = 0;
        if (idx > 0) {
            if (tok == NULL) {
                debug("%s\n", "Warning: tok is NULL");
                return token_new(buf, idx);
            }
            token_fill(tok, buf, idx);
            return tok;
        }
        idx = 0;
    }
    return NULL;
}

struct token *token_ftokenize_ar(struct arena_block *arena, struct token *tok,
                                 FILE *stream) {
    char buf[4096];
    int idx = 0;
    char c;
    while (!feof(stream) && !ferror(stream)) {
        while ((c = fgetc(stream)) != EOF && !isspace(c)) {
            buf[idx++] = c;
            if (c == 0) {
                fail("c is zero");
            }
            if (idx == 4096) {
                fail("Dynamic allocation not supported");
            }
        }

        buf[idx] = 0;
        if (idx > 0) {
            if (tok == NULL) {
                debug("%s\n", "Warning: tok is NULL");
                return token_new_a(arena, buf, idx);
            }
            token_fill_a(arena, tok, buf, idx);
            return tok;
        }
        idx = 0;
    }
    return NULL;
}

struct token *token_tokenize(char const *s) {
    struct token *root = NULL;
    struct token *walker = root;
    char const *start = s;
    while (*s) {
        start = s;
        size_t len = 0;
        while (*s && *s != ' ') {
            s++;
            len++;
        }
        if (len > 0) {
            struct token *tok = token_new(start, len);
            token_append(&walker, tok);
            if (root == NULL) {
                root = walker;
            }
        }
        if (!*s) {
            break;
        }
        s = s + 1;
    }
    return root;
}

void token_free(struct token *t) {
    free(t->value);
    free(t);
}

void token_freeall(struct token *tokens) {
    if (tokens == NULL)
        return;

    while (tokens != NULL) {
        struct token *p = tokens->next;
        token_free(tokens);
        tokens = p;
    }
}
