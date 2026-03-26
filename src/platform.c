#include "todue/platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TODUE_WINDOWS
    #include <direct.h>
    #include <stdio.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#ifndef DEBUG
static char *dir_join(const char *s1, const char *s2) {
    size_t len = strlen(s1) + strlen(s2) + 2; // 2 = '/' + '\0'
    char *str = malloc(sizeof(*str) * len);
    if (!str) {
        return NULL;
    }
    snprintf(str, len, "%s/%s", s1, s2);
    return str;
}
#endif

static const char *getenv_nonempty(const char *name) {
    const char *env = getenv(name);
    return (env && *env) ? env : NULL;
}

#ifdef TODUE_LINUX
static char *xdg_dir(const char *xdg, const char *fallback) {
    const char *dir = getenv_nonempty(name);
    if (!dir) {
        home = getenv_nonempty("HOME");
        if (!home) {
            return NULL;
        }
        return dir_join(home, fallback);
    }
    return strdup(dir);
}
#endif

char *todue_state_dir(void) {
#if defined(DEBUG)
    return strdup(APP_DOT_DIR);
#elif defined(TODUE_WINDOWS)
    const char *base = getenv_nonempty("LOCALAPPDATA");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#elif defined(TODUE_MACOS)
    const char *home = getenv_nonempty("HOME");
    if (!home) {
        return NULL;
    }
    char *out = dir_join(home, APP_DOT_DIR);
    return out;
#elif defined(TODUE_LINUX)
    char *base = xdg_dir("XDG_STATE_HOME", ".local/state");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#endif
}

char *todue_config_dir(void) {
#if defined(DEBUG)
    return strdup(APP_DOT_DIR);
#elif defined(TODUE_WINDOWS)
    const char *base = getenv_nonempty("APPDATA");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#elif defined(TODUE_MACOS)
    const char *home = getenv_nonempty("HOME");
    if (!home) {
        return NULL;
    }
    char *out = dir_join(home, APP_DOT_DIR "/config");
    return out;
#elif defined(TODUE_LINUX)
    char *base = xdg_dir("XDG_CONFIG_HOME", ".config");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#endif
}

char *todue_cache_dir(void) {
#if defined(DEBUG)
    return strdup(APP_DOT_DIR);
#elif defined(TODUE_WINDOWS)
    const char *base = getenv_nonempty("LOCALAPPDATA");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#elif defined(TODUE_MACOS)
    const char *home = getenv_nonempty("HOME");
    if (!home) {
        return NULL;
    }
    char *out = dir_join(home, APP_DOT_DIR "/cache");
    return out;
#elif defined(TODUE_LINUX)
    char *base = xdg_dir("XDG_CACHE_HOME", ".cache");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#endif
}

int todue_mkdir(const char *path) {
#if defined(TODUE_WINDOWS)
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

bool dir_exists(const char *path) {
#ifdef TODUE_WINDOWS
    struct _stat st;
    if (_stat(path, &st) != 0) {
        return 0;
    }
    return (st.st_mode & _S_IFDIR) != 0;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
#endif
}