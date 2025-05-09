/// Computes the PS1 variable for the shell prompt

#include "macros.h"
#include "ps1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHORT(path, alias, condition)                                          \
        if (condition) {                                                       \
                stpcpy(path, alias);                                           \
                return;                                                        \
        }

#define SHORTP(path, pwd, var, alias)                                          \
        SHORT(path, alias, strcmp(getenv(var), pwd) == 0);

static char *exec(const char *const command) {
        char *output = malloc(200 * sizeof(char));
        *output = '\0';
        FILE *f = popen(command, "r");
        fgets(output, 199, f);
        pclose(f);
        return output;
}

static void pwd(char *path) {
        char *pwd = exec("pwd");
        const size_t len = strlen(pwd);
        assert(len > 1);
        char *end = pwd + len;
        assert(*end == '\0');
        --end;
        assert(*end == '\n');
        *end = '\0';

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

        stpcpy(path, end);

        free(pwd);
}

static int get_battery_level(void) {
        FILE *bat = fopen("/sys/class/power_supply/BAT1/capacity", "r");
        char output[5];
        fgets(output, 5, bat);
        return atoi(output);
}

void get_ps1(char *ps1) {
        char path[50];
        pwd(path);

        int battery_level = get_battery_level();
        char battery[3] = "";
        if (battery_level < 30) {
                stpcpy(battery, "! ");
        }

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        char *branch = exec("git branch --show-current 2>/dev/null");
        if (*branch != '\0')
                *(branch + strlen(branch) - 1) = ' ';

        if (strcmp(branch, "main ") == 0)
                stpcpy(branch, ". ");
        if (strcmp(branch, "master ") == 0)
                stpcpy(branch, ": ");

        sprintf(ps1, "%s%d%d %s %s", battery, tm.tm_hour % 12, tm.tm_min, path,
                branch);
}
