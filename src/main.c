#include "display.h"
#include "keyboard.h"
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/// Main runner that displays the prompt, the written characters and handles the
/// keypresses.
void shell()
{
        FILE *debug_file = fopen("b.txt", "w");

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

        fclose(debug_file);
}

/// Wrapper around the main runner (@ref shell) for terminal support.
///
/// This functions enters raw mode to prevent all the default keybindings and
/// leaves the raw mode when the runner ends.
void handle_terminal_mode(void (*runner)())
{
        struct termios oldt, newt;

        // Save current terminal mode
        tcgetattr(STDIN_FILENO, &oldt);

        // Enter terminal raw mode
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        runner();

        // Restore terminal to default mode
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

/// Program entry point
int main()
{
        handle_terminal_mode(shell);

        return 0;
}
