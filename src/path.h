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

/// Compute the list of executables.
///
/// See @ref ExecutableList
ExecutableList get_executables(void);

/// Find the path of an executable with only its name.
char *find_executable(const ExecutableList *const list,
                      const char *const program);

/// Free the memory consumed by the @get_executables function.
void free_executables(ExecutableList *list);
