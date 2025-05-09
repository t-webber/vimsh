#include "macros.h"
#include "path.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void realloc_executable_list(ExecutableList *list) {
        const size_t new_cap = 10 + list->cap * 2;
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
        closedir(d);
}

static ExecutableList get_executables(void) {
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

static ExecutableList executable_list;

void initialise_executables(void) { executable_list = get_executables(); }

void free_executables(void) {
        char **paths = malloc(sizeof(char *) * executable_list.len);
        size_t paths_len = 0;
        for (size_t i = 0; i < executable_list.len; ++i) {
                free(executable_list.values[i].name);
                char *path = executable_list.values[i].path;

                bool already_taken = false;
                for (size_t j = 0; j < paths_len; ++j)
                        if (path == paths[j]) {
                                already_taken = true;
                                break;
                        }
                if (!already_taken)
                        paths[paths_len++] = path;
        }

        for (size_t i = 0; i < paths_len; ++i)
                free(paths[i]);

        free(paths);
        free(executable_list.values);
}

Executable *find_one_with_prefix(const char *const prefix,
                                 const size_t prefix_len) {
        for (size_t i = 0; i < executable_list.len; ++i) {
                if (strncmp(prefix, executable_list.values[i].name,
                            prefix_len) == 0) {
                        return &executable_list.values[i];
                }
        }
        return NULL;
}

#ifdef TEST
static void count_executables(void) {
        const char *const command =
            "echo $PATH | sed 's/:/\\n/g' | grep -v ^$  | xargs ls -lA | grep "
            "^-  | awk '{print $NF}' | sort -n | uniq | wc -l";

        FILE *find = popen(command, "r");

        if (find == NULL)
                panic("Failed to execute");

        char line[16];
        fgets(line, 16, find);
        size_t expected = (size_t)atoi(line) + 1;
        assert(expected == executable_list.len);

        pclose(find);
}

static void check_exact_completion(const char *const prefix,
                                   const size_t prefix_len,
                                   const char *const path, const char *name) {
        const Executable *const exec = find_one_with_prefix(prefix, prefix_len);
        if (exec == NULL)
                panic("None found for %s.\n", prefix);

        if (strcmp(exec->name, name))
                panic("expected %s but found %s.\n", name, exec->name);
        if (strcmp(exec->path, path))
                panic("expected %s but found %s for exec %s.\n", path,
                      exec->path, name);
}

static void check_completion(void) {
        check_exact_completion("rustu", 5, "/home/b/.files/.apps/cargo/bin",
                               "rustup");
        check_exact_completion("ala", 3, "/usr/bin", "alacritty");
        check_exact_completion("bra", 3, "/usr/bin", "brave");
        check_exact_completion("xe", 2, "/usr/bin", "xev");
        check_exact_completion("prof", 4, "/home/b/.files/.cmd", "profile");
        assert(find_one_with_prefix("ab", 2) == NULL);
        assert(find_one_with_prefix("abc", 3) == NULL);
}

int main(void) {
        initialise_executables();
        count_executables();
        check_completion();
        free_executables();
}
#endif
