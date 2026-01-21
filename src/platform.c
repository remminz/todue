#include "todue/platform.h"

#include <stdlib.h>
#include <string.h>

#if TODUE_WINDOWS
    #include <direct.h>
    #include <stdio.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

const char *todue_get_home(void) {
    const char *home;

#if DEBUG
    home = ".";
#elif TODUE_WINDOWS
    home = getenv("USERPROFILE");
#else
    home = getenv("HOME");
#endif

    return home;
}

int todue_mkdir(const char *path) {
#if TODUE_WINDOWS
    if (_mkdir(path) == 0) {
        return 0;
    }
#else
    if (mkdir(path, 0700) == 0) {
        return 0;
    }
#endif

    return -1;
}

int todue_isatty_stdout(void) {
#if TODUE_WINDOWS
    return _isatty(_fileno(stdout));
#else
    return isatty(STDOUT_FILENO);
#endif
}

const char *todue_get_pager(void) {
    const char *pager = getenv("PAGER");

#if TODUE_WINDOWS
    if (!pager) {
        pager = "more";
    }
#else
    if (!pager) {
        pager = "less -FRX";
    }
#endif

    return pager;
}

struct tm *todue_localtime(const time_t *t, struct tm *result) {
    if (t == NULL || result == NULL) {
        return NULL;
    }

#if TODUE_WINDOWS
    if (localtime_s(result, t)) {
        return NULL;
    }
    return result;
#else
    return localtime_r(t, result);
#endif
}