#include "execute.h"
#include "history.h"
#include "macros.h"
#include "main.h"
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

#define execute_alias(alias, expanded)                                         \
        {                                                                      \
                const size_t alias_len = sizeof(alias);                        \
                const size_t expanded_len = sizeof(expanded);                  \
                if (strncmp(alias, user_input, alias_len - 1) == 0) {          \
                        const size_t cap =                                     \
                            corrected_len - alias_len + expanded_len + 1;      \
                        char *command = malloc(cap);                           \
                        char *end = stpcpy(command, expanded);                 \
                        if (len > alias_len)                                   \
                                stpcpy(end, user_input + alias_len);           \
                        system(command);                                       \
                        free(command);                                         \
                        return;                                                \
                }                                                              \
        }

void execute_command(char *const user_input, const size_t len) {

        if (len == 0)
                return;

        push_history(user_input, len);

        if (strncmp("exit", user_input, 4) == 0)
                close_shell();

        else if (strncmp("cd", user_input, 2) == 0)
                cd(user_input);

        else if (strcmp("print_history", user_input) == 0)
                print_history();

        else {
                size_t corrected_len = len;
                if (*(user_input + len - 1) != ' ') {
                        *(user_input + len) = ' ';
                        *(user_input + len + 1) = '\0';
                        ++corrected_len;
                }

                assert(*(user_input + corrected_len) == '\0');

                execute_alias("l ", "eza -a ");
                execute_alias("bp ",
                              "cat /sys/class/power_supply/BAT1/capacity");
                execute_alias("gd ", "git diff ");
                execute_alias("vsh ", "$DEV/vsh/main.out");

                system(user_input);
        }
}
