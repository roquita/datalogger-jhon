#include "configuration.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "project_defines.h"
#include "esp_log.h"
#include "string.h"

#define TAG "SENSOR_CONFIG"

static bool is_initialized = false;

void config_init()
{
    if (is_initialized)
        return;

    // set local variables

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

esp_err_t config_sensor_save(int index, sensor_config_t *config)
{
    char key[15] = {0};
    snprintf(key, sizeof(key), "%s%2i", NVS_KEY_CONFIG_BASE_STR, index);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return err;
    }
    err = nvs_set_blob(handle, key, config, sizeof(sensor_config_t));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return err;
    }
    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return err;
    }
    nvs_close(handle);
    return err;
}
esp_err_t config_sensor_load(int index, sensor_config_t *config)
{
    ESP_LOGI(TAG, "config_sensor_load");

    char key[15] = {0};
    snprintf(key, sizeof(key), "%s%2i", NVS_KEY_CONFIG_BASE_STR, index);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return err;
    }
    size_t size = sizeof(sensor_config_t);
    err = nvs_get_blob(handle, key, config, &size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nvs_close(handle);
        return err;
    }

    nvs_close(handle);
    return err;
}
esp_err_t config_sensor_delete(int index)
{
    char key[15];
    snprintf(key, sizeof(key), "%s%2i", NVS_KEY_CONFIG_BASE_STR, index);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
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
