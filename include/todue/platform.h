#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <time.h>

#ifdef _WIN32
    #define TODUE_WINDOWS 1
    #include <io.h>
    #define todue_popen _popen
    #define todue_pclose _pclose
#else
    #define TODUE_WINDOWS 0
    #include <unistd.h>
    #define todue_popen popen
    #define todue_pclose pclose
#endif

const char *todue_get_home(void);
int todue_mkdir(const char *path);
int todue_isatty_stdout(void);
const char *todue_get_pager(void);

// strdup wrapper since strdup has limited availability
char *todue_strdup(const char *str);

// cross-platform thread-safe localtime wrapper
struct tm *todue_localtime(const time_t *t, struct tm *result);

#endif // PLATFORM_H