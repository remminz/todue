#ifndef DB_H
#define DB_H

#include <sqlite3.h>

typedef void (*todo_callback)(
    int id,
    const char *brief,
    const char *notes,
    const char *created,
    const char *due,
    int done,
    void *user_data
);

int db_open(sqlite3 **db, const char *path);
int db_close(sqlite3 *db);
int db_init(sqlite3 *db);
int db_add_todo(sqlite3 *db, const char *brief);
int db_mark_done(sqlite3 *db, int id);
int db_delete_todo(sqlite3 *db, int id);
int db_list(sqlite3 *db, todo_callback callback, void *user_data);

#endif // DB_H