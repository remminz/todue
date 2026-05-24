#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "sqlite/sqlite3.h"

#include "todue/db.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

// min/max gcc and clang only
#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

FILE *openPager(void);
void closePager(FILE *fp);
void print_row(const TodueItem *row, void *user_data);
char *substr(const char *source, size_t offset, size_t size);
char *strjoin(const char *s1, const char *s2);
void skip_space(char **str);
void check_table(sqlite3 *db);

#endif // UTIL_H
