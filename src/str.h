#pragma once

/// Inserts a char in a string.
///
/// This performs `*insert_position = c` without deleting the data under
/// `insert_position`. It thus shifts the whole string.
void insert_char(char *const insert_position, char c);

/// Deletes a char in a string.
///
/// This deletes the char `*delete_position`.
void delete_char(char *const delete_position);
