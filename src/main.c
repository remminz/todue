#include <stdio.h>
#include <sqlite3.h>

#include "db.h"

int main(void) {
    sqlite3 *db;
    db_open(&db, "db/todue.db");
    db_init(db);
    db_close(db);
    return 0;
}