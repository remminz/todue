#include "todue/util.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "todue/db.h"
#include "todue/log.h"

// ANSI Sequences
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define END_BOLD_DIM "\033[22m"
#define ITALIC "\033[3m"
#define END_ITALIC "\033[23m"
#define STRIKETHROUGH "\033[9m"
#define END_STRIKETHROUGH "\033[29m"
#define END_ALL "\033[0m"

#define WHITE "\033[38;5;255m"
#define LIGHT_GRAY "\033[38;5;253m"
#define GRAY "\033[38;5;250m"


void print_row(
    int         id,
    const char *brief,
    const char *notes,
    const char *created,
    const char *due,
    int         done,
    void       *user_data)
{
    (void)user_data;
    const int LEFT_WIDTH = 30;
    const int RIGHT_WIDTH = 80;

    printf("%s%3d%s [%c]%s ", BOLD, id, END_ITALIC, done ? 'X' : ' ', END_BOLD_DIM);
    if (done) {
        fputs(STRIKETHROUGH DIM, stdout);
    } else {
        fputs(WHITE BOLD, stdout);
    }
    printf("%-*.*s%s |", LEFT_WIDTH, LEFT_WIDTH, brief, END_ALL);

    if (notes) {
        printf(" %.*s\n", RIGHT_WIDTH, notes);
    } else {
        printf(" %s%.*s%s\n", DIM, RIGHT_WIDTH, "No notes", END_BOLD_DIM);
    }

    if (due) {
        printf("   %sdue: %-*s%s |", LIGHT_GRAY ITALIC, LEFT_WIDTH, due, END_ITALIC);
    } else {
        printf("\t%s%-*s%s |", DIM, LEFT_WIDTH, "No due date", END_BOLD_DIM);
    }

    printf("%s created: %s%s\n", GRAY ITALIC, created, END_ALL);

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

struct tm *localtime_safe(const time_t *t, struct tm *result) {
    if (t == NULL || result == NULL) {
        return NULL;
    }

#ifdef _WIN32
    return localtime_s(result, t);
#else
    return localtime_r(t, result);
#endif
}

void check_table(sqlite3 *db) {
    if (no_such_table(db)) {
        fprintf(stderr, "Database not initialized; try reload command\n");
    }
}