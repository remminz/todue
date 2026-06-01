#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    size_t log_count;
} Config;

extern Config g_config;

void config_init(void);
void config_read(void);
int config_create(void);
void config_print(Config *conf, FILE *out);

#endif // CONFIG_H
