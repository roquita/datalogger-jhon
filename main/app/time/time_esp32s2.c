#include "time_esp32s2.h"

void time_set_timestamp(struct tm *datetime)
{
    time_t timestamp = mktime(datetime);
    struct timeval now = {.tv_sec = timestamp, .tv_usec = 0};
    settimeofday(&now, NULL);
}
int64_t time_get_timestamp()
{
    return time(NULL);
}

double time_get_ftimestamp()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    double seconds = (tv_now.tv_sec * 1.0) + (tv_now.tv_usec / 1000000.0);
    return seconds;
}

/*

    struct tm tm = {0};
    tm.tm_year = 2016 - 1900;
    tm.tm_mon = 0;
    tm.tm_mday = 10;
    tm.tm_hour = 16;
    tm.tm_min = 30;
    tm.tm_sec = 0;
    time_t t = mktime(&tm);
    const char *time_str = asctime(&tm);
    strlcpy(buf, time_str, sizeof(buf));
    printf("Setting time: %s", time_str);
    struct timeval now = {.tv_sec = t};
    settimeofday(&now, NULL);

*/