#pragma once

#include "time_esp32s2.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include <stdlib.h>

typedef enum
{
    DATEFORMAT_MM_DD_YYYY = 0,
    DATEFORMAT_DD_MM_YYYY,
} dateformat_t;

typedef enum
{
    TIMEFORMAT_AM_PM = 0,
    TIMEFORMAT_24H = 2,
} timeformat_t;

//void time_init_timer();
void time_set_rtc_data_atOnce(struct tm *datetime, dateformat_t d, timeformat_t t);
//void time_rtc_start_sync();
bool time_set_rtc_data_1by1(int32_t data);
int64_t time_get_timestamp();
double time_get_ftimestamp();
char *time_get_datetime_formated(bool from_arg, int64_t fixed_timestamp);
char *time_get_date_format_str();
char *time_get_time_format_str();
