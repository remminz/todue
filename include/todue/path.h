#ifndef PATH_H
#define PATH_H

#include <stddef.h>

#define TODUE_DB_FILE   "todue.db"
#define TODUE_LOG_DIR   "logs"
#define TODUE_HIST_FILE ".history"

int ensure_todue_dirs(void);
int todue_state_path(char *buf, size_t size, const char *name);
int todue_config_path(char *buf, size_t size, const char *name);
int todue_cache_path(char *buf, size_t size, const char *name);

#endif // PATH_H
