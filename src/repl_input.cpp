#include "todue/repl_input.h"

#include <string.h>

#include "linenoise/linenoise.h"

#include "todue/path.h"

#define HISTORY ".todue_history"
static char history_path[1024];

void repl_init() {
    linenoiseInstallWindowChangeHandler();
    linenoiseHistorySetMaxLen(1024);

    todue_path(history_path, sizeof(history_path), HISTORY);
    linenoiseHistoryLoad(history_path);
}

char *repl_readline(const char *prompt) {
    return linenoise(prompt);
}

void repl_add_history(const char *line) {
    if (line && *line) {
        linenoiseHistoryAdd(line);
    }
}

void repl_free() {
    linenoiseHistorySave(history_path);
    linenoiseHistoryFree();
}