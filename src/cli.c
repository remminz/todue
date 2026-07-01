#include "todue/cli.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "todue/config.h"
#include "todue/datetime.h"
#include "todue/log.h"
#include "todue/path.h"
#include "todue/platform.h"
#include "todue/repl_input.h"
#include "todue/util.h"

static void clear_screen(sqlite3 *db) {
    char time_str[ISO_DATETIME_SIZE];
    current_iso_datetime(time_str, sizeof(time_str));

    const char *path = (const char *)sqlite3_db_filename(db, "main");

    if (g_config.use_alt_screen) {
        printf("\033[2J\033[H");
        fflush(stdout);
    }

    printf("todue CLI %s (%s)\n", time_str, path);
}

static int parse_cmd(const char *line, int *argc, char ***argv) {
    *argc = 0;
    *argv = NULL;

    if (line == NULL || line[0] == '\0') {
        LOG_DEBUG("No line to parse");
        return 0;
    }

    int rc = 0;
    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        LOG_ERROR("Failed strdup allocation");
        rc = -1;
        goto cleanup;
    }
    char *token;
    char *rest = NULL;
    char buf[CLI_LINE_LIMIT] = "";

    int count = 0;
    char **args = malloc(1 * sizeof(*args));
    if (args == NULL) {
        LOG_ERROR("Failed malloc of argv");
        rc = -1;
        goto cleanup;
    }

    // put guaranteed command name into args
    args[count] = strdup(strtok_r(line_copy, " ", &rest));
    if (args[count++] == NULL) {
        LOG_ERROR("Failed strdup allocation");
        rc = -1;
        goto cleanup;
    }

    while ((token = strtok_r(NULL, " ", &rest))) {
        if (count > CLI_ARGC_LIMIT) {
            LOG_WARN("argc reached limit of %d", CLI_ARGC_LIMIT);
            break;
        }
        args = realloc(args, (count + 1) * sizeof(*args));
        if (args == NULL) {
            LOG_ERROR("Failed realloc of argv");
            rc = -1;
            goto cleanup;
        }

        if ((token[0] == '"' || token[0] == '\'') && (token[0] != token[1])) {
            const char quote = token[0];
            char *end = token + strlen(token) - 1;
            if (*end == '\0') {
                LOG_ERROR("Unmatched quote in args");
                rc = -1;
                goto cleanup;
            }

            ++token;
            while (*end != quote) {
                strcat(buf, token);
                strcat(buf, " ");
                token = strtok_r(NULL, " ", &rest);
                if (token == NULL) {
                    LOG_ERROR("Unmatched quote in args");
                    rc = -1;
                    goto cleanup;
                }
                end = token + strlen(token) - 1;
            }
            *end = '\0';
            strcat(buf, token);
            args[count] = strdup(buf);
            buf[0] = '\0';
        } else {
            args[count] = strdup(token);
        }
        if (args[count++] == NULL) {
            LOG_ERROR("Failed strdup allocation");
            rc = -1;
            goto cleanup;
        }
    }
    *argc = count;
    *argv = args;

cleanup:
    free(line_copy);
    return rc;
}

static void exit_alt_screen(void) {
    printf("\033[?1049l");
    fflush(stdout);
}

void start_repl(sqlite3 **db) {
    if (g_config.use_alt_screen && todue_isatty_stdout()) {
        printf("\033[?1049h");
        fflush(stdout);
        atexit(exit_alt_screen);
    }

    char history_path[PATH_MAX];
    // no need for error checking; no history if failed
    todue_state_path(history_path, sizeof(history_path), TODUE_HIST_FILE);

    repl_init(history_path);
    clear_screen(*db);

    int argc;
    char **argv;
    char *input;

    while ((input = repl_readline("todue> ")) != NULL) {
        if (input[0] == '\0') {
            free(input);
            continue;
        }
        if (!strcmp(input, "quit") || !strncmp(input, "q", 2)) {
            free(input);
            break;
        }

        repl_add_history(input);

        if (!strcmp(input, "clear") || !strcmp(input, "cls")) {
            clear_screen(*db);
        } else {
            if (parse_cmd(input, &argc, &argv)) {
                fprintf(stderr, "Failed to parse command\n");
                free(input);
                continue;
            }

            if (!execute_cmd(db, argc, argv)) {
                if (!strcmp(argv[0], "load") || !strncmp(argv[0], "l", 2)) {
                    clear_screen(*db);
                }
            }

            for (int i = 0; i < argc; ++i) {
                free(argv[i]);
            }
            free(argv);
        }
        free(input);
    }

    repl_shutdown(history_path);
}
