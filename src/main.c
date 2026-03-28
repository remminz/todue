#include <stdio.h>
#include <time.h>

#include "sqlite/sqlite3.h"

#include "todue/cli.h"
#include "todue/datetime.h"
#include "todue/db.h"
#include "todue/log.h"
#include "todue/path.h"
#include "todue/util.h"

int main(int argc, char **argv) {
    // zero is possible if called as subprocess
    if (argc == 0 || argv[0] == NULL) {
        return 0;
    }

    // app checks and setup
    if (ensure_todue_dirs()) {
        fputs("Failed to ensure all app data directories\n", stderr);
        return 1;
    }

    char db_path[PATH_SIZE];
    if (todue_state_path(db_path, sizeof(db_path), TODUE_DB_FILE)) {
        fputs("Failed to get home database path\n", stderr);
        return 2;
    }

    if (log_init(LOG_DEBUG)) {
        LOG_WARN("Failed to open log file; defaulted to stderr");
    }

    int rc = 0;

    // open/create and setup database
    sqlite3 *db = db_setup(db_path);
    if (db == NULL) {
        fputs("Failed to open home database on program start\n", stderr);
        rc = 3;
        goto cleanup;
    }

    // run app
    if (argc == 1) {
        start_repl(&db);
    } else {
        // remove program name from args and clamp size to limit
        argc = min(argc - 1, CLI_ARGC_LIMIT);
        ++argv;
        execute_cmd(&db, argc, argv);
    }

cleanup:
    db_close(db);
    log_close();
    return rc;
}