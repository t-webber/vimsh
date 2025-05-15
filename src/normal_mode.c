#include "macros.h"
#include "main.h"
#include "normal_mode.h"
#include "str.h"

typedef enum {
        None = '\0',
        f = 'f',
} VimAction;

typedef struct {
        int number;
        VimAction action;
        char *arguments;
} VimCommand;

static VimCommand current_command = {
    .number = 0, .action = None, .arguments = NULL};

static void reset_command(void) {
        current_command.number = 0;
        current_command.arguments = NULL;
        current_command.action = None;
}

void handle_normal_mode(char c, char **ptr) {
        log("Command: |%d|%c|%s|\n", current_command.number,
            current_command.action, current_command.arguments);

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
                if (current_command.action == None) {

                }

                else if (current_command.number == 0) {
                        current_command.number = c - '0';
                } else {
                        current_command.number =
                            current_command.number * 10 + c - '0';
                }
                return;

        case 'a':
                if (**ptr != '\0')
                        ++*ptr;
                reset_command();
                vim_mode = InsertMode;
                return;

        case 'i':
                reset_command();
                vim_mode = InsertMode;
                return;

        default:
                return;
        }
}
