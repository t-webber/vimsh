#include "macros.h"
#include "main.h"
#include "normal_mode.h"
#include <stdbool.h>

typedef struct Motion {
        int repeat;
        enum Action {
                f = 'f',
                F = 'F',
                d = 'd',
                s = 's',
                S = 'S',
                y = 'y',
                w = 'w',
                W = 'W',
                EOL = '$',
                BOL = '^',
                NoMotion = '0'
        } action;
        union {
                struct Motion *child;
                struct {
                        char arg;
                };
                struct {
                        char arg0;
                        char arg1;
                };
        };
} Motion;

typedef enum {
        NoType,
        MotionChild,
        SimpleRepeat,
        SingleChar,
        DoubleChar
} MotionType;

static MotionType get_motion_type(const Motion *const m) {
        switch (m->action) {

        case NoMotion:
        case EOL:
        case BOL:
                return NoType;

        case d:
        case y:
                return MotionChild;

        case w:
        case W:
                return SimpleRepeat;

        case f:
        case F:
                return SingleChar;

        case s:
        case S:
                return DoubleChar;

                default_case(m->action);
        }
}

static int get_count(const Motion *const m) {
        const MotionType type = get_motion_type(m);

        switch (type) {

        case NoType:
                return 1;

        case MotionChild:
                return get_count(m->child);

        case SimpleRepeat:
        case SingleChar:
        case DoubleChar:
                if (m->repeat == 0)
                        return 1;
                return m->repeat;

                default_case(type);
        }
}

static void log_motion(const Motion motion, const MotionType type) {

        log("@ [%d]%c", motion.repeat, motion.action);

        switch (type) {

        case NoType:
        case SimpleRepeat:
                break;

        case MotionChild: {

                const MotionType child_type = get_motion_type(motion.child);
                log("|[%d]%c", motion.child->repeat, motion.child->action);

                switch (child_type) {

                case NoType:
                case SimpleRepeat:
                        break;

                case MotionChild:
                        panic("Can't nest children.\n");

                case SingleChar:
                        log("(%c)", motion.arg);
                        break;

                case DoubleChar:
                        log("(%c, %c)", motion.arg0, motion.arg1);
                        break;

                        default_case(child_type);
                }
                log("|");
                break;
        }

        case SingleChar:
                log("(%c)", motion.arg);
                break;

        case DoubleChar:
                log("(%c, %c)", motion.arg0, motion.arg1);
                break;

                default_case(type);
        }

        log("\n");
}

static bool add_single_char_motion(Motion *const motion, const enum Action a,
                                   const MotionType type) {
        if (type == NoType) {
                motion->action = a;
                motion->arg = '\0';
                return true;
        }
        if (type == MotionChild && motion->child->action == NoMotion) {
                motion->child->action = a;
                motion->child->arg = '\0';
                return true;
        }
        return false;
}

static bool add_double_char_motion(Motion *const motion, const enum Action a,
                                   const MotionType type) {
        if (type == NoType) {
                motion->action = a;
                motion->arg0 = '\0';
                motion->arg1 = '\0';
                return true;
        }
        if (type == MotionChild && motion->child->action == NoMotion) {
                motion->child->action = a;
                motion->child->arg0 = '\0';
                motion->child->arg1 = '\0';
                return true;
        }
        return false;
}

static void increment_repeat(Motion *const motion, const char c) {
        int digit = c - '0';
        if (motion->repeat == 0)
                motion->repeat = digit;
        else
                motion->repeat = motion->repeat * 10 + digit;
}

static void push_arg(const MotionType type, Motion *const motion,
                     const char c) {
        switch (type) {

        case NoType:
        case SimpleRepeat:
                log("Invalid binding %c\n.", c);
                return;

        case MotionChild:
                push_arg(get_motion_type(motion->child), motion->child, c);
                return;

        case SingleChar:
                assert(motion->arg == '\0');
                motion->arg = c;
                return;

        case DoubleChar:
                if (motion->arg0 == '\0') {
                        motion->arg0 = c;
                        return;
                }

                assert(motion->arg1 == '\0');
                motion->arg1 = c;
                return;

                default_case(type);
        }
}

static Motion current_motion = {.repeat = 0, .action = NoMotion};

static void reset_command(void) {
        current_motion = (Motion){.repeat = 0, .action = NoMotion};
}

static bool check_reached(const bool is_lower, const char *const ptr,
                          const char *const line, const int current,
                          const int expected) {

        return (is_lower ? *ptr != '\0' : ptr != line) && current < expected;
}

static void try_execute_command(char **ptr, const char *const line) {
        const MotionType type = get_motion_type(&current_motion);
        const enum Action action = current_motion.action;

        log_motion(current_motion, type);

        switch (action) {

        case NoMotion:
                return;

        case F:
        case f: {
                if (current_motion.arg == '\0')
                        return;

                const bool is_lower = action == f;
                const char expected_char = current_motion.arg;
                const int expected_cnt = get_count(&current_motion);
                int cnt = 0;

                while (check_reached(is_lower, *ptr, line, cnt, expected_cnt)) {
                        if (is_lower)
                                ++*ptr;
                        else
                                --*ptr;
                        if (**ptr == expected_char)
                                cnt += 1;
                }

                reset_command();
                return;
        }

        case S:
        case s: {
                if (*line == '\0') {
                        reset_command();
                        return;
                }

                if (current_motion.arg0 == '\0' || current_motion.arg1 == '\0')
                        return;

                const bool is_lower = action == s;
                const char c0 = current_motion.arg0;
                const char c1 = current_motion.arg1;
                const int expected_cnt = get_count(&current_motion);
                int cnt = 0;

                while (check_reached(is_lower, *ptr, line, cnt, expected_cnt)) {
                        if (is_lower)
                                ++*ptr;
                        else
                                --*ptr;
                        assert(**ptr != '\0');
                        if (**ptr == c0 && *(*ptr + 1) == c1)
                                cnt += 1;
                }

                reset_command();
                return;
        }

        case d:
        case y:
        case w:
        case W:
        case EOL:
        case BOL:

                default_case(action);
        }
}

void handle_normal_mode(const char c, char **ptr, char *const line) {

        const MotionType type = get_motion_type(&current_motion);

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
                if (type == NoType)
                        increment_repeat(&current_motion, c);
                else if (type == MotionChild)
                        increment_repeat(current_motion.child, c);
                else
                        goto label;
                break;

        case 'a':
                if (type != NoType)
                        goto label;
                if (**ptr != '\0')
                        ++*ptr;
                reset_command();
                vim_mode = InsertMode;
                break;

        case 'i':
                if (type != NoType)
                        goto label;
                reset_command();
                vim_mode = InsertMode;
                break;

        case 'f':
        case 'F':
                if (!add_single_char_motion(&current_motion, (enum Action)c,
                                            type))
                        goto label;
                break;

        case 's':
        case 'S':
                if (!add_double_char_motion(&current_motion, (enum Action)c,
                                            type))
                        goto label;
                break;

        label:
        default:
                push_arg(type, &current_motion, c);
        };

        try_execute_command(ptr, line);
}
