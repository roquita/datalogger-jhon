#pragma once
#include "stdint.h"
#include "project_defines.h"
#include "stddef.h"
#include "esp_err.h"

typedef enum
{
    DISPLACEMENT = 0,
    LOAD = 1,
    PRESSURE = 2,
    VOLUME = 3,
    SENSOR_TYPE_MAX,
} sensor_type_t;
typedef enum
{
    DISPLACEMENT_UNIT_CM = 0,
    DISPLACEMENT_UNIT_IN,
    DISPLACEMENT_UNIT_MM,
    LOAD_UNIT_KN = 6,
    LOAD_UNIT_LBF,
    LOAD_UNIT_N,
    LOAD_UNIT_KGF,
    PRESSURE_UNIT_KPA = 12,
    PRESSURE_UNIT_PSI,
    PRESSURE_UNIT_KSF,
    PRESSURE_UNIT_MPA,
    PRESSURE_UNIT_KGF_CM2,
    VOLUME_UNIT_CM3 = 18,
    VOLUME_UNIT_IN3,
    SENSOR_UNIT_MAX,
} sensor_unit_t;
typedef enum
{
    SENSIBILITY_UNIT_MV_V = 0,
    SENSIBILITY_UNIT_V_V = 1,
    SENSIBILITY_UNIT_MAX,
} sensor_sensibility_unit_t;
typedef enum
{
    NUM_POINTS_1 = 1,
    NUM_POINTS_5 = 5,
    NUM_POINTS_10 = 10,
    NUM_POINTS_CUSTOM = 0XFE,
    NUM_POINTS_MAX = 0XFF,
} calib_points_num_t;
typedef struct
{
    sensor_type_t sensor_type;
    double capacity;
    sensor_unit_t unit_src;
    double sensibility_val;
    sensor_sensibility_unit_t sensibility_unit;
    double limit_val;
    bool limit_enabled;
    double table[11][2];
    calib_points_num_t num_points;
    char name[CALIB_NAME_LEN_MAX + 1];
    int name_len;
    int num_decimals;
} sensor_calib_t;

#define SENSOR_CALIB_DEFAULT                      \
    (sensor_calib_t)                              \
    {                                             \
        .sensor_type = SENSOR_TYPE_MAX,           \
        .capacity = 0.0,                          \
        .unit_src = SENSOR_UNIT_MAX,              \
        .sensibility_val = 0.0,                   \
        .sensibility_unit = SENSIBILITY_UNIT_MAX, \
        .limit_val = 0.0,                         \
        .limit_enabled = false,                   \
        .num_points = NUM_POINTS_CUSTOM,          \
        .name_len = 0,                            \
        .num_decimals = 0,                        \
    }

typedef char(row_t)[TABLE_NUM_COLS][15];
typedef row_t *table_t;

typedef enum
{
    ROW_0_TO_FILL,
    ROW_1_TO_FILL,
    ROW_2_TO_FILL,
    ROW_3_TO_FILL,
    ROW_4_TO_FILL,
    ROW_5_TO_FILL,
    ROW_6_TO_FILL,
    ROW_7_TO_FILL,
    ROW_8_TO_FILL,
    ROW_9_TO_FILL,
    ROW_10_TO_FILL,
    ROW_NOT_TO_FILL,
} row_to_fill_t;

void inputCalib_init();
void inputCalib_set_sensor_type(sensor_type_t type);
sensor_type_t inputCalib_get_sensor_type();
void inputCalib_set_capacity(double capacity);
void inputCalib_set_unit(sensor_unit_t unit);
void inputCalib_set_sensibility(double sensibility);
void inputCalib_set_sensibility_unit(sensor_sensibility_unit_t sensibility_unit);
void inputCalib_set_limit(char *limit_str);
void inputCalib_set_limit_enable(bool limit_enable);
void inputCalib_set_table(char *table_str);
table_t inputCalib_set_num_points(calib_points_num_t num_points);
void inputCalib_set_row_to_fill(row_to_fill_t _row_to_fill);
void inputCalib_set_name(char *name_str);
esp_err_t inputCalib_save(char **info);
esp_err_t inputCalib_load(int i, sensor_calib_t *out);
void inputCalib_delete(int i);
esp_err_t inputCalib_load_last_saved(int *i, sensor_calib_t *out);
int inputCalib_get_num_decimals();
void inputCalib_set_index(int index);
int inputCalib_get_index();
row_to_fill_t inputCalib_get_row_to_fill();
sensor_calib_t *inputCalib_get_calibration();
