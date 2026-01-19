#include "todue/util.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "todue/datetime.h"
#include "todue/db.h"
#include "todue/log.h"
#include "todue/platform.h"

// ANSI Sequences
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define END_BOLD_DIM "\033[22m"
#define ITALIC "\033[3m"
#define END_ITALIC "\033[23m"
#define STRIKETHROUGH "\033[9m"
#define END_STRIKETHROUGH "\033[29m"
#define END_ALL "\033[0m"

#define WHITE "\033[38;5;255m"
#define LIGHT_GRAY "\033[38;5;253m"
#define GRAY "\033[38;5;250m"

#define NO_SECONDS 16

FILE *openPager(void) {
    if (!todue_isatty_stdout()) {
        return stdout;
    }

    const char *pager = todue_get_pager();
    FILE *fp = todue_popen(pager, "w");
    
    return fp ? fp : stdout;
}

void closePager(FILE *fp) {
    if (fp != stdout) {
        todue_pclose(fp);
    }
}

void print_row(int id, const char *brief,
    const char *notes, const char *created,
    const char *due, int done, void *user_data)
{
    FILE *out = user_data;
    const int LEFT_WIDTH = 30;
    const int RIGHT_WIDTH = 80;

    fprintf(
        out,
        "%s%3d%s [%c]%s ",
        BOLD, id, END_ITALIC, done ? 'X' : ' ', END_BOLD_DIM
    );
    if (done) {
        fputs(STRIKETHROUGH DIM, out);
    } else {
        fputs(WHITE BOLD, out);
    }
    fprintf(out, "%-*.*s%s |", LEFT_WIDTH, LEFT_WIDTH, brief, END_ALL);

    if (notes) {
        fprintf(out, " %.*s\n", RIGHT_WIDTH, notes);
    } else {
        fprintf(
            out,
            " %s%.*s%s\n",
            DIM, RIGHT_WIDTH, "No notes", END_BOLD_DIM
        );
    }

    if (due && is_valid_datetime(due)) {
        fprintf(
            out,
            "   %sdue: %-*.*s%s |",
            LIGHT_GRAY ITALIC, LEFT_WIDTH, NO_SECONDS, due, END_ITALIC
        );
    } else {
        fprintf(
            out,
            "\t%s%-*s%s |",
            DIM, LEFT_WIDTH, "No due date", END_BOLD_DIM
        );
    }

    fprintf(out, "%s created: %s%s\n", GRAY ITALIC, created, END_ALL);

    fputc('\n', out);
}

const char *substr(const char *src, size_t idx, size_t size) {
    if (src == NULL || size <= 0) {
        return NULL;
    }

    size_t src_len = strlen(src);
    if (idx + size > src_len) {
        size = src_len - idx;
    }

    char *str = malloc(sizeof(char) * (size + 1));
    if (str == NULL) {
        return NULL;
    }

    strncpy(str, src + idx, size);
    str[size] = '\0';

    return str;
}

void skip_space(char **str) {
    while (**str && isspace(**str)) {
        ++(*str);
    }
}

void check_table(sqlite3 *db) {
    if (no_such_table(db)) {
        fprintf(stderr, "Database not initialized; try reload command\n");
    }
}