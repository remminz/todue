#ifndef UTIL_H
#define UTIL_H

#include "sqlite/sqlite3.h"

#include <stddef.h>
#include <stdio.h>
#include <time.h>

// gcc and clang only
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
void print_row(
    int         id,
    const char *brief,
    const char *notes,
    const char *created,
    const char *due,
    int         done,
    void       *user_data);
const char *substr(const char *source, size_t offset, size_t size);
void skip_space(char **str);
void check_table(sqlite3 *db);

#endif // UTIL_H