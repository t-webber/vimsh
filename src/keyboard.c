#include "display.h"
#include "execute.h"
#include "keyboard.h"
#include "str.h"
#include <stdio.h>
#include <unistd.h>

void handle_keypress(FILE *debug_file, char *const line, char c, char **ptr,
                     size_t *len)
{

        // Escape character. Used to encode arrows.
        if (c == 27)
        {
                fprintf(debug_file, "Escaped!\n");

                char next;
                read_pressed_char(&next);

                fprintf(debug_file, "Then %c!\n", next);

                if (next != '[')
                        return;

                read_pressed_char(&next);

                fprintf(debug_file, "Then %c!\n", next);
                fflush(stdout);

                if (next == 'D')
                        if (*ptr != line)
                                --*ptr;

                if (next == 'C')
                        if (**ptr != '\0')
                                ++*ptr;

                return;
        }

        if (c == 10)
        {
                printf("\r$ %s\n", line);
                execute_command(line);
                *len = 0;
                *ptr = line;
                **ptr = '\0';
                return;
        }

        /// <Del> key.
        if (c == 127)
        {
                if (*ptr != line)
                        --*ptr, --*len;
                return;
        }

        ++*len;
        insert_char(*ptr, c);
        ++*ptr;
}

void read_pressed_char(char *c) { read(STDIN_FILENO, c, 1); }
