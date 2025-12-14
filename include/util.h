#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void print_row(int id,
               const char *brief,
               const char *notes,
               const char *created,
               const char *due,
               int done,
               void *user_data);
const char *substr(const char *source, size_t offset, size_t size);
void skip_spaces(char **str);

#endif // UTIL_H