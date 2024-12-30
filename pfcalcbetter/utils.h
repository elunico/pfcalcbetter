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

#if defined(PF_NUM_LONG) && defined(PF_NUM_DOUBLE)
#error "Conflicting types for pfnum_t"
#elif defined(PF_NUM_DOUBLE)
typedef double pfnum_t;
#define PF_NUM_FMT "%lf"
#elif defined(PF_NUM_LONG)
typedef long pfnum_t;
#define PF_NUM_FMT "%ld"
#else
#warning "no explicit type defined for pfnum_t. default is double"
typedef double pfnum_t;
#define PF_NUM_FMT "%lf"
#endif


#endif /* utils_h */
