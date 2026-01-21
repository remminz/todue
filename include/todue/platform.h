#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <time.h>

#ifdef _WIN32
    #define TODUE_WINDOWS 1
    #define popen _popen
    #define pclose _pclose
    #define strdup _strdup
#else
    #define TODUE_WINDOWS 0
#endif

const char *todue_get_home(void);
int todue_mkdir(const char *path);
int todue_isatty_stdout(void);
const char *todue_get_pager(void);

// cross-platform thread-safe localtime wrapper
struct tm *todue_localtime(const time_t *t, struct tm *result);

#endif // PLATFORM_H