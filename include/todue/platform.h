#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <time.h>

#ifdef _WIN32
    #define TODUE_WINDOWS 1
    #include <io.h>
#else
    #define TODUE_WINDOWS 0
    #include <unistd.h>
#endif

#ifdef _WIN32
    #include <io.h>
    #define todue_popen _popen
    #define todue_pclose _pclose
#else
    #include <unistd.h>
    #define todue_popen popen
    #define todue_pclose pclose
#endif

const char *todue_get_home(void);
int todue_mkdir(const char *path);
bool todue_isatty_stdout(void);
const char *todue_get_pager(void);
char *todue_strdup(const char *str); // strdup wrapper since strdup has limited availability
struct tm *todue_localtime(const time_t *t, struct tm *result); // cross-platform thread-safe localtime wrapper

#endif // PLATFORM_H