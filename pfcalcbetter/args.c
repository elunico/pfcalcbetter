//
//  args.c
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/30/24.
//

#include "args.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct arguments *arguments_parse(int argc, char *const argv[]) {
    struct arguments *args = calloc(1, sizeof(struct arguments));
    int c;
    int didSet = 0;
    while ((c = getopt(argc, argv, "if:")) != -1) {
        int this_option_optind = optind ? optind : 1;
        switch (c) {
        case 'i': {
            if (didSet) {
                fail("Choose either -i for -f FILE not both");
            }
            args->isStdin = 1;
            args->filename = NULL;
            didSet = 1;
            break;
        }
        case 'f': {
            if (didSet) {
                fail("Choose either -i for -f FILE not both");
            }
            args->isStdin = 0;
            args->filename = strdup(optarg);
            didSet = 1;
            break;
        }
        case '?':
        default:
                failf("?? getopt returned character code 0%o ??\n", c);
        }
    }
    if (optind < argc) {
        printf("Unrecognized arguments: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        fail("\n");
    }
    return args;
}

void arguments_free(struct arguments *args) {
    free(args->filename);
    free(args);
}
