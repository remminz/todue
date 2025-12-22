#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define BRIEF_WIDTH 20

void print_row(
    int         id,
    const char *brief,
    const char *notes,
    const char *created,
    const char *due,
    int         done,
    void       *user_data)
{
    (void)notes;
    (void)created;
    (void)user_data;

    printf("%d [%c] %-*.*s", id, done ? 'X' : ' ', BRIEF_WIDTH, BRIEF_WIDTH, brief);

    if (due) {
        printf(" | due: %s", due);
    }
    
    putchar('\n');
}

const char *substr(const char *src, size_t idx, size_t size) {
    if (src == NULL || idx < 0 || size <= 0) {
        return NULL;
    }

    size_t src_len = strlen(src);
    if (idx + size > src_len) {
        size = src_len - idx;
    }

    char *substring = malloc(sizeof(char) * (size + 1));
    if (substring == NULL) {
        return NULL;
    }

    strncpy(substring, src + idx, size);
    substring[size] = '\0';

    return substring;
}

void skip_space(char **str) {
    while (**str && isspace(**str)) {
        ++(*str);
    }
}

char *xstrdup(const char *str) {
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    return strdup(s);
#else
    size_t len = strlen(str) + 1;
    char *p = malloc(len);
    if (p) {
        memcpy(p, str, len);
    }
    return p;
#endif
}