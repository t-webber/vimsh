#pragma once

#include <stdio.h>

/// File used to store debugging information (keypresses, states, etc.)
extern FILE *debug_file;

/// Main runner that displays the prompt, the written characters and handles the
/// keypresses.
void run_shell(void);

/// Clean up before closing the shell.
///
/// This means disabling raw mode and going back to initial terminal mode.
void close_shell(void);

/// Switch to raw mode to prevent defaults on keymappings and listen to them.
void open_shell(void);
