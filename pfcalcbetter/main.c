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

#if pfnum_t == long
    return atol(s);
#elif pfnum_t == double;
    return atof(s);
#endif
}

pfnum_t pfCalculate(struct token *tokens) {
    struct stack *s = NULL;
    int stepCount = 0;
    while (tokens != NULL) {
        char *tok = tokens->value;
        if (isdigit(tok[0])) {
            pfnum_t v = atol(tok);
            stack_push(&s, v);
        } else if (ispunct(tok[0])) {
            pfnum_t rhs = stack_pop(&s);
            if (rhs == MISSING_SENTINEL) {
                assertionFailure("Too many operators");
            }
            pfnum_t lhs = stack_pop(&s);
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
            } else {
                assertionFailure("invalid operator");
            }

            debug("Step %d: %lf %s %lf = %lf\n", ++stepCount, lhs, tok, rhs, result);
            stack_push(&s, result);
        }
        tokens = tokens->next;
    }

    pfnum_t result = stack_pop(&s);

    if (stack_pop(&s) != MISSING_SENTINEL) {
        assertionFailure("Not enough operators");
    }

    return result;
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
    } else {
        FILE *f = fopen(args.filename, "r");
        root = ftokenize(f);
        fclose(f);
    }

    pfnum_t result = pfCalculate(root);
    printf("%s = %lf\n", "expr", result);
    free_tokens(root);

    return 0;
}
