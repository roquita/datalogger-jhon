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


void time_set_timestamp(struct tm* datetime);
int64_t time_get_timestamp();
double time_get_ftimestamp();