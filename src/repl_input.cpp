#include "todue/repl_input.h"

#include <cstddef>
#include <cstring>

#include "linenoise/linenoise.h"

extern "C" {
    #include "todue/commands.h"
}

static void completion_hook(const char *prefix, linenoiseCompletions* lc) {
    for (std::size_t i = 0; i < commands_len; ++i) {
        if (!strncmp(prefix, commands[i].aliases[0], strlen(prefix))) {
           linenoiseAddCompletion(lc, commands[i].aliases[0]);
        }
    }
}

void repl_init(const char *history_path) {
    linenoiseInstallWindowChangeHandler();
    linenoiseSetCompletionCallback(completion_hook);

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
