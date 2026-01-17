#include "todue/commands.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "todue/datetime.h"
#include "todue/db.h"
#include "todue/log.h"
#include "todue/platform.h"
#include "todue/util.h"

static int cmd_help(sqlite3 **db, int argc, char **argv) {
    (void)db;
    (void)argv;

    if (argc != 1) {
        LOG_WARN("help usage message triggered");
        fprintf(stderr, "usage: todue help\n");
        return -1;
    }

    printf(
        "todue commands:\n"
        "  help                                | Show this screen\n"
        "  load db_path                        | Load an existing database or create a new one\n"
        "  reload                              | Reload the current database\n"
        "  add brief [-n notes] [-d due_date]  | Add an item\n"
        "  rm {id... | --done | --all}         | Remove one or more items\n"
        "  done id                             | Mark an item as done\n"
        "  ls                                  | List all todues\n"
        "  quit                                | Exit the CLI\n"
    );
    return 0;
}

static int cmd_load(sqlite3 **db, int argc, char **argv) {
    int rc = 0;
    char *old_path = todue_strdup(sqlite3_db_filename(*db, "main"));

    if (old_path == NULL) {
        LOG_ERROR("Failed to save old path; can't continue without fallback");
        fprintf(stderr, "Encountered unexpected error\n");
        rc = -1;
    } else if (argc != 2) {
        LOG_WARN("load usage message triggered");
        fprintf(stderr, "usage: todue load db_path\n");
        rc = -1;
    } else if (db_close(*db)) {
        fprintf(stderr, "Unable to close current db '%s'\n", sqlite3_db_filename(*db, "main"));
        rc = -1;
    } else if (db_open(db, argv[1])) {
        fprintf(stderr, "Unable to open new db '%s'\n", argv[1]);
        if (db_open(db, old_path)) { // attempt to reopen old database
            LOG_ERROR("Failed to reopen old db");
            fprintf(stderr, "Old db could not be reopened: try again or quit\n");
        }
        rc = -1;
    } else if (db_init(*db)) {
        fprintf(stderr, "Unable to initialize new db '%s'\n", argv[1]);
        rc = -1;
    }

    free(old_path);
    return rc;
}

static int cmd_reload(sqlite3 **db, int argc, char **argv) {
    (void)argv;

    if (argc != 1) {
        LOG_WARN("reload usage message triggered");
        fprintf(stderr, "usage: todue reload\n");
        return -1;
    }

     if (db == NULL) {
        LOG_ERROR("Cannot reload null database");
        fprintf(stderr, "No db is open to reload\n");
        return -1;
    }

    int rc = 0;
    char *path = todue_strdup(sqlite3_db_filename(*db, "main"));

    if (db_close(*db)) {
        LOG_ERROR("Failed at close stage of reload");
        fprintf(stderr, "Failed to reload db\n");
        rc = -1;
    } else if (db_open(db, path)) {
        LOG_ERROR("Failed at open stage of reload");
        fprintf(stderr, "Failed to reload db\n");
        rc = -1;
    } else if (db_init(*db)) {
        LOG_ERROR("Failed at init stage of reload");
        fprintf(stderr, "Failed to reload db\n");
        rc = -1;
    }

    free(path);
    return rc;
}

