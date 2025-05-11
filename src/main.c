#include "execute.h"
#include "history.h"
#include "keyboard.h"
#include "macros.h"
#include "path.h"
#include "ps1.h"
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/// Prints the current line and erases the old one.
///
/// Thanks to this function, old characters aren't left on the screen (e.g.
/// after pressing <Suppr> or <Backspace>).
static void clear_line(const size_t previous_len, const char *const line,
                       const char *const ps1, const size_t ptr_offset) {
        printf("\r");
        for (size_t i = 0; i < previous_len; ++i)
                printf(" ");
        printf("\r%s%s\033[1C\033[%luD", ps1, line, ptr_offset);
        fflush(stdout);
}

struct termios initial_termial_mode;
FILE *debug_file;
char *ps1;

/// Main runner that listens for keypress, displays the prompts and handles I/O
/// for commands and logging.
static void run_shell(void) {

        char line[MAX_LINE];
        char *ptr = line;
        *ptr = '\0';

        get_ps1(ps1);

        size_t ps1len = ps1_len(ps1) + 1;

        printf("%s%s", ps1, line);
        fflush(stdout);

        char c;
        size_t previous_len = 0, current_len = 0;

        while (1) {
                read_pressed_char(&c);

                handle_keypress(line, c, &ptr, &current_len, ps1);
                line[current_len] = '\0';

                get_ps1(ps1);
                ps1len = ps1_len(ps1) + 1;

                log("> %c\n", *ptr);

                // log("[[%s] (%zu) -> (%zu)\n", line, previous_len,
                // current_len);

                // log("line: %p | ptr: %p | diff: %lu | len: %lu\n", line, ptr,
                //     (size_t)(ptr - line), previous_len);

                clear_line(previous_len + ps1len, line, ps1,
                           current_len - (size_t)(ptr - line) + 1);
                fflush(stdout);

                previous_len = current_len;
        }
}

void close_shell(void) {
        tcsetattr(STDIN_FILENO, TCSANOW, &initial_termial_mode);
        printf("\n");

        fclose(debug_file);
        free_executables();
        free_history();
        free(ps1);

        exit(0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/// Handler for the Ctrl+C keybinding.
static void ctrl_c_handler(int x) { close_shell(); }
#pragma GCC diagnostic pop

/// Initialises the used data and enters terminal in raw mode to manage more
/// accurately the keypresses.
static void open_shell(void) {
        debug_file = fopen("b.txt", "w");
        initialise_executables();
        initialise_home();
        ps1 = malloc(64);

        tcgetattr(STDIN_FILENO, &initial_termial_mode);

        struct termios current_terminal_mode;
        current_terminal_mode = initial_termial_mode;
        current_terminal_mode.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &current_terminal_mode);

        signal(SIGINT, ctrl_c_handler);
}

/// Program entry point
int main(void) {
        open_shell();
        run_shell();
        close_shell();
        return 0;
}
