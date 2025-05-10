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

void execute_command(char *const user_input, const size_t len) {

        push_history(user_input, len);

        if (strncmp("exit", user_input, 4) == 0)
                close_shell();

        else if (strncmp("cd", user_input, 2) == 0)
                cd(user_input);

        else if (strcmp("print_history", user_input) == 0)
                print_history();

        else
                system(user_input);
}
