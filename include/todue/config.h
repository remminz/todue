#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "todue/log.h"
#include "todue/platform.h"

typedef struct {
    bool create_on_load;
    size_t log_count;
    LogLevel log_level;
    char project_path[PATH_LIMIT]; // private, internal use only
    bool use_alt_screen;
    bool use_pager;
} Config;

extern Config g_config;

int config_set(const char *section, const char *name, const char *value);
void config_init(void);
void config_read(void);
int config_write(Config *conf);
void config_print(Config *conf, FILE *out);
bool project_exists(void);

#endif // CONFIG_H
