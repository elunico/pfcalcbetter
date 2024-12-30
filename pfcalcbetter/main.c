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
        pfnum_t rhs = stack_pop(s);
        if (rhs == MISSING_SENTINEL) {
            assertionFailure("Too many operators");
        }
        pfnum_t lhs = stack_pop(s);
        if (lhs == MISSING_SENTINEL) {
            assertionFailure("Too many operators");
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
                assertionFailure("Division by 0");
            }
            result = lhs / rhs;
        }
#if defined(PF_NUM_LONG)
        else if (tok[0] == '&') {
            result = lhs & rhs;
        } else if (tok[0] == '^') {
            result = lhs ^ rhs;
        } else if (tok[0] == '|') {
            result = lhs | rhs;
        }
#endif
        else {
            assertionFailure("invalid operator");
        }

        debug("Step " PF_NUM_FMT " %s " PF_NUM_FMT " = " PF_NUM_FMT "\n", lhs, tok, rhs, result);
        stack_push(s, result);
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

    if (stack_pop(&s) != MISSING_SENTINEL) {
        assertionFailure("Not enough operators");
    }

    return result;
}

void pfCalculate_r(struct token *t, struct stack **s, pfnum_t *result, int done) {
    if (done) {
        pfnum_t r = stack_pop(s);

        if (stack_pop(s) != MISSING_SENTINEL) {
            assertionFailure("Not enough operators");
        }

        *result = r;
        return;
    }
    char *tok = t->value;
    process_token(tok, s);
}

int main(int argc, char *const argv[]) {
    struct arguments args = parseargs(argc, argv);

    if (!args.isStdin && !args.filename) {
        assertionFailure("Specify -i or -f FILE\n");
    }

    struct token *root;
    if (args.isStdin) {
        printf("Please enter a postfix expression: ");
        size_t lineSize;
        char *line = fgetln(stdin, &lineSize);
        if (line[lineSize - 1] == '\n') {
            line[lineSize - 1] = '\0';
        } else {
            assertionFailure("expected newline at end of input");
        }
        root = tokenize(line);
        pfnum_t result = pfCalculate(root);
        printf("%s = " PF_NUM_FMT "\n", "expr", result);
        free_tokens(root);
    } else {
        FILE *f = fopen(args.filename, "r");
        struct token t = {0};
        struct stack *s = {0};
        pfnum_t result;
        // TODO: this calls malloc a lot because each read to moved to a freshly
        // allocated buffer. Maybe preallocate large chunks char * data then put
        // the appropriate (aligned) pointer to each struct token object
        while (ftokenize_r(&t, f) != NULL) {
            pfCalculate_r(&t, &s, NULL, 0);
        }
        pfCalculate_r(NULL, &s, &result, 1);
        printf("%s = " PF_NUM_FMT "\n", "expr", result);

        fclose(f);
    }

    return 0;
}
