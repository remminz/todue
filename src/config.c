#include "todue/config.h"

#include <stdlib.h>
#include <string.h>

#include "inih/ini.h"

#include "todue/path.h"
#include "todue/platform.h"

Config g_config;

static void set_defaults(void) {
    g_config.log_count = 7;
}

static int config_handler(void *user, const char *section,
                          const char *name, const char *value) {
    (void)user;

    if (!strcmp(section, "")) {
        if (!strcmp(name, "log_count")) {
            g_config.log_count = atol(value);
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
    set_defaults();
    config_read();
}

void config_read(void) {
    char ini[PATH_LIMIT];
    if (todue_config_path(ini, ARRAY_LEN(ini), TODUE_CONF_FILE)) {
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
