#include "todue/platform.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "todue/log.h"
#include "todue/util.h"

static char *build_path(const char *path) {
#if defined(DEBUG)
    (void)path;
    return strdup(APP_DOT_DIR);
#else
    const char *env = getenv_nonempty("HOME");
    if (!env) {
        return NULL;
    }
    char *base = dir_join(env, path);
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#endif
}

char *todue_state_dir(void) {
    return build_path(".local/state");
}

char *todue_config_dir(void) {
    return build_path(".config");
}

char *todue_cache_dir(void) {
    return build_path(".cache");
}

typedef struct {
    char path[PATH_LIMIT];
    uint32_t timestamp;
} FileEntry;

static uint32_t pack_date(const char *iso_date) {
    static const size_t date_len = ARRAY_LEN("YYYY-MM-DD") - 1;

    if (strnlen(iso_date, date_len) < date_len || !isdigit(iso_date[0])) {
        return 0;
    }

    // pack YYYY-MM-DD simply into 32 bits for comparison
    return
        ((uint32_t)(iso_date[0] - '0') << 28) |
        ((uint32_t)(iso_date[1] - '0') << 24) |
        ((uint32_t)(iso_date[2] - '0') << 20) |
        ((uint32_t)(iso_date[3] - '0') << 16) |
        ((uint32_t)(iso_date[5] - '0') << 12) |
        ((uint32_t)(iso_date[6] - '0') << 8)  |
        ((uint32_t)(iso_date[8] - '0') << 4)  |
        (uint32_t)(iso_date[9] - '0');
}

static void insertion_sort(FileEntry *files, size_t count) {
    for (size_t i = 1; i < count; ++i) {
        FileEntry key = files[i];
        size_t j = i;

        while (j > 0 && files[j - 1].timestamp < key.timestamp) {
            files[j] = files[j - 1];
            --j;
        }

        files[j] = key;
    }
}

int rotate_logs(const char *dirpath, size_t keep_count) {
    if (!dirpath || keep_count == 0) {
        return -1;
    }

    const char file_prefix[] = APP_NAME "-";

    DIR *dir = opendir(dirpath);
    if (!dir) {
        LOG_ERROR("Failed to open dir in log rotation: %s", strerror(errno));
        return -1;
    }

    struct dirent *entry;
    FileEntry *files = NULL;
    size_t count = 0;
    size_t capacity = 0;

    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;

        // ignore files that don't start with file prefix
        // also covers "." and ".." cases
        if (strncmp(name, file_prefix, ARRAY_LEN(file_prefix) - 1)) {
            continue;
        }

        // skip directories
        if (entry->d_type == DT_DIR) {
            continue;
        }

        // move past file prefix
        uint32_t timestamp = pack_date(name + ARRAY_LEN(file_prefix) - 1);
        // skip invalid dates
        if (timestamp == 0) {
            continue;
        }

        char fullpath[PATH_LIMIT];
        snprintf(fullpath, ARRAY_LEN(fullpath),
                 "%s%c%s", dirpath, PATH_SEP, name);

        if (count == capacity) {
            capacity = capacity ? capacity * 2 : 16;
            FileEntry *new_files = realloc(files, capacity * sizeof(*files));

            if (!new_files) {
                LOG_ERROR("Failed realloc in log rotation");
                free(files);
                closedir(dir);
                return -1;
            }

            files = new_files;
        }

        strncpy(files[count].path, fullpath, PATH_LIMIT - 1);
        files[count].path[PATH_LIMIT - 1] = '\0';
        files[count].timestamp = timestamp;
        ++count;
    }

    closedir(dir);

    // Sort newest first
    insertion_sort(files, count);

    // Delete everything after keep_count
    for (size_t i = keep_count; i < count; ++i) {
        if (unlink(files[i].path) != 0) {
            LOG_ERROR("File delete failed for %s: %s",
                      files[i].path, strerror(errno));
        }
    }

    free(files);
    return 0;
}
