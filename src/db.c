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
        "done INTEGER NOT NULL DEFAULT 0,"
        "UNIQUE(brief)"
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

int db_add_todo(sqlite3 *db, const char *brief) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(
        db,
        "INSERT INTO todue (brief) VALUES (?);",
        -1,
        &stmt,
        NULL
    );
    sqlite3_bind_text(stmt, 1, brief, -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int db_mark_done(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(
        db,
        "UPDATE todue SET done = 1 WHERE id = ?;",
        -1,
        &stmt,
        NULL
    );
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int db_delete_todo(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(
        db,
        "DELETE FROM todue WHERE id = ?;",
        -1,
        &stmt,
        NULL
    );
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int db_list(sqlite3 *db, todo_callback callback, void *user_data) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(
        db,
        "SELECT * FROM todue;",
        -1,
        &stmt,
        NULL
    );
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *brief = (const char *)sqlite3_column_text(stmt, 1);
        const char *notes = (const char *)sqlite3_column_text(stmt, 2);
        const char *created = (const char *)sqlite3_column_text(stmt, 3);
        const char *due = (const char *)sqlite3_column_text(stmt, 4);
        int done = sqlite3_column_int(stmt, 5);
        callback(id, brief, notes, created, due, done, user_data);
    }
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}