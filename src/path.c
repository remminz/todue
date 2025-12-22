#include "path.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#define TODUE_DIR ".todue"
#define PATH_SIZE 1024

static int create_dir(const char *path) {
#ifdef _WIN32
    if (_mkdir(path) == 0)
        return 0;
    if (errno == EEXIST)
        return 0;
#else
    if (mkdir(path, 0700) == 0)
        return 0;
    if (errno == EEXIST)
        return 0;
#endif
    return -1;
}

int get_todue_dir(char *buf, size_t size) {
    const char *home;

#ifdef DEBUG
    home = ".";
#elif defined(_WIN32)
    home = getenv("USERPROFILE");
#else
    home = getenv("HOME");
#endif

    if (home == NULL || buf == NULL || size <= 0)
        return -1;

    if (snprintf(buf, size, "%s/%s", home, TODUE_DIR) >= (int)size)
        return -1;

    return 0;
}

int ensure_todue_dir(void) {
#ifdef DEBUG
    return create_dir(TODUE_DIR);
#else
    char path[PATH_SIZE];

    if (get_todue_dir(path, sizeof(path)))
        return -1;

    return create_dir(path);
#endif
}

int todue_path(char *buf, size_t size, const char *name) {
    char base[PATH_SIZE];

    if (!name || name[0] == '\0')
        return -1;

    if (get_todue_dir(base, sizeof(base)) != 0)
        return -1;

    if (snprintf(buf, size, "%s/%s", base, name) >= (int)size)
        return -1;

    return 0;
}