static int cmd_add(sqlite3 **db, int argc, char **argv) {
    if (argc < 2 || argc == 3 || argc == 5 || argc > 6) {
        LOG_WARN("add usage message triggered");
        fprintf(
            stderr,
            "usage: todue add brief [<notes> <due_date>]\n"
            "       todue add brief [-n notes] [-d due_date]\n"
        );
        return -1;
    }
    char *brief = argv[1];
    if (brief[0] == '\0') {
        fprintf(stderr, "Brief cannot be empty\n");
        return -1;
    }
    char *notes = NULL;
    char *due = NULL;
    int rc = 0;

    for (int i = 2; i < 6 && i < argc; i += 2) {
        if (i + 1 < argc && strcmp(argv[i], "-n") == 0) {
            notes = argv[i + 1];
        } else if (i + 1 < argc && strcmp(argv[i], "-d") == 0) {
            due = malloc(sizeof(*due) * 20);
            if (due == NULL) {
                LOG_ERROR("Failed malloc in add command");
                rc = -1;
                goto cleanup;
            }
            if (relative_iso_datetime(due, 20, argv[i + 1]) == NULL) {
                LOG_ERROR("No valid parse from relative time string '%s'", argv[i + 1]);
                fprintf(stderr, "Failed to parse relative time string '%s'\n", argv[i + 1]);
                rc = -1;
                goto cleanup;
            }
        } else if (i == 2 && i + 1 < argc) {
            notes = argv[i];
            due = malloc(sizeof(*due) * 20);
            if (due == NULL) {
                LOG_ERROR("Failed malloc in add command");
                rc = -1;
                goto cleanup;
            }
            if (relative_iso_datetime(due, 20, argv[i + 1]) == NULL) {
                LOG_ERROR("No valid parse from relative time string '%s'", argv[i + 1]);
                fprintf(stderr, "Failed to parse relative time string '%s'\n", argv[i + 1]);
                rc = -1;
                goto cleanup;
            }
        } else {
            LOG_ERROR("One or more invalid arguments");
            fprintf(stderr, "One or more invalid arguments\n");
            rc = -1;
            goto cleanup;
        }
    }

    if (db_add_todue(*db, brief, notes, due)) {
        fprintf(stderr, "Failed to add item\n");
        check_table(*db);
        rc = -1;
        goto cleanup;
    }

cleanup:
    free(due);
    return rc;
}

static int cmd_edit(sqlite3 **db, int argc, char **argv) {
    if (argc < 4 || argc % 2 != 0 || argc > 8) {
        LOG_WARN("edit usage message triggered");
        fprintf(stderr, "usage: todue edit <id> [-b <brief>] [-n <notes>] [-d <due_date>]\n");
        return -1;
    }

    int rc = 0;
    unsigned long id = strtoul(argv[1], NULL, 10);
    if (id == 0) {
        LOG_ERROR("id is zero");
        return -1;
    }

    char *brief = NULL;
    char *notes = NULL;
    bool append = false;
    char *due = NULL;

    for (int i = 2; i < argc && i < 8; i += 2) {
        if (argv[i][0] != '-' || strnlen(argv[i], 2) < 2) {
            LOG_ERROR("Invalid argument '%s'", argv[i]);
            fprintf(stderr, "Invalid argument '%s'\n", argv[i]);
            rc = -1;
            goto cleanup;
        }
        switch (argv[i][1]) {
            case 'b':
                brief = argv[i + 1];
                break;
            case 'n':
                notes = argv[i + 1];
                if (argv[i][2] != '\0' && argv[i][2] != 'a') {
                    LOG_ERROR("Invalid argument '%s'", argv[i]);
                    fprintf(stderr, "Invalid argument '%s'\n", argv[i]);
                    rc = -1;
                    goto cleanup;
                }
                append = (argv[i][2] == 'a');
                break;
            case 'd':
                due = malloc(20 * sizeof(*due));
                if (due == NULL) {
                    LOG_ERROR("Failed malloc in edit");
                    rc = -1;
                    goto cleanup;
                }
                relative_iso_datetime(due, 20, argv[i + 1]);
                break;
        }
    }

    if (db_edit_todue(*db, id, brief, notes, append, due)) {
        LOG_ERROR("Failed to edit item");
        fprintf(stderr, "Failed to edit item\n");
        rc = -1;
    }

cleanup:
    free(due);
    return rc;
}

