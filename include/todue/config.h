#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
    bool create_on_load;
    size_t log_count;
    bool use_alt_screen;
    bool use_pager;
} Config;

extern Config g_config;

void config_init(void);
void config_read(void);
int config_create(void);
void config_print(Config *conf, FILE *out);

#endif // CONFIG_H
