#include <stdio.h>
#include <sqlite3.h>
#include <time.h>

#include "db.h"
#include "util.h"
#include "datetime.h"
#include "cli.h"
#include "log.h"

int main(int argc, char **argv) {
    log_set_level(LOG_DEBUG);
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