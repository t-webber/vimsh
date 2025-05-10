#pragma once

#include <stddef.h>

/// Get the PS1 to display at the begining of every line.
void get_ps1(char *ps1);

/// Computes the real length of PS1, without the colour escape codes.
size_t ps1_len(const char *const ps1);
