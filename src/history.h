#pragma once

#include <stddef.h>

/// Stores the history of entered inputs.
typedef struct
{
        char **values;
        size_t len;
        size_t cap;
} History;

/// Adds the submitted line to the command history.
///
/// This function mallocs and copies the input, so no need to copy it. Freeing
/// the given data is thus the responsability of the caller.
void push_history(const char *const input, const size_t len);

/// Frees the memory consumed by the @ref History struct.
void free_history();

/// Gets the `position`th element in history.
///
/// `0` is the oldest entry, `len-1` is the newest.
char *get_history(const size_t index);

/// Print the whole content of history.
void print_history();
