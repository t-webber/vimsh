#include "macros.h"
#include "main.h"
#include "normal_mode.h"
#include "str.h"

typedef enum { None = '\0', f = 'f', F = 'F' } VimAction;

typedef struct {
        int number;
        VimAction action;
        String arguments;
} VimCommand;

static VimCommand current_command = {
    .number = 0, .action = None, .arguments = NEW_STRING};

static void reset_command(void) {
        current_command.number = 0;
        current_command.arguments.len = 0;
        current_command.arguments.value[0] = '\0';
        current_command.action = None;
}

static int get_count(void) {
        if (current_command.number == 0)
                return 1;
        return current_command.number;
}

static void try_execute_command(char **ptr, const char *const line) {
        log("Executing command: |%d|%c|%s|\n", current_command.number,
            current_command.action, current_command.arguments.value);

        switch (current_command.action) {

        case f: {
                if (current_command.arguments.len != 1)
                        return;

                const char expected_char = *current_command.arguments.value;
                const int expected_count = get_count();
                int count = 0;
                while (**ptr != '\0' && count < expected_count) {
                        ++*ptr;
                        if (**ptr == expected_char) {
                                count += 1;
                        }
                }
                reset_command();

                return;
        }

        case F: {
                if (current_command.arguments.len != 1)
                        return;

                const char expected_char = *current_command.arguments.value;
                const int expected_count = get_count();
                int count = 0;
                while (*ptr != line && count < expected_count) {
                        --*ptr;
                        if (**ptr == expected_char) {
                                count += 1;
                        }
                }
                reset_command();
                return;
        }

        case None:
                return;
        default:
                panic("Unhandled enum variant\n");
        }
}

void handle_normal_mode(const char c, char **ptr, char *const line) {

        switch (c) {

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
                if (current_command.action != None)
                        push_string(&current_command.arguments, c);
                else if (current_command.number == 0)
                        current_command.number = c - '0';
                else
                        current_command.number =
                            current_command.number * 10 + c - '0';
                break;

        case 'a':
                if (current_command.action != None) {
                        push_string(&current_command.arguments, c);
                        break;
                }
                if (**ptr != '\0')
                        ++*ptr;
                reset_command();
                vim_mode = InsertMode;
                break;

        case 'i':
                reset_command();
                vim_mode = InsertMode;
                break;

        case 'f':
                if (current_command.action == None)
                        current_command.action = f;
                else
                        push_string(&current_command.arguments, c);
                break;

        case 'F':
                if (current_command.action == None)
                        current_command.action = F;
                else
                        push_string(&current_command.arguments, c);
                break;

        default:
                push_string(&current_command.arguments, c);
        }

        try_execute_command(ptr, line);
}

void free_vim_command(void) { free(current_command.arguments.value); }
