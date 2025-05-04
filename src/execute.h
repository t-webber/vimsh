#pragma once

#include <stdbool.h>

/// Informs on whether the shell has to be killed or not.
extern bool die;

/// Executes a command entered in the input.
void execute_command(char *const user_input);
