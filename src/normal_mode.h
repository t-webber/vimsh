#pragma once

/// Handles alphanumeric keypresses in normal mode.
void handle_normal_mode(const char c, char **ptr, char *const line);

/// Free allocate memory that holds the vim commands.
void free_vim_command(void);
