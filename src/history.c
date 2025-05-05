#include "execute.h"
#include "history.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Contains the history of entered inputs.
static History history = {.values = NULL, .len = 0, .cap = 0};

void extend_history()
{
        const size_t new_cap = 10 + history.cap * 2;
        char **const new_values = malloc(new_cap * sizeof(char *));

        if (new_values == NULL)
        {
                fprintf(stderr, "Failed to malloc.\n");
                die = true;
        }

        for (int i = 0; i < history.len; ++i)
                stpcpy(new_values[i], history.values[i]);

        free_history();
        history.values = new_values;
        history.cap = new_cap;
}

void push_history(const char *const input, const size_t len)
{
        if (history.len == history.cap)
                extend_history();

        history.values[history.len] = malloc((len + 1) * sizeof(char));

        if (history.values[history.len] == NULL)
        {
                fprintf(stderr, "Failed to malloc.\n");
                die = true;
        }

        stpcpy(history.values[history.len], input);
        history.len++;
}

void free_history()
{
        for (int i = 0; i < history.len; ++i)
                free(history.values[i]);

        free(history.values);
}

char *get_history(const size_t index)
{
        if (index < history.len)
                return history.values[history.len - index - 1];

        return NULL;
}

void print_history()
{
        for (int i = 0; i < history.len; ++i)
                printf("- %s\n", history.values[i]);
}

void test_history()
{
        printf("Testing history...\n");

        assert(get_history(0) == NULL);
        assert(get_history(1) == NULL);
        assert(get_history(2) == NULL);

        const char one[] = "one";
        const char two[] = "two";
        const char three[] = "three";
        const char four[] = "four";
        const char five[] = "five";

        push_history(one, 3);
        assert(strcmp(one, get_history(0)) == 0);
        assert(get_history(1) == NULL);

        push_history(two, 3);
        push_history(three, 4);
        push_history(four, 4);
        assert(strcmp(one, get_history(0)) == 0);
        assert(strcmp(two, get_history(1)) == 0);
        assert(strcmp(three, get_history(2)) == 0);
        assert(strcmp(four, get_history(3)) == 0);
        assert(get_history(4) == NULL);
}

#ifdef TEST
int main() { test_history(); }
#endif
