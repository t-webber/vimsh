#pragma once

#include <stddef.h>
#include <stdio.h>

/// Handle keypresses and edits the line buffer in consequences.
///
/// Returns `true` if the program should exit, and `false` otherwise.
bool handle_keypress(FILE *debug_file, char *line, char c, char **ptr,
                     size_t *len);

/// Listens for the next char produced by a keypress.
void read_pressed_char(char *c);
