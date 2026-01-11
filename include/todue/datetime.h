#ifndef DATETIME_H
#define DATETIME_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    SECOND = 1,
    MINUTE = SECOND * 60,
    HOUR   = MINUTE * 60,
    DAY    = HOUR * 24,
    WEEK   = DAY * 7,
    MONTH  = 1, // can't use seconds
    YEAR   = MONTH * 12,
} TimeUnit;

bool is_valid_date(const char *s);
bool is_valid_time(const char *s);
bool is_valid_short_time(const char *s);
bool is_valid_datetime(const char *s);
char *current_iso_datetime(char *buf, size_t size);
char *relative_iso_datetime(char *buf, size_t size, char *relative);

#endif // DATETIME_H