#pragma once

#include <stdbool.h>
#include <stddef.h>

#define NEW_STRING {.value = NULL, .len = 0, .cap = 0}

/// Inserts a char in a string.
///
/// This performs `*insert_position = c` without deleting the data under
/// `insert_position`. It thus shifts the whole string.
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

/// Extends a string with another string
void extend_string(String *dest, const char *const src, const size_t src_len);

/// Compares two strings by prefix.
///
/// This will check that the first character that differs from both strings is
/// the null character.
bool str_prefix_eq(const char *const s1, const char *const s2);
