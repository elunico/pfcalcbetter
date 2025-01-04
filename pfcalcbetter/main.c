//
//  main.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/28/24.
//

#include <ctype.h>
#include <math.h>
#include <stdio.h>

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

void process_token(char *tok, struct stack **s) {
    if (isdigit(tok[0])) {
        pfnum_t v = atol(tok);
        stack_push(s, v);
    } else if (ispunct(tok[0])) {
        require(strlen(tok) == 1, "operator must be of length 1");
        pfnum_t rhs = stack_pop(s);
        if (ISEMPTY(rhs)) {
            fail("Too many operators");
        }
        pfnum_t lhs = stack_pop(s);
        if (ISEMPTY(lhs)) {
            fail("Too many operators");
        }
        pfnum_t result;
        if (tok[0] == '+') {
            result = lhs + rhs;
        } else if (tok[0] == '-') {
            result = lhs - rhs;
        } else if (tok[0] == '*') {
            result = lhs * rhs;
        } else if (tok[0] == '/') {
            if (rhs == 0) {
#ifdef PFCALC_PREVENT_ZERO_DIV
                rhs = 1;
#else
                fail("Division by 0");
#endif
            }
            result = lhs / rhs;
        }
#ifdef PF_NUM_LONG
        else if (tok[0] == '%') {
            if (rhs == 0) {
                fail("Division by 0");
            }
            result = lhs % rhs;
        } else if (tok[0] == '&') {
            result = lhs & rhs;
        } else if (tok[0] == '^') {
            result = lhs ^ rhs;
        } else if (tok[0] == '|') {
            result = lhs | rhs;
        }
#endif
        else {
            failf("invalid operator: '%s'", tok);
        }

        debug("Step " PF_NUM_FMT " %s " PF_NUM_FMT " = " PF_NUM_FMT "\n", lhs, tok, rhs, result);
        stack_push(s, result);
    } else {
        failf("Unrecognized token: %s", tok);
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

void pfCalculate_r(struct token *t, struct stack **s, pfnum_t *result, int done) {
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

int main(int argc, char *const argv[]) {
    struct arguments *args = arguments_parse(argc, argv);

    if (!args->isStdin && !args->filename) {
        fail("Specify -i or -f FILE");
    }

    struct token *root;
    if (args->isStdin) {
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
        FILE *f = fopen(args->filename, "r");
        struct token t = {0};
        struct stack *s = NULL;
        pfnum_t result;
        // TODO: this calls malloc a lot because each read to moved to a freshly
        // allocated buffer. Maybe preallocate large chunks char * data then put
        // the appropriate (aligned) pointer to each struct token object
        while (token_ftokenize_r(&t, f) != NULL) {
            pfCalculate_r(&t, &s, NULL, 0);
            free(t.value);
        }
        pfCalculate_r(NULL, &s, &result, 1);
        printf("%s = " PF_NUM_FMT "\n", "expr", result);

        fclose(f);
    }

    arguments_free(args);
    return 0;
}
