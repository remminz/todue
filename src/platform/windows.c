#include "todue/platform.h"

#include <string.h>

static char *build_path(const char *env) {
#if defined(DEBUG)
    (void)env;
    return strdup(APP_DOT_DIR);
#elif defined(TODUE_WINDOWS)
    const char *base = getenv_nonempty(env);
    if (!base) {
        return NULL;
    }
    return dir_join(base, APP_NAME);
#endif    
}

char *todue_state_dir(void) {
    return build_path("LOCALAPPDATA");
}

char *todue_config_dir(void) {
    return build_path("APPDATA");
}

char *todue_cache_dir(void) {
    char *base = build_path("LOCALAPPDATA");
    if (!base) {
        return NULL;
    }
    char *out = dir_join(base, "Cache");
    free(base);
    return out;
}
