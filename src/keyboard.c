#include "execute.h"
#include "history.h"
#include "keyboard.h"
#include "shell.h"
#include "str.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CTRL_A 1
#define CTRL_D 4
#define CTRL_E 5
#define ENTER 10 // also ctrl+
#define CTRL_K 11
#define CTRL_L 12
#define CTRL_U 21
#define ESC 27
#define BACKSPACE 127

#define UP 'A'
#define DOWN 'B'
#define LEFT 'D'
#define RIGHT 'C'

static size_t history_pointer = -1;
static char current_line[] = "";
static size_t current_len = 0;

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

        switch (next)
        {

        case UP:
        {
                if (history_pointer == -1)
                {
                        stpcpy(current_line, line);
                        current_len = *len;
                }

                const char *const history = get_history(history_pointer + 1);

                if (history == NULL)
                        return;

                stpcpy(line, history);
                *len = strlen(line);
                ++history_pointer;
                *ptr = line;

                return;
        }

        case DOWN:
        {
                if (history_pointer == -1)
                        return;

                if (history_pointer == 0)
                {
                        --history_pointer;
                        stpcpy(line, current_line);
                        *len = current_len;
                        *ptr = line;
                        return;
                }

                const char *const history = get_history(--history_pointer);
                assert(history != NULL);
                char *end = stpcpy(line, history);
                assert(*end == '\0');
                *len = strlen(line);
                *ptr = line;

                return;
        }

        case LEFT:
        {
                if (*ptr != line)
                        --*ptr;
                return;
        }

        case RIGHT:
        {
                if (**ptr != '\0')
                        ++*ptr;
                return;
        }
        }
}
void handle_keypress(FILE *debug_file, char *const line, char c, char **ptr,
                     size_t *len)
{

        if (c == ESC)
                return handle_escape_press(debug_file, line, c, ptr, len);
        history_pointer = -1;

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

        case CTRL_K:
        {
                **ptr = '\0';
                *len = *ptr - line;
                break;
        }

        case CTRL_L:
                return (void)printf("\033[H\033[J");

        case CTRL_U:
        {
                char *end = stpcpy(line, *ptr);
                *len = end - line;
                return;
        }

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
