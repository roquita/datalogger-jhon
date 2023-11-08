#include "time_esp32s2.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "project_tasks.h"
#include <time.h>
#include "esp_log.h"

#define TAG "TIME_ESP32S2"

// used to sync nextion rtc and system rtc
static TimerHandle_t _timer = NULL;

// system format
static dateformat_t _dateformat;
static timeformat_t _timeformat;

// var used to set DATETIME in time_set_rtc_data()
static int _rtc_data_pos = 0;
static struct tm _datetime;
static int64_t _timestamp;

// used to print datetime formated
static char buffer[100];

// functions
static void time_timer_callback(TimerHandle_t xTimer)
{
    static int counter;
    SEND_EMPTY_MSG(main_queue, ((counter % 2) == 0) ? TIME_RTC_START_SYNC : TIME_DATETIME_PRINT, portMAX_DELAY);
    counter++;
}

void time_init_timer()
{
    assert(_timer == NULL);
    _timer = xTimerCreate("test_timer", pdMS_TO_TICKS(60 * 1000), pdTRUE, NULL, time_timer_callback);
    xTimerStart(_timer, portMAX_DELAY);
}

void time_rtc_start_sync()
{
    SEND_EMPTY_MSG(main_queue, TIME_RTC_START_SYNC, portMAX_DELAY);
}
void time_set_rtc_data_atOnce(struct tm *datetime, dateformat_t d, timeformat_t t)
{
    _dateformat = d;
    _timeformat = t;
    struct timeval now = {.tv_sec = mktime(datetime), .tv_usec = 0};
    settimeofday(&now, NULL);

    SEND_EMPTY_MSG(main_queue, TIME_DATETIME_PRINT, portMAX_DELAY);
}
void time_set_rtc_data_1by1(int32_t data)
{

    if ((_rtc_data_pos != 0) && (time_get_timestamp() - _timestamp > 5))
    {
        ESP_LOGW(TAG, "%s: line %d", __FILE__, __LINE__);
        _rtc_data_pos = 0;
    }
    _timestamp = time_get_timestamp();

    switch (_rtc_data_pos)
    {
    case 0:
        _datetime.tm_year = data;
        break;
    case 1:
        _datetime.tm_mon = data;
        break;
    case 2:
        _datetime.tm_mday = data;
        break;
    case 3:
        _datetime.tm_hour = data;
        break;
    case 4:
        _datetime.tm_min = data;
        break;
    case 5:
        _datetime.tm_sec = data;
        break;
    case 6:
        _dateformat = (data == 0) ? DATEFORMAT_MM_DD_YYYY : DATEFORMAT_DD_MM_YYYY;
        break;
    case 7:
        _timeformat = (data == 2) ? TIMEFORMAT_24H : TIMEFORMAT_AM_PM;
        struct timeval now = {.tv_sec = mktime(&_datetime), .tv_usec = 0};
        settimeofday(&now, NULL);

        SEND_EMPTY_MSG(main_queue, TIME_DATETIME_PRINT, portMAX_DELAY);
        break;
    default:
        return;
    }

    _rtc_data_pos++;
    if (_rtc_data_pos >= 8)
        _rtc_data_pos = 0;
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

char *time_get_datetime_formated(bool from_arg, int64_t fixed_timestamp)
{
    time_t timestamp;

    if (from_arg)
        timestamp = fixed_timestamp;
    else
        timestamp = time(NULL);

    struct tm *datetime = gmtime(&timestamp);

    snprintf(buffer, sizeof(buffer), "%02i/%02i/%i - %02i:%02i:%02i %s",
             (_dateformat == DATEFORMAT_MM_DD_YYYY) ? datetime->tm_mon : datetime->tm_mday,
             (_dateformat == DATEFORMAT_MM_DD_YYYY) ? datetime->tm_mday : datetime->tm_mon,
             datetime->tm_year,
             (_timeformat == TIMEFORMAT_24H) ? datetime->tm_hour : ((datetime->tm_hour) % 12),
             datetime->tm_min,
             datetime->tm_sec,
             (_timeformat == TIMEFORMAT_24H) ? "" : ((datetime->tm_hour >= 12) ? "PM" : "AM"));

    return buffer;
}

char *time_get_date_format_str()
{
    if (_dateformat == DATEFORMAT_MM_DD_YYYY)
        return "MM/DD/YYYY";
    else
        return "DD/MM/YYYY";
}
char *time_get_time_format_str()
{
    if (_timeformat == TIMEFORMAT_AM_PM)
        return "AM/PM";
    else
        return "24H";
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

Test Name - 01 / 10 / 2023 - 15 : 34 : 10 PM

*/