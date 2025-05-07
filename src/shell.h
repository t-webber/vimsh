#pragma once

/// Main runner that displays the prompt, the written characters and handles the
/// keypresses.
void run_shell(void);

/// Clean up before closing the shell.
///
/// This means disabling raw mode and going back to initial terminal mode.
void close_shell(void);

/// Handler for `SIGINT`, i.e., for the Ctrl+c combination key.
void ctrl_c_handler(int x);

/// Switch to raw mode to prevent defaults on keymappings and listen to them.
void open_shell(void);
