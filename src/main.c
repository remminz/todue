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
    if (argc == 0 || argv[0] == NULL) {
        return 0;
    }

    ensure_todue_dir();

    char log_path[1024];
    if (todue_path(log_path, sizeof(log_path), "debug.log")) {
        return -1;
    }

    char db_path[1024];
    if (todue_path(db_path, sizeof(db_path), "todue.db")) {
        return -1;
    }

    int rc = 0;
    log_set_level(LOG_DEBUG);
    log_set_file(log_path);
    
    sqlite3 *db = NULL;
    db_open(&db, db_path);
    db_init(db);
    if (db == NULL) {
        LOG_ERROR("Failed to open db on program start");
        fprintf(stderr, "Failed to open db on program start\n");
        rc = -1;
        goto cleanup;
    }

    if (argc == 1) {
        start_repl(&db);
    } else {
        argc = argc > CLI_ARGC_LIMIT ? CLI_ARGC_LIMIT : argc - 1;
        ++argv;
        execute_cmd(&db, argc, argv);
    }

cleanup:
    db_close(db);
    log_close();
    return rc;
}