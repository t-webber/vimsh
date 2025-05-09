#pragma once

#include <stddef.h>

/// Reprents an executable file, with its location.
typedef struct {
        char *name;
        char *path;
} Executable;

/// Stores the list of accessible executables.
///
/// This is the list accessible through the PATH variable.
typedef struct {
        Executable *values;
        size_t len;
        size_t cap;
} ExecutableList;

/// Free the memory consumed by the @get_executables function.
void free_executables(ExecutableList *list);

/// Find the first executable acessible whose name begins with the given prefix.
Executable *find_one_with_prefix(const char *const prefix, const size_t len);

/// Compute all the accessible executables through PATH
void initialise_executables(void);
