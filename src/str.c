#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/// Tests the @ref insert_fchar
void test_insert_char()
{
        char string[20] = "Hello world";
        insert_char(string + 5, ',');
        insert_char(string + 6, '-');
        insert_char(string, '#');
        insert_char(string + 14, '!');

        if (strcmp("#Hello,- world!", string))
        {
                fprintf(stderr, "Hello world error\n");
                exit(1);
        }

        char empty[20] = "";
        insert_char(empty, '!');

        if (strcmp(empty, "!"))
        {
                fprintf(stderr, "Empty error\n");
                exit(1);
        }
}
