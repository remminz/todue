#include "todue/platform.h"

#include <string.h>

static char *build_path(const char *path) {
#if defined(DEBUG)
    (void)path;
    return strdup(APP_DOT_DIR);
#elif defined(TODUE_MACOS)
    const char *base = getenv_nonempty("HOME");
    if (!base) {
        return NULL;
    }
    base = dir_join(base, path);
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, APP_NAME);
    free(base);
    return out;
#endif
}

char *todue_state_dir(void) {
    return build_path(".local/state/");
}

char *todue_config_dir(void) {
    return build_path(".config/");
}

char *todue_cache_dir(void) {
    return build_path(".cache/");
}
