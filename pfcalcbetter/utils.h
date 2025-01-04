//
//  Header.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/29/24.
//

#ifndef utils_h
#define utils_h

#ifndef NDEBUG
#define debug(fmt, ...)                                                        \
    printf("[" __FILE__ ":" STR(__LINE__) "] " fmt, __VA_ARGS__)

#define throw_halt(n) abort()
#else
#define debug(...) (void)0
#define throw_halt(n) exit(n)
#endif


#define fail(msg)                                                              \
    do {                                                                       \
        fputs("[" __FILE__ ":" STR(__LINE__) "] \n" msg, stderr);                \
        throw_halt(2);                                                          \
    } while (0)

#define failf(fmt, ...)                                                        \
    do {                                                                       \
        fprintf(stderr, "[" __FILE__ ":" STR(__LINE__) "] \n" fmt, __VA_ARGS__); \
        throw_halt(2);                                                          \
    } while (0)

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)



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

#ifdef __DARWIN_C_LEVEL
#define readline(declvar, stream, len) declvar = fgetln(stream, &len)
#else
#define readline(declvar, stream, len) len = getline(&declvar, &len, stream)
#endif

#ifndef REQUIRE_NO_CHECKS
#define require(condition, message)                                            \
    if (!(condition)) {                                                        \
        fail("requirement not satisfied: " #condition message);                \
    }
#else
#define require(condition, message) (void)0;
#endif

#endif /* utils_h */
