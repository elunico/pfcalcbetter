//
//  args.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/30/24.
//

#ifndef args_h
#define args_h

#include <unistd.h>

struct arguments {
    int isStdin;
    char *filename;
};

struct arguments parseargs(int argc, char *const argv[]);

#endif /* args_h */
