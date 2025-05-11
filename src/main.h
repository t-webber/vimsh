#pragma once

/// Closes the shell and cleans up opened files and dynamically allocated
/// memory.
#include <stdio.h>
void close_shell(void);

/// File in which all the logging is written.
///
/// Logs can't be printed to the standard outputs because they are already
/// heavily used by the program.
extern FILE *debug_file;

/// Enum that follows the vim mode currently in use.
typedef enum { NormalMode, InsertMode } VimMode;

/// Current @ref VimMode
extern VimMode vim_mode;
