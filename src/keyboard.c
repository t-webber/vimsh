#include "execute.h"
#include "keyboard.h"
#include "shell.h"
#include "str.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CTRL_A 1
#define CTRL_D 4
#define CTRL_E 5
#define ENTER 10 // also ctrl+
#define ESC 27
#define BACKSPACE 127

void handle_escape_press(FILE *debug_file, char *const line, char c, char **ptr,
                         size_t *len)
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

void handle_keypress(FILE *debug_file, char *const line, char c, char **ptr,
                     size_t *len)
{
        switch (c)
        {

        case CTRL_A:
        {
                *ptr = line;
                return;
        }

        case CTRL_D:
                close_shell();

        case CTRL_E:
        {
                *ptr = line + *len;
                return;
        }

        case ESC:
                return handle_escape_press(debug_file, line, c, ptr, len);

        case ENTER:
        {
                printf("\r$ %s\n", line);
                assert(strlen(line) == *len);
                execute_command(line, *len);
                *len = 0;
                *ptr = line;
                **ptr = '\0';
                return;
        }

        case BACKSPACE:
        {
                if (*ptr != line)
                        --*ptr, --*len;
                return;
        }

        default:
        {
                ++*len;
                insert_char(*ptr, c);
                ++*ptr;
                return;
        }
        }
}

void read_pressed_char(char *c) { read(STDIN_FILENO, c, 1); }
