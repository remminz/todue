#ifndef DB_H
#define DB_H

#include "sqlite/sqlite3.h"

typedef void (*todue_callback)(
    int         id,
    const char *brief,
    const char *notes,
    const char *created,
    const char *due,
    int         done,
    void       *user_data
);

int db_open(sqlite3 **db, const char *path);
int db_close(sqlite3 *db);
int db_init(sqlite3 *db);
int db_add_todue(sqlite3 *db, const char *brief);
int db_mark_done(sqlite3 *db, int id);
int db_delete_todue(sqlite3 *db, int id);
int db_delete_range(sqlite3 *db, int start, int end);
int db_delete_done(sqlite3 *db);
int db_list(sqlite3 *db, todue_callback callback, void *user_data);

#endif // DB_H