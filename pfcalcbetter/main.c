//
//  main.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define assertionFailure(msg) do { fprintf(stderr, msg); exit(1); } while(0)

struct token {
    char *value;
    struct token *prev;
    struct token *next;
};

struct token* tokenize(char const *s) {
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
            struct token * nextwalker = calloc(1, sizeof(struct token));
            nextwalker->value = malloc((len + 1) * sizeof(char));
            memcpy(nextwalker->value, start, len);
            nextwalker->value[len] = '\0';
            
            if (walker != NULL) {
                walker->next = nextwalker;
                nextwalker->prev = walker;
                walker = nextwalker;
            } else {
                root = nextwalker;
                walker = nextwalker;
                nextwalker->prev = NULL;
            }
        }
        if (!*s) {
            break;
        }
        s = s + 1;
    }
    return root;
}

void free_tokens(struct token* tokens) {
    if (tokens == NULL) return;
    
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

struct stack {
    long rToken;
    struct stack *next;
    struct stack *prev;
};

void stack_push(struct stack **s, long t) {
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

/// Implementation detail: the return value of `stack_pop` must be freed by the caller of this function
long stack_pop(struct stack **s) {
    if (*s == NULL) return __LONG_MAX__;
    
    long ret = (*s)->rToken;
    struct stack *root = (*s)->prev;
    free(*s);
    *s = root;
    return ret;
}

long stack_peek(struct stack *s) {
    return s->rToken;
}

void stack_free(struct stack **s) {
    while (stack_pop(s) != __LONG_MAX__);
}

/// The return value of this function is owned by the caller and should be freed by the caller of `pfCalculate`
long pfCalculate(struct token *tokens) {
    struct stack *s = NULL;
    while (tokens != NULL) {
        char *tok = tokens->value;
        if (isdigit(tok[0])) {
            long v = atol(tok);
            stack_push(&s, v);
        } else if (ispunct(tok[0])) {
            long rhs = stack_pop(&s);
            if (rhs == __LONG_MAX__) {
                assertionFailure("Too many operators");
            }
            
            long lhs = stack_pop(&s);
            if (lhs == __LONG_MAX__) {
                assertionFailure("Too many operators");
            }
            
            if (!strcmp(tok, "**")) {
                // TODO: implement
            } else {
                long result;
                if (tok[0] == '+') {
                    result = lhs + rhs;
                } else if (tok[0] == '-') {
                    result = lhs - rhs;
                } else if (tok[0] == '*') {
                    result = lhs * rhs;
                } else if (tok[0] == '/') {
                    result = lhs / rhs;
                } else {
                    assertionFailure("invalid operator");
                }
                stack_push(&s, result);
            }
        }
        tokens = tokens->next;
    }
    
    long result = stack_pop(&s);
    
    if (stack_pop(&s) != __LONG_MAX__) {
        assertionFailure("Not enough operators");
    }
    
    return result;
}

int main(int argc, const char * argv[]) {
    
    printf("Please enter a postfix expression: ");
    size_t lineSize;
    char *line = fgetln(stdin, &lineSize);
    if (line[lineSize - 1] == '\n') {
        line[lineSize - 1] = '\0';
    } else {
        assertionFailure("expected newline at end of input");
    }
    
    struct token* const root = tokenize(line);
    long result = pfCalculate(root);
    printf("%s = %ld\n", line, result);
    free_tokens(root);
    return 0;
}
