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

Executable *find_with_prefix(const ExecutableList *const list,
                             const char *const prefix,
                             const size_t prefix_len) {
        for (size_t i = 0; i < list->len; ++i) {
                if (strncmp(prefix, list->values[i].name, prefix_len) == 0) {
                        return &list->values[i];
                }
        }
        return NULL;
}

#ifdef TEST
static void count_executables(ExecutableList *list) {
        const char *const command =
            "echo $PATH | sed 's/:/\\n/g' | grep -v ^$  | xargs ls -lA | grep "
            "^-  | awk '{print $NF}' | sort -n | uniq | wc -l";

        FILE *find = popen(command, "r");

        if (find == NULL)
                panic("Failed to execute");

        char line[16];
        fgets(line, 16, find);
        size_t expected = (size_t)atoi(line) + 1;
        assert(expected == list->len);
}

static void check_exact_completion(ExecutableList *list,
                                   const char *const prefix,
                                   const size_t prefix_len,
                                   const char *const path, const char *name) {
        const Executable *const exec =
            find_with_prefix(list, prefix, prefix_len);
        if (exec == NULL)
                panic("None found for %s.\n", prefix);

        if (strcmp(exec->name, name))
                panic("expected %s but found %s.\n", name, exec->name);
        if (strcmp(exec->path, path))
                panic("expected %s but found %s for exec %s.\n", path,
                      exec->path, name);
}

static void check_completion(ExecutableList *list) {
        check_exact_completion(list, "rustu", 5,
                               "/home/b/.files/.apps/cargo/bin", "rustup");
        check_exact_completion(list, "ala", 3, "/usr/bin", "alacritty");
        check_exact_completion(list, "bra", 3, "/usr/bin", "brave");
        check_exact_completion(list, "xe", 2, "/usr/bin", "xev");
        check_exact_completion(list, "prof", 4, "/home/b/.files/.cmd",
                               "profile");
        assert(find_with_prefix(list, "ab", 2) == NULL);
        assert(find_with_prefix(list, "abc", 3) == NULL);
}

int main(void) {
        ExecutableList list = get_executables();
        count_executables(&list);
        check_completion(&list);
}
#endif
