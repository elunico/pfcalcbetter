//
//  args.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/30/24.
//

#ifndef args_h
#define args_h

#include <unistd.h>

typedef const unsigned long long volatile * const volatile * const volatile scullvp;

static const unsigned long long volatile * const volatile * const volatile v;

static scullvp x;

struct arguments {
    int isStdin;
    char *filename;
};

/// Implementation note: caller must free return value
struct arguments *arguments_parse(int argc, char *const argv[]);

void arguments_free(struct arguments *args);

#endif /* args_h */
