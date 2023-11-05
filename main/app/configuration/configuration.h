#pragma once
#include "stdbool.h"
#include "calibration.h"
#include "esp_err.h"

typedef struct
{
    int num_decimals;
    double unit_fc;
    sensor_unit_t unit;
    bool is_tara_enabled;
    double tara_val;
    bool is_sensor_enabled;
    bool is_limit_enabled;
    sensor_type_t sensor_type;
    double capacity;
    sensor_unit_t capacity_unit;
    char name[CALIB_NAME_LEN_MAX + 1];
    int name_len;
} sensor_config_t;

#define SENSOR_CONFIG_DEFAULT                            \
    (sensor_config_t)                                    \
    {                                                    \
        .num_decimals = 0,                               \
        .unit_fc = 1.0,                                  \
        .unit = SENSOR_UNIT_MAX,                         \
        .is_tara_enabled = false,                        \
        .tara_val = 0.0,                                 \
        .is_sensor_enabled = false,                      \
        .is_limit_enabled = false,                       \
        .sensor_type = SENSOR_TYPE_MAX,                  \
        .capacity = 0.0,                                 \
        .capacity_unit = SENSOR_UNIT_MAX,                \
        .name_len = strlen(NEXTION_DEFAULT_SENSOR_NAME), \
        .name = NEXTION_DEFAULT_SENSOR_NAME,             \
    }
esp_err_t config_sensor_save(int index, sensor_config_t *config);
esp_err_t config_sensor_load(int index, sensor_config_t *config);
esp_err_t config_sensor_delete(int index);

void inputConfig_set_type(sensor_type_t type);
sensor_type_t inputConfig_get_type();
void inputConfig_set_capacity(double capacity);
void inputConfig_set_capacity_unit(sensor_unit_t capacity_unit);
void inputConfig_set_name(char *name, int len);
void inputConfig_set_index(int index);
bool inputConfig_save(sensor_config_t *config, char **info);
int inputConfig_get_index();
