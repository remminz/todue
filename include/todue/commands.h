#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>

#include "sqlite/sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *aliases[2];
    int (*func)(sqlite3 **db, int argc, char **argv);
} Command;

extern const Command commands[];
extern const size_t commands_len;

int execute_cmd(sqlite3 **db, int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif // COMMANDS_H
