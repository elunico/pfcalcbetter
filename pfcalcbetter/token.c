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
    if (!nextwalker) {
        return NULL;
    }
    token_fill(nextwalker, start, len);
    return nextwalker;
}

void token_fill_a(struct arena *arena, struct token *tok, char const *s,
                  size_t len) {
    tok->value = arena_block_alloc(arena, sizeof(char) * (len + 1));
    if (tok->value == NULL) {
        abort();
    }
    memcpy(tok->value, s, len);
    tok->value[len] = '\0';
}

struct token *token_new_a(struct arena *arena, char const *s, size_t len) {
    struct token *nextwalker = arena_block_alloc(arena, sizeof(struct token));
    // struct token* nextwalker = calloc(1, sizeof(struct token));
    if (!nextwalker) {
        return NULL;
    }
    token_fill_a(arena, nextwalker, s, len);
    return nextwalker;
}

struct token *token_append(struct token **list, struct token *newtok) {
    require(newtok != NULL);
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

void get_next_token(FILE *stream, char **buf, int *idx, size_t *cap) {
    int c;
    while ((c = fgetc(stream)) != EOF && !isspace(c)) {
        (*buf)[(*idx)++] = c;
        if (*idx == *cap) {
            // fatal("Dynamic allocation not supported");
            char *old = *buf;
            if (*cap == 4096) {
                info("malloc called for extremely big token (EBT)");
                // initial buffer is stack allocated so no free()
                *buf = malloc((*cap *= 2) * sizeof(char));
                if ((*buf) == NULL) {
                    fatal("malloc failed");
                }
            } else {
                info("realloc called for gargantuanly big token (GBT)");
                // if cap is bigger, then it must be heap memory so realloc
                *buf = realloc(buf, (*cap *= 2) * sizeof(char));
                if (!*buf) {
                    free(old);
                    fatal("realloc could not allocate storage for token");
                }
            }
        }
    }
}

struct token *token_ftokenize(FILE *stream) {
    struct token *root = NULL;
    struct token *walker = root;
    size_t cap = 4096;
    char sbuf[cap];
    char *buf = sbuf;
    int idx = 0;
    while (!feof(stream) && !ferror(stream)) {
        get_next_token(stream, &buf, &idx, &cap);

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
    size_t cap = 4096;
    char sbuf[cap];
    char *buf = sbuf;
    int idx = 0;
    while (!feof(stream) && !ferror(stream)) {
        get_next_token(stream, &buf, &idx, &cap);

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

struct token *token_ftokenize_ar(struct arena *arena, struct token *tok,
                                 FILE *stream) {
    size_t cap = 4096;
    char sbuf[cap];
    char *buf = sbuf;
    int idx = 0;
    int c;
    while (!feof(stream) && !ferror(stream)) {
        get_next_token(stream, &buf, &idx, &cap);

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

void token_free(struct token *tokens) {
    while (tokens != NULL) {
        struct token *p = tokens->next;
        free(tokens->value);
        free(tokens);
        tokens = p;
    }
}
