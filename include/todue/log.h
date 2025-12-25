#ifndef LOG_H
#define LOG_H

#include <stdio.h>

typedef enum {
    LOG_NONE  = -1,
    LOG_ERROR = 0,
    LOG_WARN  = 1,
    LOG_INFO  = 2,
    LOG_DEBUG = 3
} LogLevel;

void log_set_level(LogLevel level);
void log_set_file(const char *path);
void log_close(void);
void log_msg(
    LogLevel level,
    const char *file,
    int line,
    const char *format,
    ...
);

#ifdef LOG_DISABLED
    #define LOG_ERROR(...)
    #define LOG_WARN(...)
    #define LOG_INFO(...)
    #define LOG_DEBUG(...)
#else
    #define LOG_ERROR(...) log_msg(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
    #define LOG_WARN(...)  log_msg(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
    #define LOG_INFO(...)  log_msg(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
    #define LOG_DEBUG(...) log_msg(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#endif

#endif // LOG_H