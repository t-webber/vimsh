#pragma once

/// Inserts a char in a string.
///
/// This performs `*insert_position = c` without deleting the data under
/// `insert_position`. It thus shifts the whole string.
#include <stddef.h>
void insert_char(char *const insert_position, char c);

/// Deletes a char in a string.
///
/// This deletes the char `*delete_position`.
void delete_char(char *const delete_position);

/// A string struct, made out of a growable vec.
typedef struct {
        char *value;
        size_t len;
        size_t cap;
} String;

/// Push a character into a string.
///
/// This function will grow the string with a realloc if necessary.
void push_string(String *str, char ch);
