#include "macros.h"
#include "path.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void realloc_executable_list(ExecutableList *list) {
        const size_t new_cap = 1 + list->cap;
        Executable *new_values = malloc(new_cap * sizeof(Executable));
        for (size_t i = 0; i < list->len; ++i) {
                Executable old = list->values[i];
                new_values[i].name = old.name;
                new_values[i].path = old.path;
        }

        if (list->values != NULL)
                free(list->values);

        list->cap = new_cap;
        list->values = new_values;
}

static void push_executable(ExecutableList *list, Executable executable) {
        if (list->len == list->cap) {
                realloc_executable_list(list);
        }
        list->values[list->len] = executable;
        ++list->len;
}

static void find_executables_in_path(ExecutableList *list, char *path) {

        DIR *d = opendir(path);
        if (d == NULL) {
                panic("Failed to read folder %s.\n", path);
        }
        struct dirent *elt;
        while ((elt = readdir(d)) != NULL) {
                if (elt->d_type == DT_REG) {
                        const char *const temp_name = elt->d_name;
                        char *name = malloc(strlen(temp_name) + 1);
                        stpcpy(name, temp_name);
                        push_executable(
                            list, (Executable){.name = name, .path = path});
                }
        }
}

ExecutableList get_executables(void) {
        ExecutableList list = {.len = 0, .cap = 0, .values = NULL};
        char *const path = getenv("PATH");
        for (char *start = path + 1, *end = path; *end != '\0';) {
                ++end;
                if ((*end == '\0' || *end == ':') && end > start) {
                        bool is_end = *end == '\0';
                        *end = '\0';
                        char *path_elt = malloc((u_long)(end - start) + 1ul);
                        stpcpy(path_elt, start);
                        find_executables_in_path(&list, path_elt);
                        if (is_end)
                                return list;
                        *end = ':';
                        start = (end + 1);
                }
        }
        panic("Failed to read path");
}

#ifdef TEST
#define log2(x) printf("%s => %d\n", #x, x);

int main(void) {
        printf("Testing completions...\n");

        ExecutableList list = get_executables();

        const char *const command =
            "echo $PATH | sed 's/:/\\n/g' | grep -v ^$  | xargs ls -lA | grep "
            "^-  | awk '{print $NF}' | sort -n | uniq | wc -l";

        FILE *find = popen(command, "r");

        if (find == NULL)
                panic("Failed to execute");

        char line[16];
        fgets(line, 16, find);
        size_t expected = (size_t)atoi(line) + 1;
        assert(expected == list.len);
}
#endif
