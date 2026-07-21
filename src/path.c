#include "todue/path.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "todue/config.h"
#include "todue/platform.h"
#include "todue/util.h"

static int ensure_dir(const char *dir) {
    int error = mkdir_p(dir);
    if (error && !dir_exists(dir)) {
        fprintf(stderr, "mkdir failed on %s: %s\n", dir, strerror(error));
        return -1;
    }
    return 0;
}

int ensure_todue_dirs(void) {
    int rc = 0;
    char *dir;

    dir = todue_state_dir();
    if (ensure_dir(dir)) {
        rc = -1;
    } else { // build subdirectories with this pattern
        char *logs = dir_join(dir, TODUE_LOG_DIR);
        if (ensure_dir(logs)) {
            rc = -1;
        }
        free(logs);
    }
    free(dir);

    dir = todue_config_dir();
    if (ensure_dir(dir)) {
        rc = -1;
    }
    free(dir);

    // Not in use yet
    /*
    dir = todue_cache_dir();
    if (ensure_dir(dir)) {
        rc = -1;
    }
    free(dir);
    */

    return rc;
}

static int todue_data_path(char *(*dir_fn)(void), char *buf,
                           size_t size, const char *name) {
    if (!name || *name == '\0') {
        return -1;
    }

#ifdef DEBUG
    (void)dir_fn;
    int written = snprintf(buf, size, "%s%c%s", APP_DOT_DIR, PATH_SEP, name);
    if (written < 0 || (size_t)written >= size) {
        return -1;
    }
    return 0;
#else
    char *base = dir_fn();
    if (!base) {
        return -1;
    }

    int written = snprintf(buf, size, "%s%c%s", base, PATH_SEP, name);
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

int todue_db_file(char *buf, size_t size) {
    if (project_exists()) {
        size_t written =
            snprintf(buf, size, "%s%c%s",
                     g_config.project_path, PATH_SEP, TODUE_DB_FILE);
        return (written < size ? 0 : -1);
    }
    return todue_state_path(buf, size, TODUE_DB_FILE);
}

int todue_config_file(char *buf, size_t size) {
    if (project_exists()) {
        size_t written =
            snprintf(buf, size, "%s%c%s",
                     g_config.project_path, PATH_SEP, TODUE_CONF_FILE);
        return (written < size ? 0 : -1);
    }
    return todue_config_path(buf, size, TODUE_CONF_FILE);
}
