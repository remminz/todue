#include "util.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

void print_row(int id,
               const char *brief,
               const char *notes,
               const char *created,
               const char *due,
               int done,
               void *user_data)
{
    printf("%d [%c] %-20s", id, done ? 'X' : ' ', brief);
    if (due) {
        printf(" | due: %s", due);
    }
    putchar('\n');
}