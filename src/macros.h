#pragma once

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 512

#ifdef TEST
#define DIE exit(1);
#define log(...) printf(__VA_ARGS__);
#else
#include "main.h"
#define DIE close_shell();
#define log(...)                                                               \
        {                                                                      \
                fprintf(debug_file, __VA_ARGS__);                              \
                fflush(debug_file);                                            \
        }
#endif

#define panic(...)                                                             \
        {                                                                      \
                fprintf(stderr, __VA_ARGS__);                                  \
                DIE exit(9); /**unreachable*/                                  \
        }

#define assert(expr)                                                           \
        if (!(expr)) {                                                         \
                panic("Assertion %s failed at %s:%d (function %s).\n", #expr,  \
                      __FILE__, __LINE__, __func__);                           \
        }

#define assert_int(a, b)                                                       \
        if (a != b) {                                                          \
                panic("Assertion failed at %s:%d (function %s):\n%s (= %li) "  \
                      "!= %s (= %li).",                                        \
                      __FILE__, __LINE__, __func__, #a, a, #b, b);             \
        }

#define eprint(...) fprintf(stderr, __VA_ARGS__)

#define max(a, b) (a < b ? b : a)

#define default_case(x)                                                        \
        default:                                                               \
                panic("[%s:%d at %s]Unhandled case: %d.\n", __FILE__,          \
                      __LINE__, __func__, x);
