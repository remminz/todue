#include "todue/platform.h"

#include <string.h>

#ifndef DEBUG
static char *xdg_dir(const char *xdg, const char *fallback) {
    const char *dir = getenv_nonempty(xdg);
    if (!dir) {
        dir = getenv_nonempty("HOME");
        if (!dir) {
            return NULL;
        }
        return dir_join(dir, fallback);
    }
    return strdup(dir);
}
#endif

static char *build_path(const char *xdg, const char* fallback) {
#if defined(DEBUG)
    (void)xdg;
    (void)fallback;
    return strdup(APP_DOT_DIR);
#elif defined(TODUE_LINUX)
    char *base = xdg_dir(xdg, fallback);
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#endif
}

char *todue_state_dir(void) {
    return build_path("XDG_STATE_HOME", ".local/state");
}

char *todue_config_dir(void) {
    return build_path("XDG_CONFIG_HOME", ".config");
}

char *todue_cache_dir(void) {
    return build_path("XDG_CACHE_HOME", ".cache");
}