static int cmd_remove(sqlite3 **db, int argc, char **argv) {
    if (argc != 2) {
        LOG_WARN("remove usage message triggered");
        fprintf(
            stderr,
            "usage: todue rm {id... | --done | --all}\n"
            "ids may be specified as:\n"
            "  n           single id\n"
            "  n,m,k       comma-separated list\n"
            "  n-k         inclusive range\n"
            "Formats may be mixed (e.g. 1,3-5,8).\n"
        );
        return -1;
    }

    if (strcmp(argv[1], "--done") == 0) {
        if (db_delete_done(*db)) {
            fprintf(stderr, "Failed to remove done items\n");
            return -1;
        }
        return 0;
    } else if (strcmp(argv[1], "--all") == 0) {
        if (db_delete_all(*db)) {
            fprintf(stderr, "Failed to remove all items\n");
            return -1;
        }
        return 0;
    }

    char *end = NULL;
    unsigned long id = strtoul(argv[1], &end, 10);
    
    while (id != 0) {
        if (*end == '\0') {
            db_delete_todue(*db, id);
            return 0;
        }
        if (*end == ',') {
            if (db_delete_todue(*db, id)) {
                fprintf(stderr, "Failed to remove item\n");
                check_table(*db);
                return -1;
            }
        } else if (*end == '-') {
            ++end;
            if (db_delete_range(*db, id, strtoul(end, &end, 10))) {
                fprintf(stderr, "Failed to remove items\n");
                check_table(*db);
                return -1;
            }
        }
        if (*end == ',') {
            ++end;
            id = strtoul(end, &end, 10);
        } else if (*end != '\0') {
            LOG_ERROR("Invalid character '%c' in rm args", *end);
            fprintf(stderr, "Invalid character '%c' in command arguments\n", *end);
            return -1;
        }
    }
    return 0;
}

static int cmd_done(sqlite3 **db, int argc, char **argv) {
    if (argc != 2) {
        LOG_WARN("done usage message triggered");
        fprintf(
            stderr,
            "usage: todue done {id... | --all}\n"
            "ids may be specified as:\n"
            "  n           single id\n"
            "  n,m,k       comma-separated list\n"
            "  n-k         inclusive range\n"
            "Formats may be mixed (e.g. 1,3-5,8).\n"
        );
        return -1;
    }

    if (strcmp(argv[1], "--all") == 0) {
        if (db_mark_all_done(*db)) {
            fprintf(stderr, "Failed to mark all items done\n");
            return -1;
        }
        return 0;
    }
    
    char *end = NULL;
    unsigned long id = strtoul(argv[1], &end, 10);
    
    while (id != 0) {
        if (*end == '\0') {
            db_mark_done(*db, id);
            return 0;
        }
        if (*end == ',') {
            if (db_mark_done(*db, id)) {
                fprintf(stderr, "Failed to mark item done\n");
                check_table(*db);
                return -1;
            }
        } else if (*end == '-') {
            ++end;
            if (db_mark_range_done(*db, id, strtoul(end, &end, 10))) {
                fprintf(stderr, "Failed to mark items done\n");
                check_table(*db);
                return -1;
            }
        }
        if (*end == ',') {
            ++end;
            id = strtoul(end, &end, 10);
        } else if (*end != '\0') {
            LOG_ERROR("Invalid character '%c' in done args", *end);
            fprintf(stderr, "Invalid character '%c' in command arguments\n", *end);
            return -1;
        }
    }
    return 0;
}

static int cmd_list(sqlite3 **db, int argc, char **argv) {
    (void)argv;

    if (argc != 1) {
        LOG_WARN("list usage message triggered");
        fprintf(stderr, "usage: todue ls\n");
        return -1;
    }

    FILE *out = openPager();
    if (db_list(*db, print_row, out)) {
        fprintf(stderr, "Failed to list items\n");
        check_table(*db);
        closePager(out);
        return -1;
    }
    closePager(out);
    return 0;
}

const Command commands[] = {
    {"help", cmd_help},
    {"load", cmd_load},
    {"reload", cmd_reload},
    {"add", cmd_add},
    {"edit", cmd_edit},
    {"rm", cmd_remove},
    {"done", cmd_done},
    {"ls", cmd_list},
};

int execute_cmd(sqlite3 **db, int argc, char **argv) {
    if (argc == 0 || argv == NULL) {
        return 0;
    }

    int rc = 0;
    size_t cmd_len = 1 + strlen(argv[0]); // null terminator + command
    for (int i = 1; i < argc; ++i) {
        cmd_len += strlen(argv[i]) + 3; // +3 for surrounding quotes and a preceding space
    }
    char *cmd = malloc(cmd_len * sizeof(*cmd));
    if (cmd == NULL) {
        rc = -1;
        goto cleanup;
    }
    cmd[0] = '\0';
    strcat(cmd, argv[0]);
    for (int i = 1; i < argc; ++i) {
        strcat(cmd, " \"");
        strcat(cmd, argv[i]);
        strcat(cmd, "\"");
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