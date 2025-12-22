#include "datetime.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    struct tm local_tm;
    localtime_safe(&now, &local_tm);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", &local_tm);
    return buf;
}

char *relative_iso_datetime(char *buf, size_t size, char *relative) {
    if (relative == NULL || relative[0] == '\0') {
        LOG_ERROR("Empty relative datetime string as argument");
        return NULL;
    }
    time_t now = time(NULL);
    struct tm local_tm;

    if (strcmp("today", relative) == 0 || strcmp("tonight", relative) == 0) {
        localtime_safe(&now, &local_tm);
        strftime(buf, size, "%Y-%m-%d 11:59:59", &local_tm);
    } else if (strcmp("tomorrow", relative) == 0) {
        now += 24 * 60 * 60;
        localtime_safe(&now, &local_tm);
        strftime(buf, size, "%Y-%m-%d 11:59:59", &local_tm);
    } else {
        char *token;
        long multi;

        if (isdigit(relative[0])) {
            multi = strtol(relative, &token, 10);
            if (multi == 0) {
                LOG_ERROR("Failed to read integer at '%s'", relative);
                return NULL;
            }
        } else if (strlen(relative) > 4 && strcmp("next", substr(relative, 0, 4)) == 0) {
            multi = 1;
            token = relative + 4;
        } else {
            LOG_ERROR("Invalid relative time '%s'", relative);
            return NULL;
        }
        skip_space(&token);
        if (token == NULL || token[0] == '\0') {
            LOG_ERROR("No relative time unit following integer '%s'", relative);
            return NULL;
        }

        const char c1 = tolower(token[0]);
        const char c2 = tolower(token[1]);
        if (secondable(c1, c2)) {
            switch(c1) {
                case 's':
                    now += multi * SECOND;
                    localtime_safe(&now, &local_tm);
                    strftime(buf, size, "%Y-%m-%d %H:%M:S", &local_tm);
                    break;
                case 'm':
                    now += multi * MINUTE;
                    localtime_safe(&now, &local_tm);
                    strftime(buf, size, "%Y-%m-%d %H:%M:59", &local_tm);
                    break;
                case 'h':
                    now += multi * HOUR;
                    localtime_safe(&now, &local_tm);
                    strftime(buf, size, "%Y-%m-%d %H:59:59", &local_tm);
                    break;
                case 'd':
                    now += multi * DAY;
                    localtime_safe(&now, &local_tm);
                    strftime(buf, size, "%Y-%m-%d 23:59:59", &local_tm);
                    break;
                case 'w':
                    now += multi * WEEK;
                    localtime_safe(&now, &local_tm);
                    strftime(buf, size, "%Y-%m-%d 23:59:59", &local_tm);
                    break;
            }
        } else {
            localtime_safe(&now, &local_tm);
            if (c1 == 'm' && c2 == 'o') {
                local_tm.tm_mon += multi * MONTH;
                mktime(&local_tm); // TODO normalization inconsistent across platforms
            } else if (c1 == 'y') {
                local_tm.tm_year += multi * YEAR;
            } else {
                LOG_ERROR("Invalid relative time unit '%s'", token);
                return NULL;
            }
            strftime(buf, size, "%Y-%m-%d 23:59:59", &local_tm);
        }
    }
    return buf;
}