#include "todue/platform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef TODUE_WINDOWS
    #include <direct.h>
#else
    #include <unistd.h>
#endif

bool dir_exists(const char *path) {
#ifdef TODUE_WINDOWS
    struct _stat st;
    if (_stat(path, &st) != 0) {
        return false;
    }
    return (st.st_mode & _S_IFDIR) != 0;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
#endif
}

char *dir_join(const char *s1, const char *s2) {
    size_t len = strlen(s1) + strlen(s2) + 2; // 2 = '/' + '\0'
    char *str = malloc(sizeof(*str) * len);
    if (!str) {
        return NULL;
    }
    snprintf(str, len, "%s/%s", s1, s2);
    return str;
}

const char *getenv_nonempty(const char *name) {
    const char *env = getenv(name);
    return (env && *env) ? env : NULL;
}

int todue_mkdir(const char *path) {
#if defined(TODUE_WINDOWS)
    if (_mkdir(path)) {
        return errno;
    }
#else
    if (mkdir(path, 0700)) {
        return errno;
    }
#endif
    return 0;
}

int todue_isatty_stdout(void) {
#if defined(TODUE_WINDOWS)
    return _isatty(_fileno(stdout));
#else
    return isatty(STDOUT_FILENO);
#endif
}

const char *todue_get_pager(void) {
    const char *pager = getenv_nonempty("PAGER");
#if defined(TODUE_WINDOWS)
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

#if defined(TODUE_WINDOWS)
    if (localtime_s(result, t)) {
        return NULL;
    }
    return result;
#else
    return localtime_r(t, result);
#endif
}
