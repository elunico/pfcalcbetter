
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *const argv[]) {
    int a = 10;
    int b[5] = {1, 2, 3, 4, 5};
    int *c = &a;

    printf("sizeof a: %lu\n", sizeof a);
    printf("sizeof b: %lu\n", sizeof b);
    printf("sizeof c: %lu\n", sizeof c);
    printf("sizeof int: %lu\n", sizeof(int));
    return 0;
}