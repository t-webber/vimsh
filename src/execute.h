#pragma once

#include <stdbool.h>
#include <stddef.h>

/// Informs on whether the shell has to be killed or not.
extern bool die;

/// Executes a command entered in the input.
void execute_command(char *const user_input, const size_t len);

/// Initialises the HOME variable to replace ~ later
void initialise_home(void);

/// Free the memory taken by @ref initialise_home.
void free_home(void);
