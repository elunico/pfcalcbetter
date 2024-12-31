//
//  Header.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/29/24.
//

#ifndef utils_h
#define utils_h

#define fail(msg)                                                  \
    failf("%s\n", msg)

#define failf(fmt, ...)                                            \
    do {                                                                       \
        fprintf(stderr, fmt, __VA_ARGS__);                                     \
        exit(1);                                                               \
    } while (0)

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

#ifndef NDEBUG
#define debug(fmt, ...)                                                        \
    printf("[" __FILE__ ":" STR(__LINE__) "] " fmt, __VA_ARGS__)
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
