#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <time.h>

void print_row(
    int         id,
    const char *brief,
    const char *notes,
    const char *created,
    const char *due,
    int         done,
    void       *user_data
);
const char *substr(const char *source, size_t offset, size_t size);
void skip_space(char **str);
char *xstrdup(const char *str); // strdup wrapper since strdup has limited availability
struct tm *localtime_safe(const time_t *t, struct tm *result); // cross-platform thread-safe localtime wrapper

#endif // UTIL_H