#include "configuration.h"
#include "stdint.h"
#include "stdio.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "esp_err.h"
#include "esp_log.h"

#define TAG "INPUT_CONFIGURATION"

static sensor_type_t _type;
static double _capacity;
static sensor_unit_t _capacity_unit;
static char _name[CALIB_NAME_LEN_MAX + 1];
static int _name_len;
static int _index;

void inputConfig_set_type(sensor_type_t type)
{
    _type = type;
    ESP_LOGI(TAG, "inputConfig_set_type : %u", _type);
}
sensor_type_t inputConfig_get_type()
{
    return _type;
}
void inputConfig_set_capacity(double capacity)
{
    _capacity = capacity;
    ESP_LOGI(TAG, "inputConfig_set_capacity : %lf", _capacity);
}
void inputConfig_set_capacity_unit(sensor_unit_t capacity_unit)
{
    _capacity_unit = capacity_unit;
    ESP_LOGI(TAG, "inputConfig_set_capacity_unit : %u", _capacity_unit);
}
void inputConfig_set_name(char *name, int len)
{
    snprintf(_name, sizeof(_name), "%*s", len, name);
    _name_len = len;
    ESP_LOGI(TAG, "inputConfig_set_name : %*s", len, name);
}
void inputConfig_set_index(int index)
{
    _index = index;
}
bool inputConfig_save(sensor_config_t *config, char **info)
{
    ESP_LOGI(TAG, "inputConfig_save");

    // VERIFY _INDEX RANGE
    bool index_in_range = (_index >= 0 && _index < NUM_SENSORS);
    if (!index_in_range)
    {
        *info = "Error: invalid sensor";
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return false;
    }

    // MODIDY CURRENT CONFIGURATION
    config->sensor_type = _type;
    config->capacity = _capacity;
    config->capacity_unit = _capacity_unit;
    snprintf(config->name, sizeof(config->name), "%*s", _name_len, _name);
    config->name_len = _name_len;

    // SAVE NEW CONFIGURATION
    bool save_failed = (config_sensor_save(_index, config) != ESP_OK);
    if (save_failed)
    {
        *info = "Error: can't save configuration";
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return false;
    }

    *info = "Success: new configuration saved";
    return true;
}
int inputConfig_get_index()
{
    return _index;
}
