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

struct arguments parseargs(int argc, char *const argv[]) {
    struct arguments args = {0};
    int c;
    int didSet = 0;
    while ((c = getopt(argc, argv, "if:")) != -1) {
        int this_option_optind = optind ? optind : 1;
        switch (c) {
        case 'i': {
            if (didSet) {
                assertionFailure("Choose either -i for -f FILE not both");
            }
            args.isStdin = 1;
            args.filename = NULL;
            didSet = 1;
            break;
        }
        case 'f': {
            if (didSet) {
                assertionFailure("Choose either -i for -f FILE not both");
            }
            args.isStdin = 0;
            args.filename = optarg;
            didSet = 1;
            break;
        }
        case '?':
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
    if (optind < argc) {
        printf("Unrecognized arguments: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        assertionFailure("\n");
    }
    return args;
}
