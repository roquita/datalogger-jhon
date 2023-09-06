#include "project_typedefs.h"
#include "stdbool.h"
#include "string.h"
#include "esp_log.h"
#include "calibration.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"

#define TAG "INPUT_CALIBRATION"

static sensor_calib_t calibration;
static int _index;
static row_to_fill_t row_to_fill;
static bool is_initialized = false;

void inputCalib_init()
{
    if (is_initialized)
        return;

    // set local variables
    calibration = SENSOR_CALIB_DEFAULT;
    _index = 0;
    row_to_fill = ROW_NOT_TO_FILL;

    // init nvs
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_flash_erase();
        err = nvs_flash_init();
    }

    is_initialized = ((err == ESP_OK) ? true : false);
}
void inputCalib_set_sensor_type(sensor_type_t type)
{
    calibration.sensor_type = type;
    ESP_LOGI(TAG, "inputCalib_set_sensor_type : %u", type);
}
sensor_type_t inputCalib_get_sensor_type()
{
    return calibration.sensor_type;
}
void inputCalib_set_capacity(double capacity)
{
    calibration.capacity = capacity;
    ESP_LOGI(TAG, "inputCalib_set_capacity : %lf", capacity);
}
void inputCalib_set_unit(sensor_unit_t unit)
{
    calibration.unit_src = unit;
    ESP_LOGI(TAG, "inputCalib_set_unit : %u", unit);
}
void inputCalib_set_sensibility(double sensibility)
{
    calibration.sensibility_val = sensibility;
    ESP_LOGI(TAG, "sensibility : %lf", sensibility);
}
void inputCalib_set_sensibility_unit(sensor_sensibility_unit_t sensibility_unit)
{
    calibration.sensibility_unit = sensibility_unit;
    ESP_LOGI(TAG, "inputCalib_set_sensibility_unit: %u", sensibility_unit);
}
void inputCalib_set_limit(char *limit_str)
{
    calibration.limit_val = atof(limit_str);
    ESP_LOGI(TAG, "calibration_limit : %f", calibration.limit_val);

    char *ptr = strtok(limit_str, ".");
    ptr = strtok(NULL, ".");
    calibration.num_decimals = ptr ? strlen(ptr) : 0;
    ESP_LOGI(TAG, "decimals_precision : %i", calibration.num_decimals);
}
void inputCalib_set_limit_enable(bool limit_enable)
{
    calibration.limit_enabled = limit_enable;
    ESP_LOGI(TAG, "inputCalib_set_limit_enable : %s ", limit_enable ? "true" : "false");
}
void inputCalib_set_table(char *table_str)
{
    char *ptr;

    ESP_LOGI(TAG, "TABLE RECEIVED FROM NEXTION:");
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (i == 0 && j == 0)
                ptr = strtok(table_str, "-");
            else
                ptr = strtok(NULL, "-");

            if (ptr == NULL)
                calibration.table[i][j] = 0.0;
            else
                calibration.table[i][j] = atof(ptr);
        }
        ESP_LOGI(TAG, "%.*lf  %.*lf",
                 calibration.num_decimals, calibration.table[i][0],
                 calibration.num_decimals, calibration.table[i][1]);
    }
}
table_t inputCalib_set_num_points(calib_points_num_t num_points)
{
    int num_decimals = inputCalib_get_num_decimals();
    calibration.num_points = num_points;
    ESP_LOGI(TAG, "inputCalib_set_num_points: %u", num_points);
    table_t table = calloc(TABLE_NUM_ROWS, sizeof(row_t));
    double divisor = num_points * 1.0f;

    if (num_points == NUM_POINTS_CUSTOM)
    {
        for (int i = 0; i < TABLE_NUM_ROWS; i++)
        {
            snprintf(table[i][0], sizeof(table[i][0]), "%.*lf", num_decimals, 0.0);
            snprintf(table[i][1], sizeof(table[i][1]), "%.*lf", num_decimals, 0.0);
        }
    }
    else
    {
        for (int i = 0; i < TABLE_NUM_ROWS; i++)
        {
            double value;
            if (i <= num_points)
                value = 0.00 + i * ((calibration.limit_enabled) ? calibration.limit_val : calibration.capacity) / divisor;
            else
                value = 0.00;

            snprintf(table[i][0], sizeof(table[i][0]), "%.*lf", num_decimals, value);
            snprintf(table[i][1], sizeof(table[i][1]), "%.*lf", num_decimals, 0.0);
        }
    }

    return table;
}
void inputCalib_set_row_to_fill(row_to_fill_t _row_to_fill)
{
    switch (_row_to_fill)
    {
    case 0:
        row_to_fill = ROW_0_TO_FILL;
        break;
    case 1:
        row_to_fill = ROW_1_TO_FILL;
        break;
    case 2:
        row_to_fill = ROW_2_TO_FILL;
        break;
    case 3:
        row_to_fill = ROW_3_TO_FILL;
        break;
    case 4:
        row_to_fill = ROW_4_TO_FILL;
        break;
    case 5:
        row_to_fill = ROW_5_TO_FILL;
        break;
    case 6:
        row_to_fill = ROW_6_TO_FILL;
        break;
    case 7:
        row_to_fill = ROW_7_TO_FILL;
        break;
    case 8:
        row_to_fill = ROW_8_TO_FILL;
        break;
    case 9:
        row_to_fill = ROW_9_TO_FILL;
        break;
    case 10:
        row_to_fill = ROW_10_TO_FILL;
        break;
    default:
        row_to_fill = ROW_NOT_TO_FILL;
        break;
    }

    ESP_LOGI(TAG, "inputCalib_set_row_to_fill : %u", row_to_fill);
}
void inputCalib_set_name(char *name_str)
{
    int name_len = strlen(name_str);
    if (name_len <= CALIB_NAME_LEN_MAX)
    {
        memset(calibration.name, 0, sizeof(calibration.name));
        memcpy(calibration.name, name_str, name_len);
        calibration.name_len = name_len;

        ESP_LOGI(TAG, "inputCalib_set_name:%*s", calibration.name_len, calibration.name);
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
    }
}
static esp_err_t nvs_save_calibration_parameters(int index, char *namespace, char **info)
{
    char key[15];
    snprintf(key, sizeof(key), "%s%2i", NVS_KEY_CALIB_BASE_STR, index);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        *info = "Error: can't open nvs storage";
        return ESP_FAIL;
    }
    err = nvs_set_blob(handle, key, &calibration, sizeof(sensor_calib_t));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        *info = "Error: can't set calibration parameters";
        return ESP_FAIL;
    }
    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        *info = "Error: can't commit calibration parameters";
        return ESP_FAIL;
    }
    nvs_close(handle);
    *info = "Success: calibration parameters saved";
    return ESP_OK;
}
static esp_err_t nvs_load_calibration_parameters(int index, char *namespace, sensor_calib_t *out, size_t *len)
{
    char key[15];
    snprintf(key, sizeof(key), "%s%2i", NVS_KEY_CALIB_BASE_STR, index);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
    err = nvs_get_blob(handle, key, out, len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return ESP_FAIL;
    }
    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return ESP_FAIL;
    }
    nvs_close(handle);
    return ESP_OK;
}
static esp_err_t nvs_delete_calibration_parameters(int index, char *namespace)
{
    char key[15];
    snprintf(key, sizeof(key), "%s%2i", NVS_KEY_CALIB_BASE_STR, index);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
    err = nvs_erase_key(handle, key);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return ESP_FAIL;
    }
    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return ESP_FAIL;
    }
    nvs_close(handle);
    return ESP_OK;
}
esp_err_t inputCalib_save(char **info)
{
    ESP_LOGI(TAG, "INPUTCALIB SAVE");

    if (_index >= 0 && _index < NUM_SENSORS)
    {
        return nvs_save_calibration_parameters(_index, NVS_NAMESPACE, info);
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        *info = "Error: invalid sensor";
        return ESP_FAIL;
    }
}
esp_err_t inputCalib_load(int i, sensor_calib_t *out)
{
    ESP_LOGI(TAG, "INPUTCALIB LOAD");

    if (i >= 0 && i < NUM_SENSORS)
    {
        size_t len = sizeof(sensor_calib_t);
        return nvs_load_calibration_parameters(i, NVS_NAMESPACE, out, &len);
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
}
void inputCalib_delete(int i)
{
    ESP_LOGI(TAG, "INPUTCALIB DELETE");

    if (i >= 0 && i < NUM_SENSORS)
    {
        nvs_delete_calibration_parameters(i, NVS_NAMESPACE);
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
    }
}
esp_err_t inputCalib_load_last_saved(int *i, sensor_calib_t *out)
{
    ESP_LOGI(TAG, "INPUTCALIB LOAD LAST SAVED");

    if (_index >= 0 && _index < NUM_SENSORS)
    {
        size_t len = sizeof(sensor_calib_t);
        *i = _index;
        return nvs_load_calibration_parameters(_index, NVS_NAMESPACE, out, &len);
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
}
int inputCalib_get_num_decimals()
{
    return calibration.num_decimals;
}
void inputCalib_set_index(int index)
{
    _index = index;
    ESP_LOGI(TAG, "inputCalib_set_index:%i", index);
}
int inputCalib_get_index()
{
    return _index;
}
row_to_fill_t inputCalib_get_row_to_fill()
{
    return row_to_fill;
}
sensor_calib_t *inputCalib_get_calibration()
{
    return &calibration;
}