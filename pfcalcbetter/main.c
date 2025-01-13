//
//  main.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>

#include "arena.h"
#include "args.h"
#include "stack.h"
#include "token.h"
#include "utils.h"

pfnum_t atopfnt(char const *s) {
#if defined(PF_NUM_LONG)
    return atol(s);
#elif defined(PF_NUM_DOUBLE)
    return atof(s);
#else
    return atof(s);
#endif
}

int is_number(char const *s) {
    return isdigit(s[0]) || (s[0] == '-' && isdigit(s[1]));
}

pfnum_t stack_consume(struct stack **s) {
    pfnum_t result = stack_pop(s);
    if (ISEMPTY(result)) {
        fatal("Not enough operators");
    }
    return result;
}

pfnum_t stack_consume_a(struct arena *arena, struct stack **s) {
    pfnum_t result = stack_pop_a(arena, s);
    if (ISEMPTY(result)) {
        fatal("Not enough operators");
    }
    return result;
}

#define CASE_RESOLVE(result, lhs, rhs, op)                                     \
    case (#op)[0]:                                                             \
        result = lhs op rhs;                                                   \
        break

#if defined(PFCALC_PREVENT_ZERO_DIV)
#define SAFETY_CHECK(rhs)                                                      \
    if (rhs == 0) {                                                            \
        rhs = 1;                                                               \
    }
#else
#define SAFETY_CHECK(rhs)                                                      \
    if (rhs == 0) {                                                            \
        fatal("Division by 0");                                                 \
    }
#endif

#define CASE_SAFE_RESOLVE(result, lhs, rhs, op)                                \
    case (#op)[0]:                                                             \
        SAFETY_CHECK(rhs)                                                      \
        result = lhs op rhs;                                                   \
        break

pfnum_t operate(pfnum_t lhs, pfnum_t rhs, char const *tok) {
    pfnum_t result;
    switch (tok[0]) {
        CASE_RESOLVE(result, lhs, rhs, +);
        CASE_RESOLVE(result, lhs, rhs, -);
        CASE_RESOLVE(result, lhs, rhs, *);
        CASE_SAFE_RESOLVE(result, lhs, rhs, /);
#ifdef PF_NUM_LONG
        CASE_SAFE_RESOLVE(result, lhs, rhs, %);
        CASE_RESOLVE(result, lhs, rhs, <<);
        CASE_RESOLVE(result, lhs, rhs, >>);
        CASE_RESOLVE(result, lhs, rhs, &);
        CASE_RESOLVE(result, lhs, rhs, ^);
        CASE_RESOLVE(result, lhs, rhs, |);
#endif
    default:
        fatalf("invalid operator: '%s'", tok);
        break;
    }

    return result;
}

void process_token(char *tok, struct stack **s) {
    if (is_number(tok)) {
        pfnum_t const v = atopfnt(tok);
        stack_push(s, v);
    } else if (ispunct(tok[0])) {
        require(strlen(tok) == 1, "operator must be of length 1");
        pfnum_t const rhs = stack_consume(s);
        pfnum_t const lhs = stack_consume(s);
        pfnum_t const result = operate(lhs, rhs, tok);
        debug("Step " PF_NUM_FMT " %s " PF_NUM_FMT " = " PF_NUM_FMT "\n", lhs,
              tok, rhs, result);
        stack_push(s, result);
    } else {
        fatalf("Unrecognized token: %p '%x' %s\n", tok, tok[0], tok);
    }
}

void process_token_a(struct arena *arena, char *tok, struct stack **s) {
    if (is_number(tok)) {
        pfnum_t const v = atopfnt(tok);
        stack_push_a(arena, s, v);
    } else if (ispunct(tok[0])) {
        require(strlen(tok) == 1, "operator must be of length 1");
        pfnum_t const rhs = stack_consume_a(arena, s);
        pfnum_t const lhs = stack_consume_a(arena, s);
        pfnum_t const result = operate(lhs, rhs, tok);
        debug("Step " PF_NUM_FMT " %s " PF_NUM_FMT " = " PF_NUM_FMT "\n", lhs,
              tok, rhs, result);
        stack_push_a(arena, s, result);
    } else {
        fatalf("Unrecognized token: %p '%x' %s\n", tok, tok[0], tok);
    }
}

pfnum_t pfCalculate(struct token *tokens) {
    struct stack *s = NULL;
    while (tokens != NULL) {
        char *tok = tokens->value;
        process_token(tok, &s);
        tokens = tokens->next;
    }

    pfnum_t const result = stack_pop(&s);

    if (!ISEMPTY(stack_pop(&s))) {
        fatal("Not enough operators");
    }

    return result;
}

void pfCalculate_r(struct token *t, struct stack **s, pfnum_t *result,
                   int done) {
    if (done) {
        pfnum_t const r = stack_pop(s);

        if (!ISEMPTY(stack_pop(s))) {
            fatal("Not enough operators");
        }

        *result = r;
        return;
    }
    char *tok = t->value;
    process_token(tok, s);
}

void pfCalculate_ar(struct arena *arena, struct token *t, struct stack **s,
                    pfnum_t *result, int done) {
    if (done) {
        pfnum_t const r = stack_pop_a(arena, s);

        if (!ISEMPTY(stack_pop_a(arena, s))) {
            fatal("Not enough operators");
        }

        *result = r;
        return;
    }
    char *tok = t->value;
    process_token_a(arena, tok, s);
}

int main(int argc, char *const argv[]) {
    struct arguments *args = arguments_parse(argc, argv);

    if (!arguments_wasset(args)) {
        fatal("Specify -i or -f FILE");
    }

    struct token *root;
    char *filename;
    if ((filename = arguments_getfilename(args)) == NULL) {
        printf("Please enter a postfix expression: ");
        size_t lineSize = 0;
        char *line = NULL;
        readline(line, stdin, lineSize);
        if (line[lineSize - 1] == '\n') {
            line[lineSize - 1] = '\0';
        } else {
            fatal("expected newline at end of input");
        }
        printf("Line: %s\n", line);
        root = token_tokenize(line);
        free(line);
        pfnum_t result = pfCalculate(root);
        printf("%s = " PF_NUM_FMT "\n", "expr", result);
        token_free(root);
    } else {
        FILE *f = fopen(filename, "r");
        struct token t = {0};
        struct stack *s = NULL;

        struct stat st;
        if (stat(filename, &st) != 0) {
            fatal("Could not stat file");
        }

        pfnum_t result;

        if (st.st_size > (1 << 19)) {
            fatal("File too large");
        }

        if (st.st_size > (1 << 10)) {
            struct arena *token_arena = arena_alloc(100);
            struct arena *stack_arena = arena_alloc(1 << 19);
            while (token_ftokenize_ar(token_arena, &t, f) != NULL) {
                pfCalculate_ar(stack_arena, &t, &s, NULL, 0);
                arena_block_free(token_arena, t.value);
            }
            pfCalculate_ar(token_arena, NULL, &s, &result, 1);
            arena_free(token_arena);
            arena_free(stack_arena);
        } else {
            while (token_ftokenize_r(&t, f) != NULL) {
                pfCalculate_r(&t, &s, NULL, 0);
                free(t.value);
            }
            pfCalculate_r(NULL, &s, &result, 1);
        }
        printf("%s = " PF_NUM_FMT "\n", "expr", result);
        fclose(f);
    }

    arguments_free(args);
    return 0;
}
