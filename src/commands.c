#include "commands.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"
#include "log.h"
#include "util.h"

static int cmd_help(sqlite3 **db, int argc, char **argv) {
    (void)db;
    (void)argc;
    (void)argv;

    printf(
        "todo commands:\n"
        "  help            | Show this screen\n"
        "  load <db_path>  | Load a different database\n"
        "  add <brief>     | Add an item\n"
        "  rm <id>         | Remove an item\n"
        "  done <id>       | Mark an item as done\n"
        "  ls              | List the todues\n"
        "  quit            | Exit the CLI\n"
    );
    return 0;
}

static int cmd_load(sqlite3 **db, int argc, char **argv) {
    int rc = 0;
    char *old_path = xstrdup(sqlite3_db_filename(*db, "main"));

    if (old_path == NULL) {
        LOG_ERROR("Failed to save old path; can't continue without fallback");
        fprintf(stderr, "Encountered unexpected error");
        rc = -1;
    } else if (argc < 2) {
        LOG_ERROR("No arguments passed to load command");
        fprintf(stderr, "usage: todo load <path>\n");
        rc = -1;
    } else if (db_close(*db)) {
        fprintf(stderr, "Unable to close current db '%s'\n", sqlite3_db_filename(*db, "main"));
        rc = -1;
    } else if (db_open(db, argv[1])) {
        fprintf(stderr, "Unable to open new db '%s'\n", argv[1]);
        if (db_open(db, old_path)) { // attempt to reopen old database
            LOG_ERROR("Failed to reopen old db");
            fprintf(stderr, "Old db could not be reopened: try again or quit");
        }
        rc = -1;
    } else if (db_init(*db)) {
        fprintf(stderr, "Unable to initialize new db '%s'\n", argv[1]);
        rc = -1;
    }

    free(old_path);
    return rc;
}

static int cmd_add(sqlite3 **db, int argc, char **argv) {
    if (argc < 2) {
        LOG_ERROR("No arguments passed to add command");
        fprintf(stderr, "usage: todo add <id>\n");
        return -1;
    }
    if (db_add_todue(*db, argv[1])) {
        fprintf(stderr, "Failed to add item");
        return -1;
    }
    return 0;
}

static int cmd_remove(sqlite3 **db, int argc, char **argv) {
    if (argc < 2) {
        LOG_ERROR("No arguments passed to remove command");
        fprintf(stderr, "usage: todo rm <id>\n");
        return -1;
    }
    int id = strtoull(argv[1], NULL, 10);
    if (db_delete_todue(*db, id)) {
        fprintf(stderr, "Failed to remove item");
        return -1;
    }
    return 0;
}

static int cmd_done(sqlite3 **db, int argc, char **argv) {
    if (argc < 2) {
        LOG_ERROR("No arguments passed to done command");
        fprintf(stderr, "usage: todo done <id>\n");
        return -1;
    }
    int id = strtoull(argv[1], NULL, 10);
    if (db_mark_done(*db, id)) {
        fprintf(stderr, "Failed to mark item done");
        return -1;
    }
    return 0;
}

static int cmd_list(sqlite3 **db, int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (db_list(*db, print_row, NULL)) {
        fprintf(stderr, "Failed to list items");
        return -1;
    }
    return 0;
}

const Command commands[] = {
    {"help", cmd_help},
    {"load", cmd_load},
    {"add", cmd_add},
    {"rm", cmd_remove},
    {"done", cmd_done},
    {"ls", cmd_list},
};

int execute_cmd(sqlite3 **db, int argc, char **argv) {
    if (argc == 0 || argv == NULL) {
        return 0;
    }

    int rc = 0;
    size_t cmd_len = 0;
    for (int i = 0; i < argc; ++i) {
        cmd_len += strlen(argv[i]);
    }
    char *cmd = malloc(cmd_len * sizeof(*cmd));
    if (cmd == NULL) {
        rc = -1;
        goto cleanup;
    }
    cmd[0] = '\0';
    for (int i = 0; i < argc; ++i) {
        strcat(cmd, argv[i]);
        if (i < argc - 1) {
            strcat(cmd, " ");
        }
    }

    for (size_t i = 0; i < sizeof(commands) / sizeof(Command); ++i) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            LOG_DEBUG("Matched command '%s' from '%s'", argv[0], cmd);
            if (commands[i].func(db, argc, argv)) {
                LOG_ERROR("Non zero return code from '%s'", cmd);
            }
            goto cleanup;
        }
    }
    rc = -1;
    LOG_WARN("Invalid command '%s' from '%s'", argv[0], cmd);
    fprintf(stderr, "Invalid command '%s'\n", argv[0]);

cleanup:
    free(cmd);
    return rc;
}