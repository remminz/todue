#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "log.h"

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

const char *substr(const char *source, size_t offset, size_t size) {
    if (source == NULL || offset < 0 || size <= 0) {
        return NULL;
    }

    size_t source_len = strlen(source);
    if (offset + size > source_len) {
        size = source_len - offset;
    }

    char *substring = malloc(sizeof(char) * (size + 1));
    if (substring == NULL) {
        return NULL;
    }

    strncpy(substring, source + offset, size);
    substring[size] = '\0';

    return substring;
}

void skip_spaces(char **str) {
    while (**str && isspace(**str)) {
        ++(*str);
    }
}