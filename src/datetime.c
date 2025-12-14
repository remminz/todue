#include "datetime.h"

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#include "log.h"
#include "util.h"

static int to_int(const char *s) {
    return (s[0] - '0') * 10 + (s[1] - '0');
}

static int secondable(char c1, char c2) {
    return c1 == 's' ||
        (c1 == 'm' && c2 == 'i') ||
        c1 == 'h' ||
        c1 == 'd' ||
        c1 == 'w';
}

int is_valid_datetime(const char *s) {
    if (strlen(s) != 19) return 0;

    // YYYY-MM-DD HH:MM:SS
    if (!(isdigit(s[0]) && isdigit(s[1]) && isdigit(s[2]) && isdigit(s[3]) &&
          s[4] == '-' &&
          isdigit(s[5]) && isdigit(s[6]) &&
          s[7] == '-' &&
          isdigit(s[8]) && isdigit(s[9]) &&
          s[10] == ' ' &&
          isdigit(s[11]) && isdigit(s[12]) &&
          s[13] == ':' &&
          isdigit(s[14]) && isdigit(s[15]) &&
          s[16] == ':' &&
          isdigit(s[17]) && isdigit(s[18]))) {
        return 0;
    }

    // check numeric ranges
    int month = to_int(&s[5]);
    int day   = to_int(&s[8]);
    int hour  = to_int(&s[11]);
    int min   = to_int(&s[14]);
    int sec   = to_int(&s[17]);

    if (month < 1 || month > 12) return 0;
    if (day   < 1 || day   > 31) return 0;
    if (hour  > 23) return 0;
    if (min   > 59) return 0;
    if (sec   > 59) return 0;

    return 1;
}

char *current_iso_datetime(char *buf, size_t size) {
    time_t now = time(NULL);
    struct tm tm;
    localtime_r(&now, &tm);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

char *relative_datetime(char *buf, size_t size, char *relative) {
    if (relative == NULL || relative[0] == '\0') {
        LOG_ERROR("Empty relative datetime string as argument");
        return NULL;
    }
    time_t t = time(NULL);
    struct tm tm;

    if (!strcmp("today", relative) || !strcmp("tonight", relative)) {
        localtime_r(&t, &tm);
        strftime(buf, size, "%Y-%m-%d 11:59:59", &tm);
    } else if (!strcmp("tomorrow", relative)) {
        t += 24 * 60 * 60;
        localtime_r(&t, &tm);
        strftime(buf, size, "%Y-%m-%d 11:59:59", &tm);
    } else {
        char *token;
        long multi;

        if (isdigit(relative[0])) {
            multi = strtol(relative, &token, 10);
            if (!multi) {
                LOG_ERROR("Failed to read integer at '%s'", relative);
                return NULL;
            }
        } else if (strlen(relative) > 4 && !strcmp("next", substr(relative, 0, 4))) {
            multi = 1;
            token = relative + 4;
        } else {
            LOG_ERROR("Invalid relative time '%s'", relative);
            return NULL;
        }
        skip_spaces(&token);
        if (!strlen(token)) {
            LOG_ERROR("No relative time unit following integer '%s'", relative);
            return NULL;
        }

        const char c1 = tolower(*token);
        const char c2 = tolower(*(token + 1));
        if (secondable(c1, c2)) {
            if (c1 == 's') {
                t += multi * SECOND;
                localtime_r(&t, &tm);
                strftime(buf, size, "%Y-%m-%d %H:%M:S", &tm);
            } else if (c1 == 'm') {
                t += multi * MINUTE;
                localtime_r(&t, &tm);
                strftime(buf, size, "%Y-%m-%d %H:%M:59", &tm);
            } else if (c1 == 'h') {
                t += multi * HOUR;
                localtime_r(&t, &tm);
                strftime(buf, size, "%Y-%m-%d %H:59:59", &tm);
            } else if (c1 == 'd') {
                t += multi * DAY;
                localtime_r(&t, &tm);
                strftime(buf, size, "%Y-%m-%d 23:59:59", &tm);
            } else if (c1 == 'w') {
                t += multi * WEEK;
                localtime_r(&t, &tm);
                strftime(buf, size, "%Y-%m-%d 23:59:59", &tm);
            }
        } else {
            localtime_r(&t, &tm);
            if (c1 == 'm' && c2 == 'o') {
                tm.tm_mon += multi * MONTH;
                mktime(&tm); // TODO normalization inconsistent across platforms
            } else if (c1 == 'y') {
                tm.tm_year += multi * YEAR;
            } else {
                LOG_ERROR("Invalid relative time unit '%s'", token);
                return NULL;
            }
            strftime(buf, size, "%Y-%m-%d 23:59:59", &tm);
        }
    }
    return buf;
}