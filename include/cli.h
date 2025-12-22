#ifndef CLI_H
#define CLI_H

#include <sqlite3.h>

#include "commands.h"

#define CLI_ARGC_LIMIT 16
#define CLI_LINE_LIMIT 256

void start_repl(sqlite3 **db);

#endif // CLI_H