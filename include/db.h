#ifndef DB_H
#define DB_H

#include <sqlite3.h>

int db_open(sqlite3 **db, const char *path);
int db_close(sqlite3 *db);
int db_init(sqlite3 *db);

#endif // DB_H