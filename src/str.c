#include "str.h"
#include <assert.h>
#include <string.h>

void delete_char(char *const delete_position)
{
        for (char *reader = delete_position; *reader != '\0'; ++reader)
                *reader = *(reader + 1);
}

void insert_char(char *const insert_position, char c)
{
        char next = *insert_position, current = 1, *shifter = insert_position;

        while (current != '\0')
        {
                current = next;
                next = *++shifter;
                *shifter = current;
        }

        *insert_position = c;
}

/// Tests the @ref delete_char function
static void test_delete_char()
{
        char string[] = "#Hello, world!";
        delete_char(string);
        delete_char(string + 5);
        delete_char(string + 11);

        assert(strcmp("Hello world", string) == 0);

        char empty[] = "1";
        delete_char(empty);
        assert(empty[0] == '\0');
}

/// Tests the @ref insert_char function.
static void test_insert_char()
{
        char string[20] = "Hello world";
        insert_char(string + 5, ',');
        insert_char(string + 6, '-');
        insert_char(string, '#');
        insert_char(string + 14, '!');

        assert(strcmp("#Hello,- world!", string) == 0);

        char empty[20] = "";
        insert_char(empty, '!');
        assert(strcmp(empty, "!") == 0);
}

#ifdef TEST
int main()
{
        test_delete_char();
        test_insert_char();
}
#endif
