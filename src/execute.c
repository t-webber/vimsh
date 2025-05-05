#include "execute.h"
#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool die = false;

void cd(char *const user_input)
{
        bool has_path = false;

        for (const char *reader = user_input + 2; *reader != '\0'; reader++)
                if (*reader != ' ')
                        has_path = true;

        if (has_path)
        {
                chdir(user_input + 3);
                return;
        }

        chdir(getenv("HOME"));
}

void execute_command(char *const user_input, const size_t len)
{

        push_history(user_input, len);

        if (strncmp("exit", user_input, 4) == 0)
                die = true;

        else if (strncmp("cd", user_input, 2) == 0)
                cd(user_input);

        else
                system(user_input);
}
