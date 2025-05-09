#include "execute.h"
#include "history.h"
#include "keyboard.h"
#include "macros.h"
#include "path.h"
#include "shell.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CTRL_A 1
#define CTRL_D 4
#define CTRL_E 5
#define TAB 9    // aso ctrl+i
#define ENTER 10 // also ctrl+j
#define CTRL_K 11
#define CTRL_L 12
#define CTRL_U 21
#define ESC 27
#define BACKSPACE 127

#define UP 'A'
#define DOWN 'B'
#define LEFT 'D'
#define RIGHT 'C'

const size_t CURRENT_POSITION = -1UL;
static size_t history_pointer = CURRENT_POSITION;
static char current_line[MAX_LINE] = "";
static size_t current_len = 0;

static void handle_ctrl_arrow_press(char *const line, char **ptr) {
        char next;
        read_pressed_char(&next);
        log("1. Then %c!\n", next);
        if (next != ';')
                return;

        read_pressed_char(&next);
        log("2. Then %c!\n", next);
        if (next != '5')
                return;

        read_pressed_char(&next);
        log("3. Then %c!\n", next);

        switch (next) {

        case LEFT: {
                log("Ctrl+Left\n");
                if (*ptr == line)
                        return;
                --*ptr;
                while (*ptr != line && **ptr != ' ')
                        --*ptr;
                return;
        }

        case RIGHT: {
                log("Ctrl+Right\n");
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

static void handle_escape_press(char *const line, char **ptr, size_t *len) {
        log("Escaped!\n");

        char next;
        read_pressed_char(&next);

        log("a. Then %c!\n", next);

        if (next != '[') {
                if (next != 'd')
                        return;

                log("d!!\n");
                return;
        }

        read_pressed_char(&next);

        log("b. Then %c!\n", next);

        switch (next) {

        case '1':
                handle_ctrl_arrow_press(line, ptr);
                return;

        case UP: {
                if (history_pointer == CURRENT_POSITION) {
                        stpcpy(current_line, line);
                        current_len = *len;
                }

                const char *const history = get_history(history_pointer + 1);

                if (history == NULL)
                        return;

                stpcpy(line, history);
                *len = strlen(line);
                ++history_pointer;
                *ptr = line + *len;

                return;
        }

        case DOWN: {
                if (history_pointer == CURRENT_POSITION)
                        return;

                if (history_pointer == 0) {
                        --history_pointer;
                        stpcpy(line, current_line);
                        *len = current_len;
                        *ptr = line + *len;
                        return;
                }

                const char *const history_entry =
                    get_history(--history_pointer);
                assert(history_entry != NULL);
                char *end = stpcpy(line, history_entry);
                assert(*end == '\0');

                *len = strlen(line);
                *ptr = line + *len;

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
void handle_keypress(char *const line, char c, char **ptr, size_t *len,
                     const char *const ps1) {

        if (c == ESC) {
                handle_escape_press(line, ptr, len);
                return;
        };

        history_pointer = CURRENT_POSITION;

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

        case TAB: {
                const Executable *exec = find_one_with_prefix(line, *len);
                if (exec == NULL)
                        return;

                stpcpy(line, exec->name);
                *len = strlen(line);
                *ptr = line + *len;
                return;
        }

        case ENTER:
                printf("\r%s%s\n", ps1, line);
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
