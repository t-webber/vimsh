#include "macros.h"
#include "str.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void delete_char(char *const delete_position) {
        for (char *reader = delete_position; *reader != '\0'; ++reader)
                *reader = *(reader + 1);
}

void insert_char(char *const insert_position, char c) {
        char next = *insert_position, current = 1, *shifter = insert_position;

        while (current != '\0') {
                current = next;
                next = *++shifter;
                *shifter = current;
        }

        *insert_position = c;
}

void push_string(String *str, char ch) {
        if (str->cap <= str->len + 1) {
                size_t new_cap = 10 + str->cap * 2;
                char *new_value = malloc(new_cap * sizeof(char));
                if (str->value != NULL) {
                        stpcpy(new_value, str->value);
                        free(str->value);
                }
                str->value = new_value;
                str->cap = new_cap;
        }

        str->value[str->len++] = ch;
        str->value[str->len] = '\0';
}

bool str_prefix_eq(const char *const s1, const char *const s2) {
        for (const char *ptr1 = s1, *ptr2 = s2; *ptr1 != '\0' && *ptr2 != '\0';
             ++ptr1, ++ptr2)
                if (*ptr1 != *ptr2)
                        return false;
        return true;
}

#ifdef TEST
#include <stdio.h>
#include <string.h>

/// Tests for the @ref str_prefix_eq function
static void test_prefix(void) {
        assert(!str_prefix_eq("pi8o", "CTM"));
        assert(str_prefix_eq("cjy9", "cj"));
        assert(!str_prefix_eq("0429", "kBbz"));
        assert(str_prefix_eq("", "MxJd"));
        assert(!str_prefix_eq("E7eI", "kknC"));
        assert(str_prefix_eq("GtKH", "GtKH"));
        assert(str_prefix_eq("GtKH ", "GtKH"));
        assert(!str_prefix_eq("UAnM", "P1rM"));
}

/// Tests the logic on the @ref String struct.
static void test_grow_string(void) {
        String s = {.cap = 0, .len = 0, .value = NULL};

        push_string(&s, 'H');
        push_string(&s, 'e');
        push_string(&s, 'l');
        push_string(&s, 'l');
        push_string(&s, 'o');
        push_string(&s, ',');
        push_string(&s, ' ');
        push_string(&s, 'w');
        push_string(&s, 'o');
        push_string(&s, 'r');
        push_string(&s, 'l');
        push_string(&s, 'd');
        push_string(&s, '!');

        const char *const helloworld = "Hello, world!";
        assert(s.len == strlen(helloworld));
        assert(s.cap == 30);
        assert(strcmp(s.value, helloworld) == 0);

        free(s.value);
}

/// Tests the @ref delete_char function
static void test_delete_char(void) {
        char string[] = "#Hello, world!";
        delete_char(string);
        delete_char(string + 5);
        delete_char(string + 11);

        assert(strcmp("Hello world", string) == 0);

        char empty[] = "1";
        delete_char(empty);
        assert(empty[0] == '\0');
}

/// Tests the @ref insert_char function.
static void test_insert_char(void) {
        char string[20] = "Hello world";
        insert_char(string + 5, ',');
        insert_char(string + 6, '-');
        insert_char(string, '#');
        insert_char(string + 14, '!');

        assert(strcmp("#Hello,- world!", string) == 0);

        char empty[20] = "";
        insert_char(empty, '!');
        assert(strcmp(empty, "!") == 0);
}

int main() {
        test_prefix();
        test_delete_char();
        test_grow_string();
        test_insert_char();
}
#endif
