#pragma once

#ifdef TEST
#include <stdlib.h>
#define DIE exit(1);
#else
#include "shell.h"
#define DIE close_shell();
#endif

#define panic(...)                                                             \
        {                                                                      \
                fprintf(stderr, __VA_ARGS__);                                  \
                DIE                                                            \
        }

#define assert(expr)                                                           \
        if (!(expr)) {                                                         \
                panic("Assert %s failed at %s:%d (function %s)", #expr,        \
                      __FILE__, __LINE__, __func__);                           \
        }
