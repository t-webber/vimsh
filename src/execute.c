#include "execute.h"
#include <stdlib.h>
#include <string.h>

void execute_command(char *const user_input)
{
        if (strncmp("exit", user_input, 4) == 0)
                exit(0);

        system(user_input);
        return;
}
