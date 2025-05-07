#include "display.h"
#include "execute.h"
#include "keyboard.h"
#include "shell.h"
#include <assert.h>
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

void run_shell()
{
        debug_file = fopen("b.txt", "w");

        char line[1000];
        char *ptr = line;
        *ptr = '\0';
        char c;
        size_t previous_len = 0;

        printf("$ ");
        fflush(stdout);

        while (1)
        {

                read_pressed_char(&c);

                fprintf(debug_file, ">>> %c (%d)\n", c, c);
                fflush(debug_file);

                clear_line(previous_len + 2);

                handle_keypress(debug_file, line, c, &ptr, &previous_len);

                line[previous_len] = '\0';
                printf("$ %s\r\033[%luC", line, ptr - line + 2);
                fflush(stdout);
        }
}

void close_shell()
{
        fclose(debug_file);
        printf("\n");
        tcsetattr(STDIN_FILENO, TCSANOW, &initial_termial_mode);
        exit(0);
}

void ctrl_c_handler(int x) { close_shell(); }

void open_shell()
{
        tcgetattr(STDIN_FILENO, &initial_termial_mode);

        struct termios current_terminal_mode;
        current_terminal_mode = initial_termial_mode;
        current_terminal_mode.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &current_terminal_mode);

        signal(SIGINT, ctrl_c_handler);
}
