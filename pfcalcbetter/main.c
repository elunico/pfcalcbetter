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
        fail("Not enough operators");
    }
    return result;
}

pfnum_t stack_consume_a(struct arena_block *arena, struct stack **s) {
    pfnum_t result = stack_pop_a(arena, s);
    if (ISEMPTY(result)) {
        fail("Not enough operators");
    }
    return result;
}

pfnum_t operate(pfnum_t lhs, pfnum_t rhs, char const *tok) {
    pfnum_t result;
    switch (tok[0]) {
    case '+':
        result = lhs + rhs;
        break;
    case '-':
        result = lhs - rhs;
        break;
    case '*':
        result = lhs * rhs;
        break;
    case '/':
        result = safe_op(lhs, rhs, &divide);
        break;
#ifdef PF_NUM_LONG
    case '%':
        result = safe_op(lhs, rhs, &modulus);
        break;
    case '&':
        result = lhs & rhs;
        break;
    case '^':
        result = lhs ^ rhs;
        break;
    case '|':
        result = lhs | rhs;
        break;
#endif
    default:
        failf("invalid operator: '%s'", tok);
        break;
    }

    return result;
}

void process_token(char *tok, struct stack **s) {
    if (is_number(tok)) {
        pfnum_t v = atopfnt(tok);
        stack_push(s, v);
    } else if (ispunct(tok[0])) {
        require(strlen(tok) == 1, "operator must be of length 1");
        pfnum_t rhs = stack_consume(s);
        pfnum_t lhs = stack_consume(s);
        pfnum_t result = operate(lhs, rhs, tok);
        debug("Step " PF_NUM_FMT " %s " PF_NUM_FMT " = " PF_NUM_FMT "\n", lhs,
              tok, rhs, result);
        stack_push(s, result);
    } else {
        failf("Unrecognized token: %p '%x' %s\n", tok, tok[0], tok);
    }
}

void process_token_a(struct arena_block *arena, char *tok, struct stack **s) {
    if (is_number(tok)) {
        pfnum_t v = atopfnt(tok);
        stack_push_a(arena, s, v);
    } else if (ispunct(tok[0])) {
        require(strlen(tok) == 1, "operator must be of length 1");
        pfnum_t rhs = stack_consume_a(arena, s);
        pfnum_t lhs = stack_consume_a(arena, s);
        pfnum_t result = operate(lhs, rhs, tok);
        debug("Step " PF_NUM_FMT " %s " PF_NUM_FMT " = " PF_NUM_FMT "\n", lhs,
              tok, rhs, result);
        stack_push_a(arena, s, result);
    } else {
        failf("Unrecognized token: %p '%x' %s\n", tok, tok[0], tok);
    }
}

pfnum_t pfCalculate(struct token *tokens) {
    struct stack *s = NULL;
    while (tokens != NULL) {
        char *tok = tokens->value;
        process_token(tok, &s);
        tokens = tokens->next;
    }

    pfnum_t result = stack_pop(&s);

    if (!ISEMPTY(stack_pop(&s))) {
        fail("Not enough operators");
    }

    return result;
}

void pfCalculate_r(struct token *t, struct stack **s, pfnum_t *result,
                   int done) {
    if (done) {
        pfnum_t r = stack_pop(s);

        if (!ISEMPTY(stack_pop(s))) {
            fail("Not enough operators");
        }

        *result = r;
        return;
    }
    char *tok = t->value;
    process_token(tok, s);
}

void pfCalculate_ar(struct arena_block *arena, struct token *t,
                    struct stack **s, pfnum_t *result, int done) {
    if (done) {
        pfnum_t r = stack_pop_a(arena, s);

        if (!ISEMPTY(stack_pop_a(arena, s))) {
            fail("Not enough operators");
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
        fail("Specify -i or -f FILE");
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
            fail("expected newline at end of input");
        }
        printf("Line: %s\n", line);
        root = token_tokenize(line);
        free(line);
        pfnum_t result = pfCalculate(root);
        printf("%s = " PF_NUM_FMT "\n", "expr", result);
        token_freeall(root);
    } else {
        FILE *f = fopen(filename, "r");
        struct token t = {0};
        struct stack *s = NULL;

        struct stat st;
        if (stat(filename, &st) != 0) {
            fail("Could not stat file");
        }

        pfnum_t result;

        if (st.st_size > (1 << 19)) {
            fail("File too large");
        }

        if (st.st_size > (1 << 10)) {
            struct arena_block *arena = arena_create(100);
            struct arena_block *stack_arena = arena_create(1 << 19);
            // TODO: this calls malloc a lot because each read to moved to a
            // freshly allocated buffer. Maybe preallocate large chunks char *
            // data then put the appropriate (aligned) pointer to each struct
            // token object
            while (token_ftokenize_ar(arena, &t, f) != NULL) {
                pfCalculate_ar(stack_arena, &t, &s, NULL, 0);
                arena_block_free(arena, t.value);
            }
            pfCalculate_ar(arena, NULL, &s, &result, 1);
            arena_free(arena);
        } else {
            // TODO: this calls malloc a lot because each read to moved to a
            // freshly allocated buffer. Maybe preallocate large chunks char *
            // data then put the appropriate (aligned) pointer to each struct
            // token object
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
