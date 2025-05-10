#pragma once

#include <stdbool.h>

/// Executes a command entered in the input
///
/// Returns `true` if the program must exit, `false` else.
bool execute_command(char *const user_input);
