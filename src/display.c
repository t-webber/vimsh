#include "display.h"
#include <stdio.h>

/// Clears the current line.
///
/// Thanks to this function, old characters aren't left on the screen (e.g.
/// after pressing <Del>).
void clear_line(const size_t len) {
        printf("\r");
        for (size_t i = 0; i < len; ++i)
                printf(" ");
        printf("\r");
        fflush(stdout);
}
