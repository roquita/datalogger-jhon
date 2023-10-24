#pragma once

#include "stdbool.h"
#include "dial_big_async.h"
#include "calibration.h"
#include "configuration.h"
#include "stdint.h"

typedef enum
{
    CLASS_I, // factory calibrated and settings on sensor side
    CLASS_D, // need calibration and settings on equip side
    CLASS_MAX,
} sensor_class_t;

typedef struct
{
    double value;
    sensor_unit_t unit;
    int num_decimals;
    int num_ints;
    int32_t tickcount;
} real_t;
typedef struct
{
    int32_t value;
    int32_t tickcount;
} raw_t;
typedef union
{
    raw_t raw;
    real_t real;
} reading_t;
typedef struct
{
    reading_t reading;
    int index;
} content_reading_t;
typedef struct
{
    sensor_class_t class;
    int index;
} content_class_t;
typedef struct
{
    char *str;
    int index;
} content_str_t;
typedef struct
{
    int32_t raw;
    double rawps;
    double real;
    double realps;
    int32_t tickcount;
    sensor_unit_t unit_src;
    int num_decimals;
    int num_ints;
} sensor_data_t;
#define SENSOR_DATA_DEFAULT      \
    (sensor_data_t)              \
    {                            \
        .raw = 0,                \
        .rawps = 0.0,            \
        .real = 0.0,             \
        .realps = 0.0,           \
        .tickcount = 0,          \
        .unit_src = SENSOR_UNIT_MAX, \
        .num_decimals = 0,       \
        .num_ints = 0,           \
    }

typedef enum
{
    USE_FREE,
    USE_TESTING,
    USE_MAX,
} sensor_use_t;
typedef struct
{
    sensor_calib_t calib;
    sensor_config_t config;
    sensor_data_t data;
    sensor_class_t class;
    bool is_free;
    bool is_calibrated;
} sensor_dev_t;

void sensor_init(int index);
void sensor_set_class(int index, sensor_class_t class);
sensor_class_t sensor_get_class(int index);
void sensor_enable(int index);
void sensor_disable(int index);
bool sensor_is_enabled(int index);
void sensor_set_tara(int index);
double sensor_get_tara(int index);
void sensor_clear_tara(int index);
double sensor_get_real(int index, bool use_tara);
char *sensor_get_real_str(int index, bool use_tara);
int32_t sensor_get_raw(int index);
char *sensor_get_raw_str(int index);
double sensor_get_rawps(int index);
double sensor_get_realps(int index);
char *sensor_get_realps_str(int index);
bool sensor_is_calibrated(int index);
bool sensor_is_ready(int index);
bool sensor_is_free(int index);
void sensor_set_calibration(int index, sensor_calib_t *calib);
void sensor_set_configuration(int index, sensor_config_t *config);
sensor_config_t *sensor_get_configuration(int index);
double sensor_get_capacity(int index);
int sensor_get_num_decimals(int index);
sensor_unit_t sensor_get_unit(int index);
sensor_unit_t sensor_get_src_unit(int index);
char *sensor_get_unit_str(int index);
char *sensor_get_unitps_str(int index);
void sensor_set_data(int index, reading_t *reading);
char *sensor_get_name(int index);
char *sensor_get_range(int index);
char *sensor_get_units_path(int index);
int32_t sensor_get_units_val(int index);
bool sensor_is_limit_enabled(int index);
sensor_type_t sensor_get_type(int index);
void sensor_set_limits(int index, bool limits);
