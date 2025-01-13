//
//  Header.h
//  pfcalcbetter
//
//  Created by Thomas Povinelli on 12/29/24.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdlib.h>

#if defined(PRINT) && (PRINT & 1)
#define critical(fmt, ...) fprintf(stderr, "\033[91m\033[1mCRITICAL: [" __FILE__ ":" STR(__LINE__) "]" fmt "\033[0m", ##__VA_ARGS__)
#else
#define critical(...) (void)0
#endif

#if defined(PRINT) && (PRINT & 2)
#define error(fmt, ...) fprintf(stderr, "\033[91m[" __FILE__ ":" STR(__LINE__) "]" fmt "\033[0m", ##__VA_ARGS__)
#else
#define error(...) (void)0
#endif

#if defined(PRINT) && (PRINT & 4)
#define warning(fmt, ...) fprintf(stderr, "\033[93m[" __FILE__ ":" STR(__LINE__) "]" fmt "\033[0m", ##__VA_ARGS__)
#else
#define warning(...) (void)0
#endif

#if defined(PRINT) && (PRINT & 8)
#define info(fmt, ...) fprintf(stderr, "\033[92m[" __FILE__ ":" STR(__LINE__) "]" fmt "\033[0m", ##__VA_ARGS__)
#else
#define info(...) (void)0
#endif

#if defined(PRINT) && (PRINT & 16)
#define debug(fmt, ...) fprintf(stderr, "\033[94m[" __FILE__ ":" STR(__LINE__) "]" fmt "\033[0m", ##__VA_ARGS__)
#else
#define debug(...) (void)0
#endif


#define throw_halt(n) exit(n)

#define fatal(msg)                                                              \
    do {                                                                       \
        fputs("[" __FILE__ ":" STR(__LINE__) "]" msg, stderr);                 \
        throw_halt(2);                                                         \
    } while (0)

#define fatalf(fmt, ...)                                                        \
    do {                                                                       \
        fprintf(stderr, "[" __FILE__ ":" STR(__LINE__) "]" fmt, __VA_ARGS__);  \
        throw_halt(2);                                                         \
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
#define require(condition, ...)                                            \
    if (!(condition)) {                                                        \
        fatal("requirement not satisfied: " #condition __VA_ARGS__);                \
    }
#else
#define require(condition, message) (void)0;
#endif

#endif /* utils_h */
