#include "todue/config.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "inih/ini.h"

#include "todue/path.h"
#include "todue/platform.h"

#define INVALID_BOOL(s) \
    (strcmp(s, "true") && \
     strcmp(s, "1") && \
     strcmp(s, "false") && \
     strcmp(s, "0"))
#define STRING_TO_BOOL(s) \
    (!strcmp(s, "true") || \
     !strcmp(s, "1"))
#define BOOL_TO_STRING(x) ((x) ? "true" : "false")

Config g_config;

static void set_defaults(Config *conf) {
    if (!conf) {
        return;
    }

    conf->create_on_load = true;
    conf->log_count = 7;
    conf->log_level = LOG_WARN;
    find_todue_home(conf->project_path, ARRAY_LEN(conf->project_path));
    conf->use_alt_screen = false;
    conf->use_pager = true;
}

static int parse_log_level(const char *value, LogLevel *level) {
    if (!strcasecmp(value, "none")) {
        *level = LOG_NONE;
    } else if (!strcasecmp(value, "error")) {
        *level = LOG_ERROR;
    } else if (!strcasecmp(value, "warn") ||
                !strcasecmp(value, "warning")) {
        *level = LOG_WARN;
    } else if (!strcasecmp(value, "info")) {
        *level = LOG_INFO;
    } else if (!strcasecmp(value, "debug")) {
        *level = LOG_DEBUG;
    } else {
        return 0;
    }
    return 1;
}
static int config_handler(void *user, const char *section,
                          const char *name, const char *value) {
    (void)user;
    return config_set(section, name, value);
}

int config_set(const char *section, const char *name, const char *value) {
    if (!strcmp(section, "")) {
        if (!strcmp(name, "create_on_load")) {
            if (INVALID_BOOL(value)) {
                return 0;
            }
            g_config.create_on_load = STRING_TO_BOOL(value);
        } else if (!strcmp(name, "log_count")) {
            long val = atol(value);
            if (val == 0) {
                return 0;
            }
            g_config.log_count = atol(value);
        } else if (!strcmp(name, "log_level")) {
            return parse_log_level(value, &g_config.log_level);
        } else if (!strcmp(name, "use_alt_screen")) {
            if (INVALID_BOOL(value)) {
                return 0;
            }
            g_config.use_alt_screen = STRING_TO_BOOL(value);
        } else if (!strcmp(name, "use_pager")) {
            if (INVALID_BOOL(value)) {
                return 0;
            }
            g_config.use_pager = STRING_TO_BOOL(value);
        } else {
            fprintf(stderr, "Unrecognized config option '%s' in '%s' section\n",
                    name, section);
            return 0;
        }
    } else {
        fprintf(stderr, "Unrecognized config option '%s' in '%s' section\n",
                name, section);
        return 0;
    }

    return 1; // success
}

void config_init(void) {
    set_defaults(&g_config);
    config_read();
}

void config_read(void) {
    char ini[PATH_LIMIT];

    if (project_exists()) {
        if (todue_config_file(ini, ARRAY_LEN(ini))) {
            return;
        }
    } else if (todue_config_path(ini, ARRAY_LEN(ini), TODUE_CONF_FILE)) {
        fprintf(stderr, "Unexpected error before reading config\n");
        return;
    }

    int rc = ini_parse(ini, config_handler, NULL);
    switch(rc) {
        case -2:
            fprintf(stderr, "Config memory error\n");
            break;
        case -1:
            fprintf(stderr, "Failed to open config file\n");
            break;
        case 0:
            break;
        default:
            fprintf(stderr, "Config error at line %d\n", rc);
    }
}

int config_write(Config *conf) {
    char path[PATH_LIMIT];

    if (project_exists()) {
        if (todue_config_file(path, ARRAY_LEN(path))) {
            return -1;
        }
    } else if (todue_config_path(path, ARRAY_LEN(path), TODUE_CONF_FILE)) {
        fprintf(stderr, "Unexpected error creating config\n");
        return -1;
    }

    FILE *fp = fopen(path, "w");
    if (!fp) {
        fprintf(stderr, "Failed opening config file: %s\n", strerror(errno));
        return -1;
    }

    if (!conf) {
        Config defaults;
        set_defaults(&defaults);
        config_print(&defaults, fp);
    } else {
        config_print(conf, fp);
    }

    fclose(fp);
    return 0;
}

static const char *log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_NONE:  return "none";
        case LOG_ERROR: return "error";
        case LOG_WARN:  return "warn";
        case LOG_INFO:  return "info";
        case LOG_DEBUG: return "debug";
        default:        return "warn";
    }
}

void config_print(Config *conf, FILE *out) {
    if (!conf || !out) {
        return;
    }

    fprintf(out,
            "create_on_load=%s\n"
            "log_count=%zu\n"
            "log_level=%s\n"
            "use_alt_screen=%s\n"
            "use_pager=%s\n",
            BOOL_TO_STRING(conf->create_on_load),
            conf->log_count,
            log_level_to_string(conf->log_level),
            BOOL_TO_STRING(conf->use_alt_screen),
            BOOL_TO_STRING(conf->use_pager));
}

bool project_exists(void) {
    return g_config.project_path[0] != '\0';
}
