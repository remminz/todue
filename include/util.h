#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

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

#endif // UTIL_H