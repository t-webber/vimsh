#include "macros.h"
#include "ps1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SHORT(path, alias, condition)                                          \
        if (condition) {                                                       \
                return stpcpy(path, alias);                                    \
        }

#define SHORTP(path, pwd, var, alias)                                          \
        SHORT(path, alias, strcmp(getenv(var), pwd) == 0);

static char *pwd(char *path) {
        char pwd[128];
        getcwd(pwd, 128);

        const size_t len = strlen(pwd);
        assert(len > 1);
        char *end = pwd + len;
        assert(*end == '\0');

        SHORT(path, "/", strcmp(pwd, "/") == 0);
        SHORTP(path, pwd, "HOME", "~");
        SHORTP(path, pwd, "FILES", "f");
        SHORTP(path, pwd, "CMD", "c");
        SHORTP(path, pwd, "APPS", "a");
        SHORTP(path, pwd, "DEV", "d");

        while (*end != '/' && *end != '.')
                --end;

        if (end != pwd || *(end + 1) != '\0')
                ++end;

        return stpcpy(path, end);
}

static int get_battery_level(void) {
        FILE *bat = fopen("/sys/class/power_supply/BAT1/capacity", "r");
        char output[5];
        fgets(output, 5, bat);
        return atoi(output);
}

static void git_branch(char *end) {
        FILE *head = fopen(".git/HEAD", "r");
        if (head == NULL) {
                *end = '\0';
                return;
        }

        char line[128];
        fgets(line, sizeof(line), head);
        fclose(head);

        *(line + strlen(line) - 1) = '\0';

        if (strcmp(line + 16, "main") == 0)
                stpcpy(end, ". ");

        else if (strcmp(line + 16, "master") == 0)
                stpcpy(end, ": ");

        else {
                end = stpcpy(end, line + 16);
                stpcpy(end, " ");
        }
}

void get_ps1(char *const ps1) {
        char *end = ps1;
        *end = '\0';

        if (get_battery_level() < 30)
                end = stpcpy(end, "! ");

        assert(*end == '\0');

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        end += sprintf(end, "%d%d ", tm.tm_hour % 12, tm.tm_min);

        assert(*end == '\0');

        end = pwd(end);
        assert(*end == '\0');
        *end++ = ' ';

        git_branch(end);
}
