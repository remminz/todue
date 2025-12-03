#include "log.h"

#include <time.h>

static LogLevel current_level = LOG_INFO;
static FILE *log_fp = NULL;

void log_set_level(LogLevel level) {
    current_level = level;
}

void log_set_file(const char *path) {
    if (log_fp) {
        fclose(log_fp);
    }
    log_fp = fopen(path, "a");
    if (!log_fp) {
        log_fp = stderr;
    }
}

void log_close(void) {
    if (log_fp && log_fp != stderr) {
        fputc('\n', log_fp);
        fclose(log_fp);
    }
    log_fp = NULL;
}

void log_msg(LogLevel level,
             const char *file,
             int line,
             const char *format,
             ...)
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
    struct tm *t = localtime(&now);
    
    if (log_fp == NULL) {
        char filename[255];
        snprintf(filename, 255, "log/debug.log");
        log_fp = fopen(filename, "a");
        if (!log_fp) {
            log_fp = stderr;
        }
    }

    fprintf(log_fp, "%04d-%02d-%02d %02d:%02d:%02d [%s] %s:%d: ",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec,
            level_names[level],
            file,
            line);

    va_list args;
    va_start(args, format);
    vfprintf(log_fp, format, args);
    va_end(args);

    fputc('\n', log_fp);
    if (level == LOG_ERROR) {
        fflush(log_fp);
    }
}