#ifndef UTIL_H
#define UTIL_H

void print_row(int id,
               const char *brief,
               const char *notes,
               const char *created,
               const char *due,
               int done,
               void *user_data);
#endif // UTIL_H