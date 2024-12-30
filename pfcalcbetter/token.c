//
//  token.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include "token.h"

struct token *token_append(struct token **list, char const *s, size_t len) {
    struct token *nextwalker = calloc(1, sizeof(struct token));
    nextwalker->value = malloc((len + 1) * sizeof(char));
    memcpy(nextwalker->value, s, len);
    nextwalker->value[len] = '\0';
    if (*list != NULL) {
        (*list)->next = nextwalker;
        nextwalker->prev = *list;
        *list = nextwalker;
        return nextwalker;
    } else {
        *list = nextwalker;
        nextwalker->prev = NULL;
        return nextwalker;
    }
}

struct token *ftokenize(FILE *stream) {
    struct token *root = NULL;
    struct token *walker = root;
    char buf[4096];
    int idx = 0;
    char c;
    while (!feof(stream) && !ferror(stream)) {
        while ((c = fgetc(stream)) != EOF && !isspace(c)) {
            buf[idx++] = c;
            if (idx == 4096) {
                assertionFailure("Dynamic allocation not supported");
            }
        }

        buf[idx] = 0;
        if (idx > 0) {
            if (root == NULL) {
                root = token_append(&walker, buf, idx);
            } else {
                token_append(&walker, buf, idx);
            }
        }
        idx = 0;
    }
    return root;
}

struct token *tokenize(char const *s) {
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
            if (root == NULL) {
                root = token_append(&walker, start, len);
            } else {
                token_append(&walker, start, len);
            }
        }
        if (!*s) {
            break;
        }
        s = s + 1;
    }
    return root;
}

void free_tokens(struct token *tokens) {
    if (tokens == NULL)
        return;

    while (tokens->next != NULL) {
        tokens = tokens->next;
    }
    while (tokens != NULL) {
        struct token *p = tokens->prev;
        free(tokens->value);
        free(tokens);
        tokens = p;
    }
}
