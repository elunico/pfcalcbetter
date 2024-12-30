//
//  Header.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/29/24.
//

#ifndef utils_h
#define utils_h

#define assertionFailure(msg)                                                  \
    do {                                                                       \
        fprintf(stderr, msg);                                                  \
        exit(1);                                                               \
    } while (0)

#ifndef NDEBUG
#define debug(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define debug(...) (void)0
#endif

typedef double pfnum_t;

#endif /* utils_h */
