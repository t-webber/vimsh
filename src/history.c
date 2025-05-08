#include "history.h"
#include "macros.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Contains the history of entered inputs.
static History history = {.inputs = NULL, .len = 0, .cap = 0};

static void extend_history(void) {
        const size_t new_cap = 1 + history.cap * 2;
        HistoryInput *const new_inputs = malloc(new_cap * sizeof(HistoryInput));

        assert(new_inputs != NULL);

        for (size_t i = 0; i < history.len; ++i) {
                const HistoryInput input = history.inputs[i];
                char *const new_value = malloc(input.len * sizeof(char));
                stpcpy(new_value, input.value);
                const HistoryInput new_input = {.value = new_value,
                                                .len = input.len};
                new_inputs[i] = new_input;
        }

        free_history();
        history.inputs = new_inputs;
        history.cap = new_cap;
}

void free_history(void) {
        for (size_t i = 0; i < history.len; ++i)
                free(history.inputs[i].value);

        free(history.inputs);
}

void push_history(const char *const input, const size_t len) {
        if (history.len == history.cap)
                extend_history();

        assert(len == strlen(input));
        assert(history.inputs[history.len].value == NULL);
        assert(history.inputs[history.len].len == 0);

        history.inputs[history.len].value = malloc((len + 1) * sizeof(char));
        history.inputs[history.len].len = len;

        assert(history.inputs[history.len].value != NULL);

        stpcpy(history.inputs[history.len].value, input);

        history.len++;
}

char *get_history(const size_t index) {
        if (index < history.len)
                return history.inputs[history.len - index - 1].value;

        return NULL;
}

void print_history(void) {
        for (size_t i = 0; i < history.len; ++i)
                printf("- %s\n", history.inputs[i].value);
}

#ifdef TEST
static void test_history(void) {
        assert(get_history(0) == NULL);
        assert(get_history(1) == NULL);
        assert(get_history(2) == NULL);

        const char one[] = "one";
        const char two[] = "two";
        const char three[] = "three";
        const char four[] = "four";

        push_history(one, 3);
        assert(strcmp(one, get_history(0)) == 0);
        assert(get_history(1) == NULL);

        push_history(two, 3);
        push_history(three, 5);
        push_history(four, 4);
        assert(strcmp(four, get_history(0)) == 0);
        assert(strcmp(three, get_history(1)) == 0);
        assert(strcmp(two, get_history(2)) == 0);
        assert(strcmp(one, get_history(3)) == 0);
        assert(get_history(4) == NULL);
}

int main() { test_history(); }
#endif
