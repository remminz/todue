#include "todue/log.h"

#include <stdarg.h>
#include "string.h"
#include <time.h>
#include <stdlib.h>

#include "todue/datetime.h"
#include "todue/path.h"
#include "todue/platform.h"
#include "todue/util.h"

static const size_t ISO_DATE_LEN = sizeof("YYYY-MM-DD");

static LogLevel current_level = LOG_INFO;
static FILE *log_fp = NULL;

static char *resolve_log_name(void) {
     // todue-YYYY-MM-DD.log
    char *date = malloc(ISO_DATE_LEN * sizeof(*date));
    if (!date) {
        fputs("Failed malloc in log setup\n", stderr);
        return NULL;
    }

    current_iso_datetime(date, ISO_DATE_LEN);
    date[ISO_DATE_LEN - 1] = '\0';

    const char *fmt = "%s-%s.log";
    size_t len = snprintf(NULL, 0, fmt, APP_NAME, date) + 1;

    char *name = malloc(len * sizeof(*name));
    snprintf(name, len, fmt, APP_NAME, date);

    free(date);
    return name;
}

static char *resolve_log_path(void) {
    char *log_name = resolve_log_name();
    if (!log_name) {
        return NULL;
    }

    char *log_path = strjoin(TODUE_LOG_DIR "/", log_name);
    free(log_name);

    char path[PATH_SIZE];
    todue_state_path(path, sizeof(path), log_path);

    free(log_path);
    return strdup(path);
}

int log_init(LogLevel level) {
#ifdef LOG_DISABLED
    return 0;
#endif
    log_set_level(level);

    char *log_path = resolve_log_path();
    int rc = log_set_file(log_path);

    free(log_path);
    return rc;
}

void log_set_level(LogLevel level) {
    current_level = level;
}

int log_set_file(const char *path) {
    if (log_fp) {
        fclose(log_fp);
    }
    log_fp = fopen(path, "a");

    if (!log_fp) {
        log_fp = stderr;
        return -1;
    }
    return 0;
}

void log_close(void) {
    if (log_fp && log_fp != stderr) {
        fputc('\n', log_fp);
        fclose(log_fp);
    }
    log_fp = NULL;
}

void log_msg(LogLevel level, const char *file,
             int line, const char *format, ...)
{
    if (level > current_level || level == LOG_NONE) {
        return;
    }

    const char *level_names[] = {
        "ERROR",
        "WARN",
        "INFO",
        "DEBUG"
    };

    time_t now = time(NULL);
    struct tm timestamp;
    todue_localtime(&now, &timestamp);

    if (!log_fp) {
        log_fp = stderr;
    }

    fprintf(
        log_fp,
        "%04d-%02d-%02d %02d:%02d:%02d [%s] %s:%d: ",
        timestamp.tm_year + 1900,
        timestamp.tm_mon + 1,
        timestamp.tm_mday,
        timestamp.tm_hour,
        timestamp.tm_min,
        timestamp.tm_sec,
        level_names[level],
        file,
        line
    );

    va_list args;
    va_start(args, format);
    vfprintf(log_fp, format, args);
    va_end(args);

    fputc('\n', log_fp);
    if (level == LOG_ERROR) {
        fflush(log_fp);
    }
}