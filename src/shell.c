#include "display.h"
#include "execute.h"
#include "keyboard.h"
#include "macros.h"
#include "path.h"
#include "ps1.h"
#include "shell.h"
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

struct termios initial_termial_mode;
FILE *debug_file;

void run_shell(void) {

        char line[MAX_LINE];
        char *ptr = line;
        *ptr = '\0';
        char c;
        size_t previous_len = 0;

        char ps1[32];
        get_ps1(ps1);
        size_t ps1_len = strlen(ps1);

        printf("%s", ps1);
        fflush(stdout);

        while (1) {
                read_pressed_char(&c);

                log(">>> %c (%d)\n", c, c);

                clear_line(previous_len + ps1_len);

                handle_keypress(line, c, &ptr, &previous_len, ps1);
                line[previous_len] = '\0';

                get_ps1(ps1);
                ps1_len = strlen(ps1);

                printf("%s%s\r\033[%luC", ps1, line,
                       (size_t)(ptr - line) + ps1_len);
                fflush(stdout);
        }
}

void close_shell(void) {
        fclose(debug_file);
        printf("\n");
        tcsetattr(STDIN_FILENO, TCSANOW, &initial_termial_mode);
        exit(0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/// Handler for the Ctrl+C keybinding.
static void ctrl_c_handler(int x) { close_shell(); }
#pragma GCC diagnostic pop

void open_shell(void) {
        debug_file = fopen("b.txt", "w");
        initialise_executables();
        initialise_home();

        tcgetattr(STDIN_FILENO, &initial_termial_mode);

        struct termios current_terminal_mode;
        current_terminal_mode = initial_termial_mode;
        current_terminal_mode.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &current_terminal_mode);

        signal(SIGINT, ctrl_c_handler);
}
