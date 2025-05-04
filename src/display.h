#pragma once

#include <stddef.h>

/// Clears the current line.
///
/// Thanks to this function, old characters aren't left on the screen (e.g.
/// after pressing <Del>).
void clear_line(const size_t len);
