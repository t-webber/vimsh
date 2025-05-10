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
        char *end = pwd + len;
        assert(*end == '\0');

        path = stpcpy(path, "\x1b[36m");
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
        fclose(bat);
        return atoi(output);
}

static char *git_branch(char *end) {
        FILE *head_fd = fopen(".git/HEAD", "r");
        if (head_fd == NULL) {
                return end;
        }

        char head[128];
        fgets(head, sizeof(head), head_fd);
        fclose(head_fd);

        *(head + strlen(head) - 1) = '\0';
        end = stpcpy(end, "\x1b[32m");

        if (strcmp(head + 16, "main") == 0)
                return stpcpy(end, ". ");

        else if (strcmp(head + 16, "master") == 0)
                return stpcpy(end, ": ");

        else {
                end = stpcpy(end, head + 16);
                *end++ = ' ';
                return end;
        }
}

size_t ps1_len(const char *const ps1) {
        size_t len = 0;
        size_t esc_cnt = 0;
        for (const char *counter = ps1; *counter != '\0'; ++counter, ++len)
                if (*counter == '\033')
                        ++esc_cnt;
        return len - 5 * esc_cnt;
}

void get_ps1(char *const ps1) {
        char *end = ps1;
        if (get_battery_level() < 30)
                end = stpcpy(end, "\x1b[31m! ");

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        end += sprintf(end, "\x1b[33m%d%d ", tm.tm_hour % 12, tm.tm_min);

        assert(*end == '\0');

        end = pwd(end);
        assert(*end == '\0');
        *end++ = ' ';

        end = git_branch(end);
        end = stpcpy(end, "\x1b[0m");

        assert(*end == '\0');
}
