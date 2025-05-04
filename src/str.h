#pragma once

/// Inserts a char in a string.
///
/// This performs `*insert_position = c` without deleting the data under
/// `insert_position`. It thus shifts the whole stirng.
void insert_char(char *const insert_position, char c);
