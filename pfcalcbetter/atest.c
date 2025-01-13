
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

int main(int argc, char *const argv[]) {
    int a = 10;
    int b[5] = {1, 2, 3, 4, 5};
    int *c = &a;

    printf("sizeof a: %lu\n", sizeof a);
    printf("sizeof b: %lu\n", sizeof b);
    printf("sizeof c: %lu\n", sizeof c);
    printf("sizeof int: %lu\n", sizeof(int));

    debug("This is a debug message\n");
    info("This is an info message\n");
    warning("This is a warning message\n");
    error("This is an error message\n");
    critical("This is a critical message\n");
    return 0;
}