#include <stdio.h>
#include <sqlite3.h>

#include "db.h"
#include "util.h"
#include "log.h"

int main(void) {
    log_set_level(LOG_DEBUG);
    
    sqlite3 *db;
    db_open(&db, "db/todue.db");
    db_init(db);
    db_close(db);

    log_close();
    return 0;
}