#ifndef PATH_H
#define PATH_H

#include <stddef.h>

int get_todue_dir(char *buf, size_t size);
int ensure_todue_dir(void);

#ifdef __cplusplus
extern "C" {
#endif

int todue_path(char *buf, size_t size, const char *name);

#ifdef __cplusplus
}
#endif

#endif // PATH_H