//
//  args.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/30/24.
//

#ifndef args_h
#define args_h

#include <unistd.h>

struct arguments;

/// Implementation note: caller must free return value
struct arguments *arguments_parse(int argc, char *const argv[]);
char *arguments_getfilename(struct arguments *a);
int arguments_wasset(struct arguments *a);
void arguments_free(struct arguments *args);

#endif /* args_h */
