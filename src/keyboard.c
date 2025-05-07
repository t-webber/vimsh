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

static size_t history_pointer = -1UL;
static char current_line[] = "";
static size_t current_len = 0;

static void handle_ctrl_arrow_press(FILE *debug_file, char *const line,
                                    char **ptr) {
        char next;
        read_pressed_char(&next);
        fprintf(debug_file, "1. Then %c!\n", next);
        if (next != ';')
                return;

        read_pressed_char(&next);
        fprintf(debug_file, "2. Then %c!\n", next);
        if (next != '5')
                return;

        read_pressed_char(&next);
        fprintf(debug_file, "3. Then %c!\n", next);

        switch (next) {

        case LEFT: {

                fprintf(debug_file, "Ctrl+Left\n");
                if (*ptr == line)
                        return;
                --*ptr;
                while (*ptr != line && **ptr != ' ')
                        --*ptr;
                return;
        }

        case RIGHT: {
                fprintf(debug_file, "Ctrl+Right\n");
                if (**ptr == '\0')
                        return;
                ++*ptr;
                while (**ptr != '\0' && **ptr != ' ')
                        ++*ptr;
                return;
        }

        default:
                return;
        }
}

static void handle_escape_press(FILE *debug_file, char *const line, char **ptr,
                                size_t *len) {
        fprintf(debug_file, "Escaped!\n");

        char next;
        read_pressed_char(&next);

        fprintf(debug_file, "a. Then %c!\n", next);

        if (next != '[')
                return;

        read_pressed_char(&next);

        fprintf(debug_file, "b. Then %c!\n", next);
        fflush(stdout);

        switch (next) {

        case '1':
                handle_ctrl_arrow_press(debug_file, line, ptr);
                return;

        case UP: {
                if (history_pointer == -1UL) {
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

        case DOWN: {
                if (history_pointer == -1UL)
                        return;

                if (history_pointer == 0) {
                        --history_pointer;
                        stpcpy(line, current_line);
                        *len = current_len;
                        *ptr = line;
                        return;
                }

                const char *const history_entry =
                    get_history(--history_pointer);
                assert(history_entry != NULL);
                char *end = stpcpy(line, history_entry);
                assert(*end == '\0');

                *len = strlen(line);
                *ptr = line;

                return;
        }

        case LEFT: {
                if (*ptr != line)
                        --*ptr;
                return;
        }

        case RIGHT: {
                if (**ptr != '\0')
                        ++*ptr;
                return;
        }

        default:
                return;
        }
}
void handle_keypress(FILE *debug_file, char *const line, char c, char **ptr,
                     size_t *len) {

        if (c == ESC) {
                handle_escape_press(debug_file, line, ptr, len);
                return;
        };

        history_pointer = -1UL;

        switch (c) {

        case CTRL_A:
                *ptr = line;
                return;

        case CTRL_D:
                close_shell();
                return;

        case CTRL_E:
                *ptr = line + *len;
                return;

        case CTRL_K:
                **ptr = '\0';
                *len = (size_t)(*ptr - line);
                break;

        case CTRL_L:
                printf("\033[H\033[J");
                return;

        case CTRL_U: {
                char *end = stpcpy(line, *ptr);
                *len = (size_t)(end - line);
                return;
        }

        case ENTER:
                printf("\r$ %s\n", line);
                assert(strlen(line) == *len);
                execute_command(line, *len);
                *len = 0;
                *ptr = line;
                **ptr = '\0';
                return;

        case BACKSPACE:
                if (*ptr != line)
                        --*ptr, --*len;
                return;

        default:
                ++*len;
                insert_char(*ptr, c);
                ++*ptr;
                return;
        }
}

void read_pressed_char(char *c) { read(STDIN_FILENO, c, 1); }
