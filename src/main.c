#include <stdio.h>
#include <time.h>

#include <sqlite3.h>

#include "cli.h"
#include "datetime.h"
#include "db.h"
#include "log.h"
#include "path.h"
#include "util.h"

int main(int argc, char **argv) {
    ensure_todue_dir();

    char log_path[1024];
    if (todue_path(log_path, sizeof(log_path), "debug.log"))
        return -1;

    char db_path[1024];
    if (todue_path(db_path, sizeof(db_path), "todue.db"))
        return -1;

    log_set_level(LOG_DEBUG);
    log_set_file(log_path);
    
    sqlite3 *db;
    db_open(&db, db_path);
    db_init(db);

    if (argc == 1) {
        start_repl(&db);
    } else {
        argc = argc > CLI_ARGC_LIMIT ? CLI_ARGC_LIMIT : argc - 1;
        ++argv;
        execute_cmd(&db, argc, argv);
    }

    db_close(db);
    log_close();
    return 0;
}