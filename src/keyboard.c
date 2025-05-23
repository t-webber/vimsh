#include "execute.h"
#include "history.h"
#include "keyboard.h"
#include "macros.h"
#include "main.h"
#include "normal_mode.h"
#include "path.h"
#include "str.h"
#include <dirent.h>
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

typedef enum { None, Char, CharAndSpace } ContainsSpace;
static ContainsSpace contains_space = None;

static void update_state(char c) {
        if (contains_space == Char && c == ' ')
                contains_space = CharAndSpace;

        else if (contains_space == None && c != ' ')
                contains_space = Char;
}

static void handle_ctrl_arrow_press(char *const line, char **ptr) {
        char next;
        read_pressed_char(&next);
        if (next != ';')
                return;

        read_pressed_char(&next);
        if (next != '5')
                return;

        read_pressed_char(&next);

        switch (next) {

        case LEFT: {
                if (*ptr == line)
                        return;
                --*ptr;
                while (*ptr != line && **ptr != ' ')
                        --*ptr;
                return;
        }

        case RIGHT: {
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

static void history_load_previous(char *const line, char **ptr, size_t *len) {
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
}

static void history_load_next(char *const line, char **ptr, size_t *len) {
        if (history_pointer == CURRENT_POSITION)
                return;

        if (history_pointer == 0) {
                --history_pointer;
                stpcpy(line, current_line);
                *len = current_len;
                *ptr = line + *len;
                return;
        }

        const char *const history_entry = get_history(--history_pointer);
        assert(history_entry != NULL);
        char *end = stpcpy(line, history_entry);
        assert(*end == '\0');

        *len = strlen(line);
        *ptr = line + *len;
}

static void handle_suppr(char *const line, char **ptr, size_t *len) {
        char next;
        read_pressed_char(&next);

        if (next != '~')
                return;

        log("Suppr!\n");

        if (*ptr == line + *len)
                return;

        delete_char(*ptr);
        --*len;
}

static void handle_escape_press(char *const line, char **ptr, size_t *len) {
        char next;
        read_pressed_char(&next);

        if (next != '[') {
                return;
        }

        read_pressed_char(&next);

        switch (next) {

        case '1':
                handle_ctrl_arrow_press(line, ptr);
                return;

        case '3':
                handle_suppr(line, ptr, len);
                return;

        case UP:
                history_load_previous(line, ptr, len);
                return;

        case DOWN:
                history_load_next(line, ptr, len);
                return;

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

static void handle_tab(char *const line, char **ptr, size_t *len) {
        char *completion = NULL;

        log("Completion...\n");

        if (contains_space != CharAndSpace) {
                log("Searching executable...\n");
                const Executable *exec = find_one_with_prefix(line, *len);
                if (exec != NULL)
                        completion = exec->name;

                if (completion != NULL) {
                        stpcpy(line, completion);
                        *len = strlen(line);
                        *ptr = line + *len;
                        log("Found exec completion %s!\n", completion);
                        return;
                }
        }

        log("Searching file...\n");
        DIR *d = opendir(".");
        if (d == NULL)
                return;

        char *line_second = line + *len;
        while (*line_second != ' ' && line_second != line)
                --line_second;
        if (line_second != line)
                ++line_second;

        struct dirent *elt;
        while ((elt = readdir(d)) != NULL) {
                log("Testing [%s] with [%s]...\n", elt->d_name, line_second);
                if (str_prefix_eq(elt->d_name, line_second)) {
                        completion = elt->d_name;
                        break;
                }
        }

        if (completion == NULL) {
                log("None found...\n");
                closedir(d);
                return;
        }

        log("Completing with %s\n", completion);
        stpcpy(line_second, completion);
        *len = strlen(line);
        *ptr = line + *len;

        log("Found file completion %s!\n", completion);

        closedir(d);
        return;
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
                *ptr = line;
                return;
        }

        case TAB:
                handle_tab(line, ptr, len);
                return;

        case ENTER:
                printf("\r%s%s\n", ps1, line);
                assert_int(strlen(line), *len);
                execute_command(line, *len);
                *len = 0;
                *ptr = line;
                **ptr = '\0';
                assert_int(strlen(line), 0l);
                return;

        case BACKSPACE:
                if (*ptr != line) {
                        --*ptr;
                        delete_char(*ptr);
                        --*len;
                }
                return;

        case -62: {
                char next;
                read_pressed_char(&next);
                if (next != -78)
                        return;
                vim_mode = NormalMode;
                return;
        }

        default:

                switch (vim_mode) {

                case NormalMode:
                        handle_normal_mode(c, ptr, line);
                        return;

                case InsertMode:
                        update_state(c);
                        ++*len;
                        insert_char(*ptr, c);
                        ++*ptr;
                        return;

                default:
                        panic("Unhandled mode %d.\n", vim_mode);
                }
        }
}

void read_pressed_char(char *c) {
        read(STDIN_FILENO, c, 1);
        log("[[%hhd](%hhd)]\n", *c, *c);
}
