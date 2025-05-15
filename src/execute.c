#include "execute.h"
#include "history.h"
#include "macros.h"
#include "main.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *home_path = NULL;

void initialise_home(void) {
        if (home_path == NULL) {
                home_path = getenv("HOME");
        }
}

static void safe_chdir(const char *const path) {
        int x = chdir(path);
        if (x == 0)
                return;
        else if (x == -1)
                eprint("cd: %s: No such file or directory.\n", path);
        else
                panic("Failed to cd to %s with error %d", path, x);
}

static void cd(char *const user_input) {
        bool has_path = false;

        for (const char *reader = user_input + 2; *reader != '\0'; reader++)
                if (*reader != ' ')
                        has_path = true;

        if (!has_path) {
                safe_chdir(home_path);
                return;
        }

        // TODO: ~, prefix and infix
        safe_chdir(user_input + 3);
}

typedef struct {
        const char *const alias;
        const char *const expanded;
        const size_t len;
} Alias;

#define alias(alias, expanded) {alias, expanded, sizeof(expanded) - 1}

const Alias ALIASES[] = {
    alias("l", "eza -a "),
    alias("bp", "cat /sys/class/power_supply/BAT1/capacity "),
    alias("gd", "git diff "),
    alias("vsh", "$DEV/vsh/main.out "),
    alias("gls", "git ls-files "),
    alias("xcnt", "xargs wc -l "),
    alias("cnt", "wc -l "),
};

const size_t ALIASES_LEN = 7;

static bool try_extend_with_alias(const char *const start, String *expanded) {
        for (size_t i = 0; i < ALIASES_LEN; ++i)
                if (strcmp(start, ALIASES[i].alias) == 0) {
                        extend_string(expanded, ALIASES[i].expanded,
                                      ALIASES[i].len);
                        return true;
                }
        return false;
}

static String expand_aliases(char *const user_input) {
        String expanded = NEW_STRING;
        for (char *start = user_input, *end = user_input;;) {
                if (*end != ' ' && *end != '\0') {
                        ++end;
                        continue;
                }

                bool must_break = *end == '\0';
                *end = '\0';
                bool found = try_extend_with_alias(start, &expanded);
                if (!found) {
                        *end++ = ' ';

                        char old = '\0';
                        if (!must_break)
                                old = *end;

                        *end = '\0';
                        extend_string(&expanded, start, (size_t)(end - start));

                        if (!must_break)
                                *end-- = old;
                }

                if (must_break)
                        return expanded;

                *end++ = ' ';

                while (*end == ' ')
                        ++end;

                if (*end == '\0')
                        return expanded;

                start = end;
        }

        panic("Unknown error occurred.\n")
}

void execute_command(char *const user_input, const size_t len) {
        char *trimmed_input = user_input;
        size_t trimmed_len = len;

        while (*trimmed_input == ' ')
                ++trimmed_input, --trimmed_len;

        if (trimmed_len == 0)
                return;

        char *end = trimmed_input + trimmed_len - 1;
        while (*end == ' ')
                --end, --trimmed_len;
        *(end + 1) = '\0';
        assert(trimmed_input + trimmed_len - 1 == end);

        push_history(trimmed_input, trimmed_len);

        if (strncmp("exit", trimmed_input, 4) == 0)
                close_shell();

        else if (strncmp("cd", trimmed_input, 2) == 0)
                cd(user_input);

        else if (strcmp("print_history", trimmed_input) == 0)
                print_history();

        else {
                String command = expand_aliases(trimmed_input);
                system(command.value);
                free(command.value);
        }
}
