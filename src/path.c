#include "todue/path.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "todue/platform.h"

#define TODUE_DIR ".todue"
#define PATH_SIZE 1024

int get_todue_dir(char *buf, size_t size) {
    const char *home = todue_get_home();

    if (home == NULL || buf == NULL || size <= 0)
        return -1;

    if (snprintf(buf, size, "%s/%s", home, TODUE_DIR) >= (int)size)
        return -1;

    return 0;
}

int ensure_todue_dir(void) {
#ifdef DEBUG
    return todue_mkdir(TODUE_DIR);
#else
    char path[PATH_SIZE];

    if (get_todue_dir(path, sizeof(path)))
        return -1;

    return todue_mkdir(path);
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