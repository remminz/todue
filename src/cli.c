#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "datetime.h"
#include "log.h"
#include "util.h"

static char *info_str(sqlite3 *db) {
    char time_str[20];
    current_iso_datetime(time_str, sizeof(time_str));
    
    const char* path = sqlite3_db_filename(db, "main");
    char *str = malloc(sizeof(char) * 256);

    snprintf(str, 256, "todue CLI %s (%s)", time_str, path);
    return str;
}

static int parse_cmd(const char *line, int *argc, char ***argv) {
    *argc = 0;
    *argv = NULL;

    if (line == NULL || line[0] == '\0') {
        LOG_DEBUG("No line to parse");
        return 0;
    }

    int rc = 0;
    char *line_copy = xstrdup(line);
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
        goto cleanup;
        rc = -1;
    }
    args[count] = xstrdup(strtok_r(line_copy, " ", &rest)); // put gauranteed command name into args
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
            goto cleanup;
            rc = -1;
        }
        
        if (token[0] == '"' || token[0] == '\'') {
            char quote = token[0];
            char *end = token + strlen(token) - 1;
            ++token;
            
            while (*end != quote) {
                strcat(buf, token);
                strcat(buf, " ");
                token = strtok_r(NULL, " ", &rest);
                if (token == NULL) {
                    LOG_ERROR("Unmatched quote in args");
                    goto cleanup;
                    rc = -1;
                }
                end = token + strlen(token) - 1;
            }
            *end = '\0';
            strcat(buf, token);
            args[count] = xstrdup(buf);
        } else {
            args[count] = xstrdup(token);
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

void start_repl(sqlite3 **db) {
    char *info = info_str(*db);
    puts(info);
    free(info);

    int argc;
    char **argv;
    
    char *input = malloc(sizeof(char) * CLI_LINE_LIMIT);
    fputs("todue> ", stdout);
    while (fgets(input, CLI_LINE_LIMIT, stdin)) {
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "quit") == 0) {
            break;
        }

        parse_cmd(input, &argc, &argv);
        execute_cmd(db, argc, argv);

        for (int i = 0; i < argc; ++i) {
            free(argv[i]);
        }
        free(argv);
        
        fputs("todue> ", stdout);
    }
    
    free(input);
}