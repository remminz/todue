#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

typedef struct {
    size_t log_count;
} Config;

extern Config g_config;

void config_init(void);
void config_read(void);

#endif // CONFIG_H
