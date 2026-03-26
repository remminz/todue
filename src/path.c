#include "todue/path.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "todue/platform.h"

int ensure_todue_dirs(void) {
#ifdef DEBUG
    if (!dir_exists(APP_DOT_DIR)) {
        return todue_mkdir(APP_DOT_DIR);
    }
    return 0;
#else
    int rc = 0;
    char *dir;

    dir = todue_state_dir();
    if (!dir_exists(dir)) {
        if (todue_mkdir(dir)) {
            rc = -1;
        }
    }
    free(dir);

    // Not in use yet
    /*
    dir = todue_config_dir();
    if (!dir_exists(dir)) {
        if (todue_mkdir(dir)) {
            rc = -1;
        }
    }
    free(dir);

    dir = todue_cache_dir();
    if (!dir_exists(dir)) {
        if (todue_mkdir(dir)) {
            rc = -1;
        }
    }
    free(dir);
    */
    return rc;
#endif
}

static int todue_data_path(char *(*dir_fn)(void), char *buf,
                           size_t size, const char *name) {
    if (!name || *name == '\0') {
        return -1;
    }

#ifdef DEBUG
    (void)dir_fn;
    int written = snprintf(buf, size, "%s/%s", APP_DOT_DIR, name);
    if (written < 0 || (size_t)written >= size) {
        return -1;
    }
    return 0;
#else
    char *base = dir_fn();
    if (!base) {
        return -1;
    }

    int written = snprintf(buf, size, "%s/%s", base, name);
    free(base);

    if (written < 0 || (size_t)written >= size) {
        return -1;
    }
    return 0;
#endif
}

int todue_state_path(char *buf, size_t size, const char *name) {
    return todue_data_path(todue_state_dir, buf, size, name);
}

int todue_config_path(char *buf, size_t size, const char *name) {
    return todue_data_path(todue_config_dir, buf, size, name);
}

int todue_cache_path(char *buf, size_t size, const char *name) {
    return todue_data_path(todue_cache_dir, buf, size, name);
}