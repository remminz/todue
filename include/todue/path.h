#ifndef PATH_H
#define PATH_H

#include <stddef.h>

#define TODUE_DIR_NAME   ".todue"
#define TODUE_DB_FILE    "todue.db"
#define TODUE_LOG_FILE   "debug.log"
#define TODUE_HIST_FILE  ".history"
#define PATH_SIZE 1024

int get_todue_dir(char *buf, size_t size);
int ensure_todue_dir(void);

#ifdef __cplusplus
extern "C" {
#endif

int todue_path(char *buf, size_t size, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // PATH_H