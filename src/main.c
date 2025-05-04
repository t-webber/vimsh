#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/// Inserts a char in a string.
///
/// This performs `*insert_position = c` without deleting the data under
/// `insert_position`. It thus shifts the whole stirng.
void insert_char(char *const insert_position, char c)
{
        char next = *insert_position, current = 1, *shifter = insert_position;

        while (current != '\0')
        {
                current = next;
                next = *++shifter;
                *shifter = current;
        }

        *insert_position = c;
}

/// Tests the @ref insert_fchar
void insert_char_test()
{

        char string[20] = "Hello world";
        insert_char(string + 5, ',');
        insert_char(string + 6, '-');
        insert_char(string, '#');
        insert_char(string + 14, '!');

        if (strcmp("#Hello,- world!", string))
        {
                fprintf(stderr, "Hello world error\n");
                exit(1);
        }

        char empty[20] = "";
        insert_char(empty, '!');

        if (strcmp(empty, "!"))
        {
                fprintf(stderr, "Empty error\n");
                exit(-1);
        }
}

/// Handle keypresses and edits the line buffer in consequences.
void handle_keypress(FILE *debug_file, const char *const line, char c,
                     char **ptr, size_t *len)
{

        if (c == 27)
        {
                fprintf(debug_file, "Escaped!\n");

                char next;
                read(STDIN_FILENO, &next, 1);

                fprintf(debug_file, "Then %c!\n", next);

                if (next != '[')
                        return;

                read(STDIN_FILENO, &next, 1);

                fprintf(debug_file, "Then %c!\n", next);
                fflush(debug_file);

                if (next == 'D')
                        if (*ptr != line)
                                --*ptr;

                if (next == 'C')
                        if (**ptr != '\0')
                                ++*ptr;

                return;
        }

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

/// Clears the current line.
///
/// Thanks to this function, old characters aren't left on the screen (e.g.
/// after pressing <Del>).
void clear_line(const size_t len)
{
        printf("\r");
        for (size_t i = 0; i < len; ++i)
                printf(" ");
        fflush(stdout);
}

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
                read(STDIN_FILENO, &c, 1);
                fprintf(debug_file, ">>> %c (%d)\n", c, c);
                fflush(debug_file);

                clear_line(previous_len + 2);
                handle_keypress(debug_file, line, c, &ptr, &previous_len);
                line[previous_len] = '\0';
                line[previous_len + 1] = '@';
                line[previous_len + 2] = '\0';

                printf("\r$ %s\r\033[%luC", line, ptr - line + 2);
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
        insert_char_test();

        handle_terminal_mode(shell);

        return 0;
}
