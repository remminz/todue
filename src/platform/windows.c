#include "todue/platform.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <Windows.h>

#include "todue/log.h"
#include "todue/util.h"

static char *build_path(const char *env) {
#if defined(DEBUG)
    (void)env;
    return strdup(APP_DOT_DIR);
#else
    const char *base = getenv_nonempty(env);
    if (!base) {
        return NULL;
    }
    return dir_join(base, APP_NAME);
#endif
}

char *todue_state_dir(void) {
    return build_path("LOCALAPPDATA");
}

char *todue_config_dir(void) {
    return build_path("APPDATA");
}

char *todue_cache_dir(void) {
    char *base = build_path("LOCALAPPDATA");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, "Cache");
    free(base);
    return out;
}

typedef struct {
    wchar_t path[PATH_LIMIT];
    uint32_t timestamp;
} FileEntry;

static uint32_t pack_date(const wchar_t *iso_date) {
    static const size_t date_len = ARRAY_LEN(L"YYYY-MM-DD") - 1;

    if (wcsnlen(iso_date, date_len) < date_len || !isdigit(iso_date[0])) {
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

static wchar_t *utf8_to_wide(const char *s) {
    int len = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (len <= 0) {
        return NULL;
    }

    wchar_t *buf = malloc(len * sizeof(wchar_t));
    if (!buf) {
        return NULL;
    }

    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, len);
    return buf;
}

int rotate_logs(const char *dirpath, size_t keep_count) {
    if (!dirpath || keep_count == 0) {
        return -1;
    }

    wchar_t *wdirpath = utf8_to_wide(dirpath);
    if (!wdirpath) {
        return -1;
    }

    const wchar_t file_prefix[] = L"" APP_NAME "-";
    wchar_t search_path[PATH_LIMIT];

    _snwprintf(search_path, ARRAY_LEN(search_path),
               L"%ls%c*", wdirpath, PATH_SEP);

    WIN32_FIND_DATAW find_data;

    HANDLE hFind = FindFirstFileW(search_path, &find_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        fwprintf(stderr, L"FindFirstFileW failed (%lu)\n", GetLastError());
        free(wdirpath);
        return -1;
    }

    FileEntry *files = NULL;
    size_t count = 0;
    size_t capacity = 0;

    do {
        const wchar_t *name = find_data.cFileName;

        // ignore files that don't start with file prefix
        // also covers "." and ".." cases
        if (wcsncmp(name, file_prefix, ARRAY_LEN(file_prefix) - 1)) {
            continue;
        }

        // skip directories
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        // move past file prefix
        uint32_t timestamp = pack_date(name + ARRAY_LEN(file_prefix) - 1);
        // skip invalid dates
        if (timestamp == 0) {
            continue;
        }

        wchar_t fullpath[PATH_LIMIT];
        _snwprintf(fullpath, ARRAY_LEN(fullpath),
                   L"%ls%c%ls", wdirpath, PATH_SEP, name);

        if (count == capacity) {
            capacity = capacity ? capacity * 2 : 16;
            FileEntry *new_files = realloc(files, capacity * sizeof(*files));

            if (!new_files) {
                LOG_ERROR("Failed realloc in log rotation");
                free(wdirpath);
                free(files);
                FindClose(hFind);
                return -1;
            }

            files = new_files;
        }

        wcsncpy(files[count].path, fullpath, PATH_LIMIT - 1);
        files[count].path[PATH_LIMIT - 1] = L'\0';
        files[count].timestamp = timestamp;
        ++count;
    } while (FindNextFileW(hFind, &find_data));

    FindClose(hFind);

    // Sort newest first
    insertion_sort(files, count);

    // Delete everything after keep_count
    for (size_t i = keep_count; i < count; ++i) {
        if (!DeleteFileW(files[i].path)) {
            fwprintf(stderr,
                     L"File delete failed for %ls (%lu)\n",
                     files[i].path, GetLastError());
        }
    }

    free(wdirpath);
    free(files);
    return 0;
}
