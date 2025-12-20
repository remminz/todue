#ifndef COMMANDS_H
#define COMMANDS_H

#include <sqlite3.h>

typedef struct {
    const char *name;
    int (*func)(sqlite3 **db, int argc, char **argv);
} Command;

int execute_cmd(sqlite3 **db, int argc, char **argv);

#endif // COMMANDS_H