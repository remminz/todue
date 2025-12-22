#include <stdio.h>
#include <time.h>

#include <sqlite3.h>

#include "cli.h"
#include "datetime.h"
#include "db.h"
#include "log.h"
#include "util.h"

int main(int argc, char **argv) {
    log_set_level(LOG_DEBUG);
    log_set_file("log/debug.log");
    
    sqlite3 *db;
    db_open(&db, "db/todue.db");
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