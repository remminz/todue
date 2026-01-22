#include "todue/repl_input.h"

#include <string.h>

#include "linenoise/linenoise.h"

void repl_init(const char *history_path) {
    linenoiseInstallWindowChangeHandler();

    if (history_path && history_path[0] != '\0') {
        linenoiseHistoryLoad(history_path);
    }
}

char *repl_readline(const char *prompt) {
    return linenoise(prompt);
}

void repl_add_history(const char *line) {
    if (line && line[0] != '\0') {
        linenoiseHistoryAdd(line);
    }
}

void repl_shutdown(const char *history_path) {
    if (history_path && history_path[0] != '\0') {
        linenoiseHistorySave(history_path);
    }

    linenoiseHistoryFree();
}