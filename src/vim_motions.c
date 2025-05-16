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

static bool add_simple_repeat(Motion *const motion, const enum Action a,
                              const MotionType type) {
        if (type == NoType) {
                motion->action = a;
                return true;
        }
        if (type == MotionChild && motion->child->action == NoMotion) {
                motion->child->action = a;
                return true;
        }
        return false;
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
