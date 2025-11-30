#include "db.h"

#include <stdio.h>

int db_open(sqlite3 **db, const char *path) {
    int rc = sqlite3_open(path, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(*db));
        return -1;
    } else {
        fputs("Successfully opened database\n", stderr);
        return 0;
    }
}

int db_close(sqlite3 *db) {
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to close database: %s\n", sqlite3_errmsg(db));
        return -1;
    } else {
        fputs("Successfully closed database\n", stderr);
        return 0;
    }
}

int db_init(sqlite3 *db) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS todue ("
        "id INTEGER PRIMARY KEY,"
        "brief TEXT NOT NULL,"
        "notes TEXT,"
        "created TEXT DEFAULT CURRENT_TIMESTAMP,"
        "due TEXT,"
        "done BOOLEAN NOT NULL DEFAULT 0"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_todue_due ON todue(due);"
        "CREATE INDEX IF NOT EXISTS idx_todue_done ON todue(done);";

    int rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to initialize schema: %s\n", sqlite3_errmsg(db));
        return -1;
    } else {
        fputs("Successfully initialized schema\n", stderr);
        return 0;
    }
}