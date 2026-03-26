#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <time.h>

#define APP_NAME "todue"
#define APP_DOT_DIR "." APP_NAME

#ifdef _WIN32
    #define TODUE_WINDOWS
    #define popen _popen
    #define pclose _pclose
    #define strdup _strdup
#elif defined(__APPLE__)
    #define TODUE_MACOS
#elif defined(__linux__)
    #define TODUE_LINUX
#else
    #define TODUE_LINUX
    #warning "Platform not officially suported. Treating as linux."
#endif

char *todue_state_dir(void);
char *todue_config_dir(void);
char *todue_cache_dir(void);
int todue_mkdir(const char *path);
int todue_isatty_stdout(void);
const char *todue_get_pager(void);
bool dir_exists(const char *path);

// cross-platform thread-safe localtime wrapper
struct tm *todue_localtime(const time_t *t, struct tm *result);

#endif // PLATFORM_H