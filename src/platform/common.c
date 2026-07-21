#include "todue/platform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef TODUE_WINDOWS
    #include <ctype.h>
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
    snprintf(str, len, "%s%c%s", s1, PATH_SEP, s2);
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

int mkdir_p(const char *path) {
    if (!path) {
        return -1;
    }

    int error;
    char temp[PATH_LIMIT];

    if (strlen(path) >= ARRAY_LEN(temp)) {
        return -1;
    }
    strcpy(temp, path);

    for (char *p = temp + 1; *p; ++p) {
        if (*p == PATH_SEP) {
            *p = '\0';

            error = todue_mkdir(temp);
            if (error && error != EEXIST) {
                return error;
            }

            *p = PATH_SEP;
        }
    }

    error = todue_mkdir(temp);
    if (error && error != EEXIST) {
        return error;
    }

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

int find_todue_home(char *path, size_t size) {
    if (!path || size == 0) {
        return -1;
    }
    path[0] = '\0';

    // get current directory
    char cwd[PATH_LIMIT];
    if (!getcwd(cwd, ARRAY_LEN(cwd))) {
        perror("error getting cwd");
        return -1;
    }

    // walk up directories to search for dot dir
    char project[ARRAY_LEN(cwd) + ARRAY_LEN(APP_DOT_DIR) + 1];
    const char *const root = strchr(cwd, PATH_SEP);
    char *sep = NULL;

    while (sep != root) {
        int project_rc = snprintf(project, ARRAY_LEN(project),
                          "%s%c%s", cwd, PATH_SEP, APP_DOT_DIR);

        if (project_rc < 0 || ARRAY_LEN(project) < (size_t)project_rc) {
            return -1;
        }

        if (dir_exists(project)) {
            int path_rc = snprintf(path, size, "%s", project);
            if (path_rc < 0 || size < (size_t)path_rc) {
                path[0] = '\0';
                return -1;
            }
            return 0;
        }

        sep = strrchr(cwd, PATH_SEP);
        *sep = '\0';
    }

    return 0;
}
