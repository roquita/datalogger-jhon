#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "string.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "project_tasks.h"
#include "sensor.h"
#include "gui.h"
#include "calibration.h"
#include "configuration.h"
#include "esp_err.h"
#include "helpers.h"
#include "test.h"

#define TAG "MAIN_CALLBACK"

/*
██╗███╗   ██╗██╗████████╗
██║████╗  ██║██║╚══██╔══╝
██║██╔██╗ ██║██║   ██║
██║██║╚██╗██║██║   ██║
██║██║ ╚████║██║   ██║
╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝

*/

void main_task_init()
{
    // init gui
    nextion_1_init();
    vTaskDelay(pdMS_TO_TICKS(3000));

    // init sensors default
    for (int index = 0; index < NUM_SENSORS; index++)
        sensor_init(index);

    sensor_set_class(0, CLASS_D);
    sensor_set_class(1, CLASS_D);
    sensor_set_class(2, CLASS_D);
    sensor_set_class(3, CLASS_D);
    sensor_set_class(4, CLASS_I);
    sensor_set_class(5, CLASS_I);
    sensor_set_class(6, CLASS_I);
    sensor_set_class(7, CLASS_I);

    // init nvs storage
    inputCalib_init();

    for (int index = 0; index < NUM_SENSORS; index++)
    {
        // load nvs content: calibrations and configurations
        sensor_calib_t calib;
        sensor_config_t config;

        bool load_calib_success = (inputCalib_load(index, &calib) == ESP_OK);
        bool load_config_success = (config_sensor_load(index, &config) == ESP_OK);

        bool load_success = load_calib_success && load_config_success;

        // update ram
        if (load_calib_success)
            sensor_set_calibration(index, &calib);
        else
            ESP_LOGW(TAG, "file:%s, line:%d", __FILE__, __LINE__);

        if (load_config_success)
            sensor_set_configuration(index, &config);
        else
            ESP_LOGW(TAG, "file:%s, line:%d", __FILE__, __LINE__);

        // update objects on screen
        nextion_1_home_write_name(index, load_success ? sensor_get_name(index) : NEXTION_DEFAULT_SENSOR_NAME);
        nextion_1_home_write_unit(index, load_success ? sensor_get_unit_str(index) : NEXTION_DEFAULT_SENSOR_UNIT);
        nextion_1_home_write_unitps(index, load_success ? sensor_get_unitps_str(index) : NEXTION_DEFAULT_SENSOR_UNITPS);
        nextion_1_calibration_write_name(index, load_success ? sensor_get_name(index) : NEXTION_DEFAULT_SENSOR_NAME);
        nextion_1_calibration_write_range(index, load_success ? sensor_get_range(index) : NEXTION_DEFAULT_RANGE);
        nextion_1_calibration_write_units_path(index, load_success ? sensor_get_units_path(index) : NEXTION_DEFAULT_PATH);
        nextion_1_calibration_write_units_val(index, load_success ? sensor_get_units_val(index) : NEXTION_DEFAULT_UNITS_VAL);
        nextion_1_calibration_write_unit(index, load_success ? sensor_get_unit_str(index) : NEXTION_DEFAULT_SENSOR_UNIT);
        nextion_1_calibration_write_limit(index, load_success ? sensor_is_limit_enabled(index) : NEXTION_DEFAULT_LIMIT_ENABLED);
        bool is_enabled = load_success ? sensor_is_enabled(index) : NEXTION_DEFAULT_SENSOR_ENABLE;
        nextion_1_calibration_write_switch(index, is_enabled);
        SEND_I32_MSG(sensor_queue, is_enabled ? SENSOR_ENABLED : SENSOR_DISABLED, index, portMAX_DELAY);
    }

    nextion_1_start_timer();
}

/*
██████╗  █████╗  ██████╗ ███████╗███████╗    ██╗      ██████╗  █████╗ ██████╗ ███████╗██████╗
██╔══██╗██╔══██╗██╔════╝ ██╔════╝██╔════╝    ██║     ██╔═══██╗██╔══██╗██╔══██╗██╔════╝██╔══██╗
██████╔╝███████║██║  ███╗█████╗  ███████╗    ██║     ██║   ██║███████║██║  ██║█████╗  ██║  ██║
██╔═══╝ ██╔══██║██║   ██║██╔══╝  ╚════██║    ██║     ██║   ██║██╔══██║██║  ██║██╔══╝  ██║  ██║
██║     ██║  ██║╚██████╔╝███████╗███████║    ███████╗╚██████╔╝██║  ██║██████╔╝███████╗██████╔╝
╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚══════╝    ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═════╝ ╚══════╝╚═════╝

*/

void home_loaded_cb(msg_t *msg)
{
    // ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_HOME);
    nextion_1_start_timer();
}
void system_settings_loaded_cb(msg_t *msg)
{
    // ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_SYSTEM_SETTINGS);
    nextion_1_stop_timer();
}
void calibration_loaded_cb(msg_t *msg)
{
    // ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_CALIBRATION);
    nextion_1_start_timer();
}
void inputcalibp1_loaded_cb(msg_t *msg)
{
    nextion_1_set_page(PAGE_INPUTCALIBP1);
    nextion_1_stop_timer();
}
void inputcalibp2_loaded_cb(msg_t *msg)
{
    nextion_1_set_page(PAGE_INPUTCALIBP2);
    nextion_1_stop_timer();
}
void inputcalibp3_loaded_cb(msg_t *msg)
{
    // ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_INPUTCALIBP3);
    nextion_1_start_timer();
}
void inputcalibp4_loaded_cb(msg_t *msg)
{
    nextion_1_set_page(PAGE_INPUTCALIBP4);
    nextion_1_stop_timer();
}
void keybda_loaded_cb(msg_t *msg) {}
void numpad_popup_loaded_cb(msg_t *msg)
{
    nextion_1_set_page(PAGE_NUMPAD_POPUP);
    nextion_1_stop_timer();
}
void intro_loaded_cb(msg_t *msg) {}
void inputconfigp1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(INPUTCONFIGP1_LOADED);
    nextion_1_stop_timer();
}
void new_test_p1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P1);
    nextion_1_stop_timer();
}
void new_test_p2_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P2);
    nextion_1_stop_timer();
}
void new_test_p3_1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P3_1);
    nextion_1_stop_timer();
    new_test_set_logging_page(PAGE_NEW_TEST_P3_1);
}
void new_test_p3_2_loadedcb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P3_2);
    nextion_1_stop_timer();
    new_test_set_logging_page(PAGE_NEW_TEST_P3_2);
}
void new_test_p3_3_loadedcb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P3_3);
    nextion_1_stop_timer();
    new_test_set_logging_page(PAGE_NEW_TEST_P3_3);
}
void new_test_p3_4_loadedcb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P3_4);
    nextion_1_stop_timer();
    new_test_set_logging_page(PAGE_NEW_TEST_P3_4);
}
void log_sensor_table_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_LOG_SENSOR_TABLE);
    nextion_1_stop_timer();
}
void log_time_table_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_LOG_TIME_TABLE);
    nextion_1_stop_timer();
}
void new_test_p4_1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_1);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_1);
}
void new_test_p4_2_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_2);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_2);
}
void new_test_p4_3_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_3);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_3);
}
void new_test_p4_4_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_4);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_4);
}
void new_test_p4_5_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_5);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_5);
}
void new_test_p4_6_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_6);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_6);
}
void new_test_p4_7_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_7);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_7);
}
void new_test_p4_8_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_8);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_8);
}
void new_test_p4_9_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P4_9);
    nextion_1_stop_timer();
    new_test_set_stop_page(PAGE_NEW_TEST_P4_9);
}
void new_test_p5_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P5);
    nextion_1_stop_timer();
}
void new_test_p6_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P6);
    nextion_1_stop_timer();
}
void new_test_p7_1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P7_1);
    nextion_1_stop_timer();
    new_test_set_start_page(PAGE_NEW_TEST_P7_1);
}
void new_test_p7_2_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P7_2);
    nextion_1_stop_timer();
    new_test_set_start_page(PAGE_NEW_TEST_P7_2);
}
void new_test_p7_3_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P7_3);
    nextion_1_start_timer();
    new_test_set_start_page(PAGE_NEW_TEST_P7_3);
}
void new_test_p7_4_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST_P7_4);
    nextion_1_start_timer();
    new_test_set_start_page(PAGE_NEW_TEST_P7_4);
}
void previous_test_p1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_PREVIOUS_TEST_P1);
    nextion_1_stop_timer();
}
void previous_test_p2_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_PREVIOUS_TEST_P2);
    nextion_1_stop_timer();
}
void previous_test_p3_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_PREVIOUS_TEST_P3);
    nextion_1_stop_timer();
}
void export_test_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_EXPORT_TEST);
    nextion_1_stop_timer();
}
void delete_test_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_DELETE_TEST);
    nextion_1_stop_timer();
}
void message_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_MESSAGE);
    nextion_1_stop_timer();
}
void current_test_p1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_CURRENT_TEST_P1);
    nextion_1_stop_timer();
}
void current_test_p2_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_CURRENT_TEST_P2);
    nextion_1_stop_timer();
}
void current_test_p3_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_CURRENT_TEST_P3);
    nextion_1_start_timer();
}

/*
██╗  ██╗ ██████╗ ███╗   ███╗███████╗
██║  ██║██╔═══██╗████╗ ████║██╔════╝
███████║██║   ██║██╔████╔██║█████╗
██╔══██║██║   ██║██║╚██╔╝██║██╔══╝
██║  ██║╚██████╔╝██║ ╚═╝ ██║███████╗
╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝

*/
void tara_enabled_cb(msg_t *msg)
{
    // get index
    int index = msg->content.i32;

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // update ram
    sensor_set_tara(index);

    // update nvs
    config_sensor_save(index, sensor_get_configuration(index));
}
void tara_disabled_cb(msg_t *msg)
{
    // get index
    int index = msg->content.i32;

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // update ram
    sensor_clear_tara(index);

    // update nvs
    config_sensor_save(index, sensor_get_configuration(index));
}

/*
 ██████╗ █████╗ ██╗     ██╗██████╗ ██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝██╔══██╗██║     ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██║     ███████║██║     ██║██████╔╝██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║
██║     ██╔══██║██║     ██║██╔══██╗██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
╚██████╗██║  ██║███████╗██║██████╔╝██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
 ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

void sensor_disabled_cb(msg_t *msg)
{
    // get index
    int index = msg->content.i32;

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // disable physically
    SEND_I32_MSG(sensor_queue, SENSOR_DISABLED, index, portMAX_DELAY);

    // update ram
    sensor_disable(index);

    // update nvs
    config_sensor_save(index, sensor_get_configuration(index));
}
void sensor_enabled_cb(msg_t *msg)
{
    // ESP_LOGI(TAG, "file:%s,line:%i", __FILE__, __LINE__);
    int index = msg->content.i32;

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // enable physically
    SEND_I32_MSG(sensor_queue, SENSOR_ENABLED, index, portMAX_DELAY);

    // status disabled in ram
    sensor_enable(index);

    // status disabled in nvs
    config_sensor_save(index, sensor_get_configuration(index));
}
void sensor_limits_disabled(msg_t *msg)
{
    // get index
    int index = msg->content.i32;

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // set on ram
    sensor_set_limits(index, false);

    // set on nvs
    config_sensor_save(index, sensor_get_configuration(index));
}
void sensor_limits_enabled(msg_t *msg)
{
    // get index
    int index = msg->content.i32;

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // set on ram
    sensor_set_limits(index, true);

    // set on nvs
    config_sensor_save(index, sensor_get_configuration(index));
}

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗  ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗███║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝╚██║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝  ██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║      ██║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝      ╚═╝

*/

void inputcalibp1_type_received_cb(msg_t *msg)
{
    // UNPACK
    uint8_t type_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERIFY TYPE IS IN RANGE
    bool in_range = helper_type_in_range(type_u8);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputCalib_set_sensor_type((sensor_type_t)type_u8);
}
void inputcalibp1_capacity_received_cb(msg_t *msg)
{
    // UNPACK
    char *addr = (char *)(msg->content.addr);
    double capacity = atof(addr);
    free(msg->content.addr);

    // UPDATE RAM
    inputCalib_set_capacity(capacity);
}
void inputcalibp1_capacity_unit_received_cb(msg_t *msg)
{
    // UNPACK
    uint8_t unit_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERYIFY UNIT IN RANGE
    bool in_range = helper_unit_in_range(unit_u8, inputCalib_get_sensor_type());
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputCalib_set_unit((sensor_unit_t)unit_u8);
}
void inputcalibp1_sensibility_received_cb(msg_t *msg)
{
    // UNPACK
    char *addr = (char *)(msg->content.addr);
    double sensibility = atof(addr);
    free(msg->content.addr);

    // UPDATE RAM
    inputCalib_set_sensibility(sensibility);
}
void inputcalibp1_sensibility_unit_received_cb(msg_t *msg)
{
    // UNPACK
    uint8_t sensibility_unit_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERYIFY UNIT IN RANGE
    bool in_range = helper_sensibility_unit_in_range(sensibility_unit_u8);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputCalib_set_sensibility_unit((sensor_sensibility_unit_t)sensibility_unit_u8);
}
void inputcalibp1_sensor_index_received(msg_t *msg)
{
    // UNPACK
    uint8_t index = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputCalib_set_index((int)index);
}
/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██████╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗╚════██╗
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝ █████╔╝
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝ ██╔═══╝
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║     ███████╗
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝     ╚══════╝

*/

void inputcalibp2_calibration_limit_received_cb(msg_t *msg)
{
    // UNPACK
    char *addr = (char *)(msg->content.addr);

    // UPDATE RAM
    inputCalib_set_limit(addr);

    free(msg->content.addr);
}

void inputcalibp2_limit_enable_receive_cb(msg_t *msg)
{
    // UNPACK
    bool limit_enable = *(bool *)(msg->content.addr);
    free(msg->content.addr);

    // UPDATE RAM
    inputCalib_set_limit_enable(limit_enable);
}
/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██████╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗╚════██╗
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝ █████╔╝
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝  ╚═══██╗
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║     ██████╔╝
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝     ╚═════╝

*/

void inputcalibp3_table_received(msg_t *msg)
{
    char *addr = (char *)(msg->content.addr);
    inputCalib_set_table(addr);
    free(addr);
}
void inputcalibp3_num_points_received(msg_t *msg)
{
    // UNPACK
    uint8_t num_points_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERIFY NUM POINTS IN RANGE
    bool in_range = helper_num_points_in_range(num_points_u8);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    table_t table = inputCalib_set_num_points((calib_points_num_t)num_points_u8);

    ESP_LOGI(TAG, "NEW TABLE:");
    for (int i = 0; i < 11; i++)
        ESP_LOGI(TAG, "%s - %s", table[i][0], table[i][1]);

    // UPDATE SCREEN
    nextion_1_inputcalibp3_write_real0(table[0][0]);
    nextion_1_inputcalibp3_write_real1(table[1][0]);
    nextion_1_inputcalibp3_write_real2(table[2][0]);
    nextion_1_inputcalibp3_write_real3(table[3][0]);
    nextion_1_inputcalibp3_write_real4(table[4][0]);
    nextion_1_inputcalibp3_write_real5(table[5][0]);
    nextion_1_inputcalibp3_write_real6(table[6][0]);
    nextion_1_inputcalibp3_write_real7(table[7][0]);
    nextion_1_inputcalibp3_write_real8(table[8][0]);
    nextion_1_inputcalibp3_write_real9(table[9][0]);
    nextion_1_inputcalibp3_write_real10(table[10][0]);

    nextion_1_inputcalibp3_write_raw0(table[0][1]);
    nextion_1_inputcalibp3_write_raw1(table[1][1]);
    nextion_1_inputcalibp3_write_raw2(table[2][1]);
    nextion_1_inputcalibp3_write_raw3(table[3][1]);
    nextion_1_inputcalibp3_write_raw4(table[4][1]);
    nextion_1_inputcalibp3_write_raw5(table[5][1]);
    nextion_1_inputcalibp3_write_raw6(table[6][1]);
    nextion_1_inputcalibp3_write_raw7(table[7][1]);
    nextion_1_inputcalibp3_write_raw8(table[8][1]);
    nextion_1_inputcalibp3_write_raw9(table[9][1]);
    nextion_1_inputcalibp3_write_raw10(table[10][1]);

    free(table);
}
void inputcalibp3_row_to_fill_received(msg_t *msg)
{
    // UNPACK
    uint8_t row_to_fill_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERIFY ROW TO FILL IN RANGE
    bool in_range = helper_row_to_fill_in_Range(row_to_fill_u8);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // update ram
    inputCalib_set_row_to_fill((row_to_fill_t)row_to_fill_u8);
}

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██╗  ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗██║  ██║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝███████║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝ ╚════██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║          ██║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝          ╚═╝

*/

void inputcalibp4_name_received_cb(msg_t *msg)
{
    char *addr = (char *)(msg->content.addr);
    inputCalib_set_name(addr);
    free(addr);
}
void inputcalibp4_save_pressed_cb(msg_t *msg)
{
    // save current sensor calibration on inpucalib engine
    char *info;
    bool save_calibration_success = (inputCalib_save(&info) == ESP_OK);

    // get calibration and index from inputCalib
    sensor_calib_t *calib = inputCalib_get_calibration();
    int index = inputCalib_get_index();

    // generate configuration from last calibration saved
    sensor_config_t config;
    config.num_decimals = calib->num_decimals;
    config.unit_fc = 1.0;
    config.unit = calib->unit_src;
    config.is_tara_enabled = false;
    config.tara_val = 0.0;
    config.is_sensor_enabled = sensor_is_enabled(index);
    config.is_limit_enabled = false;

    // save configuration in nvs
    bool save_configuration_success = (config_sensor_save(index, &config) == ESP_OK);

    // set configuration in ram
    sensor_set_configuration(index, &config);

    // set calibration in ram
    sensor_set_calibration(index, calib);

    // update objects in screen
    nextion_1_home_write_name(index, sensor_get_name(index));
    nextion_1_home_write_unit(index, sensor_get_unit_str(index));
    nextion_1_home_write_unitps(index, sensor_get_unitps_str(index));
    nextion_1_calibration_write_switch(index, sensor_is_enabled(index));
    nextion_1_calibration_write_name(index, sensor_get_name(index));
    nextion_1_calibration_write_range(index, sensor_get_range(index));
    nextion_1_calibration_write_unit(index, sensor_get_unit_str(index));
    nextion_1_calibration_write_units_path(index, sensor_get_units_path(index));
    nextion_1_calibration_write_units_val(index, sensor_get_units_val(index));

    if (!save_configuration_success || !save_calibration_success)
    {
        // infor text (red)
        nextion_1_inputcalibp4_write_result_color(63488);
        nextion_1_inputcalibp4_write_result("Error: cant save");
        ESP_LOGE(TAG, "file:%s,line:%i", __FILE__, __LINE__);
        return;
    }
    else
    {
        // info text (green)
        nextion_1_inputcalibp4_write_result_color(3457);
        nextion_1_inputcalibp4_write_result("Success: saved");
    }
}

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ ██████╗ ███╗   ██╗███████╗██╗ ██████╗ ██╗   ██╗██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔═══██╗████╗  ██║██╔════╝██║██╔════╝ ██║   ██║██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ██║   ██║██╔██╗ ██║█████╗  ██║██║  ███╗██║   ██║██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██║   ██║██║╚██╗██║██╔══╝  ██║██║   ██║██║   ██║██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗╚██████╔╝██║ ╚████║██║     ██║╚██████╔╝╚██████╔╝██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝     ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

void inputconfigp1_type_received_cb(msg_t *msg)
{
    // UNPACK
    uint8_t type_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERIFY TYPE IS IN RANGE
    bool in_range = helper_type_in_range(type_u8);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputConfig_set_type((sensor_type_t)type_u8);
}
void inputconfigp1_capacity_received_cb(msg_t *msg)
{
    // UNPACK
    char *addr = (char *)(msg->content.addr);
    double capacity = atof(addr);
    free(msg->content.addr);

    // UPDATE RAM
    inputConfig_set_capacity(capacity);
}
void inputconfigp1_capacity_unit_received_cb(msg_t *msg)
{
    // UNPACK
    uint8_t unit_u8 = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERYIFY UNIT IN RANGE
    bool in_range = helper_unit_in_range(unit_u8, inputConfig_get_type());
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputConfig_set_capacity_unit((sensor_unit_t)unit_u8);
}
void inputconfigp1_name_received_cb(msg_t *msg)
{
    char *addr = (char *)(msg->content.addr);
    int len = msg->size;
    inputConfig_set_name(addr, len);
    free(addr);
}
void inputconfigp1_index_received_cb(msg_t *msg)
{
    // UNPACK
    uint8_t index = *(uint8_t *)(msg->content.addr);
    free(msg->content.addr);

    // VERIFY INDEX IN RANGE
    bool in_range = helper_index_in_range(index);
    if (!in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // UPDATE RAM
    inputConfig_set_index((int)index);
}
void inputconfigp1_save_pressed_cb(msg_t *msg)
{
    // get index
    int index = inputConfig_get_index();

    // save on ram and nvs
    char *info;
    sensor_config_t *config = sensor_get_configuration(index);
    bool success = inputConfig_save(config, &info);

    // update screen
    nextion_1_home_write_name(index, sensor_get_name(index));
    nextion_1_calibration_write_name(index, sensor_get_name(index));
    nextion_1_calibration_write_range(index, sensor_get_range(index));

    if (success)
    {
        // info text (green)
        nextion_1_inputconfigp1_write_result_color(3457);
        nextion_1_inputconfigp1_write_result(info);
    }
    else
    {
        // info text (red)
        nextion_1_inputconfigp1_write_result_color(63488);
        nextion_1_inputconfigp1_write_result(info);
    }
}

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗  ██╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗███║
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝╚██║
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝  ██║
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║      ██║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝      ╚═╝

*/

void new_test_p1_type_received(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 1);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    test_type_t test_type;
    esp_err_t err = new_test_get_type_from_multiradio(*addr, &test_type);

    // VERIFY UNPACK ERR
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR: INVALID TYPE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    err = new_test_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOW MEMORY");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    err = new_test_set_type(&test_type);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n INTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P2);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ██████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██╗
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝ █████╔╝
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝ ██╔═══╝
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║     ███████╗
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚══════╝

*/
void new_test_p2_inputs_received(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 2);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n COMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = (uint8_t *)msg->content.addr;
    uint8_t input_enable = *addr;
    uint8_t tara_enable = *(addr + 1);
    int sensor_num;
    new_test_input_get_sensor_num_from_enablers(&sensor_num, input_enable);

    // VERIFY UNPACK ERR
    if (input_enable == 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n NO INPUTS SELECTED");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_inputs_t test_inputs;
    test_inputs.input_enable.byte = input_enable;
    test_inputs.tara_enable.byte = tara_enable;
    test_inputs.sensor_num = sensor_num;

    esp_err_t err3 = new_test_set_inputs(&test_inputs);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n INTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    page_t page;
    new_test_get_logging_page(&page);
    nextion_1_change_page(page);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ██████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██╗
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝ █████╔╝
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝  ╚═══██╗
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║     ██████╔╝
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚═════╝

*/
void new_test_p3_1_logging_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 12);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n COMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    logging_direction_t direction;
    double value = atof((char *)addr + 2);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*(addr), &sensor_index);
    esp_err_t err2 = new_test_get_logging_direction_from_combobox(*(addr + 1), &direction);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOGGING INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    // VERIFY UNPACK ERR
    if (err2 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOGGING DIRECTION");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    // VERIFY UNPACK ERR
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOGGING VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_logging_t test_logging;
    test_logging.condition = LOGGING_CONDITION_INTERVAL_LOGGING;
    test_logging.parameters.interval_logging.sensor_index = sensor_index;
    test_logging.parameters.interval_logging.direction = direction;
    test_logging.parameters.interval_logging.value = value;

    esp_err_t err3 = new_test_set_logging(&test_logging);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    page_t page;
    new_test_get_stop_page(&page);
    nextion_1_change_page(page);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p3_2_logging_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 10);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    int value = atoi((char *)addr);

    // VERIFY UNPACKED
    if (value <= 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOGGING VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_logging_t test_logging;
    test_logging.condition = LOGGING_CONDITION_LINEAR_TIME_INTERVAL;
    test_logging.parameters.linear_time_interval.value = (uint32_t)value;

    esp_err_t err3 = new_test_set_logging(&test_logging);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n INTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    page_t page;
    new_test_get_stop_page(&page);
    nextion_1_change_page(page);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p3_3_logging_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 302);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n COMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    logging_direction_t direction;
    double value[30] = {0}; // all zeros
    int size = 30;
    for (int i = 0; i < 30; i++)
    {
        value[i] = atof((char *)addr + 2 + (i * 10));

        if (value[i] < 0.0001)
        {
            size = i;
            break;
        }
    }
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*(addr), &sensor_index);
    esp_err_t err2 = new_test_get_logging_direction_from_combobox(*(addr + 1), &direction);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOGGING INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (err2 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n LOGGING DIRECTION");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_logging_t test_logging;
    test_logging.condition = LOGGING_CONDITION_INTERVAL_LOGGING_TABLE;
    test_logging.parameters.interval_logging_table.sensor_index = sensor_index;
    test_logging.parameters.interval_logging_table.direction = direction;
    memcpy(test_logging.parameters.interval_logging_table.value, value, sizeof(value));
    test_logging.parameters.interval_logging_table.size = size;

    esp_err_t err3 = new_test_set_logging(&test_logging);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    page_t page;
    new_test_get_stop_page(&page);
    nextion_1_change_page(page);

end:
    if (size_received > 0)
        free(msg->content.addr);
}

void new_test_p3_4_logging_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 330);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P3_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    test_time_t value[30] = {0};
    int size = 30;
    for (int i = 0; i < 30; i++)
    {
        char *base = (char *)addr + (i * 11);
        value[i].h = (uint8_t)atoi(base);
        value[i].m = (uint8_t)atoi(base + 3);
        value[i].s = (float)atof(base + 6);

        bool zero_detected = ((value[i].h == 0) && (value[i].m == 0) && (value[i].s < 0.0001));
        if (zero_detected)
        {
            size = i;
            break;
        }
    }

    // STORE DATA
    test_logging_t test_logging;
    test_logging.condition = LOGGING_CONDITION_ELAPSED_TIME_TABLE;
    memcpy(test_logging.parameters.elapsed_time_table.value, value, sizeof(value));
    test_logging.parameters.elapsed_time_table.size = size;

    esp_err_t err3 = new_test_set_logging(&test_logging);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    page_t page;
    new_test_get_stop_page(&page);
    nextion_1_change_page(page);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ██╗  ██╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗██║  ██║
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝███████║
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝ ╚════██║
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║          ██║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝          ╚═╝

*/
void new_test_p4_1_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_GREATER_THAN;
    test_stop.parameters.greater_than.sensor_index = sensor_index;
    test_stop.parameters.greater_than.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_2_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_LESS_THAN;
    test_stop.parameters.less_than.sensor_index = sensor_index;
    test_stop.parameters.less_than.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_3_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 10);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint32_t value = (uint32_t)atoi((char *)addr);

    // VERIFY UNPACK ERR
    if (value == 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_TIME_DELAY;
    test_stop.parameters.time_delay.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_4_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_DISTANCE_UP;
    test_stop.parameters.distance_up.sensor_index = sensor_index;
    test_stop.parameters.distance_up.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_5_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_5);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_5);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_5);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_DISTANCE_DOWN;
    test_stop.parameters.distance_down.sensor_index = sensor_index;
    test_stop.parameters.distance_down.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_6_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_6);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_6);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_6);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_PERCENT_DROP;
    test_stop.parameters.percent_drop.sensor_index = sensor_index;
    test_stop.parameters.percent_drop.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_7_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_7);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_7);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_7);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_PERCENT_STRAIN;
    test_stop.parameters.percent_strain.sensor_index = sensor_index;
    test_stop.parameters.percent_strain.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_8_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 11);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_8);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    double value = atof((char *)addr + 1);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if (err1 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_8);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if (value < 0.001)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n STOP VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_8);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_PERCENT_STRESS_DROP;
    test_stop.parameters.percent_stress_drop.sensor_index = sensor_index;
    test_stop.parameters.percent_stress_drop.value = value;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p4_9_stop_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 0);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P4_9);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_stop_t test_stop;
    test_stop.condition = STOP_CONDITION_OPERATOR_STOP;

    esp_err_t err3 = new_test_set_stop(&test_stop);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_NEW_TEST_P5);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ███████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗██╔════╝
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝███████╗
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝ ╚════██║
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║     ███████║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚══════╝

*/

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗  ██████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗██╔════╝
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝███████╗
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝ ██╔═══██╗
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║     ╚██████╔╝
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝      ╚═════╝

*/

void new_test_p6_graph_received_cb(msg_t *msg)
{

    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 2);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P6);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    axis_option_t x_axis_option;
    axis_option_t y_axis_option;
    esp_err_t err1 = new_test_get_graph_option_from_combobox(*addr, &x_axis_option);
    esp_err_t err2 = new_test_get_graph_option_from_combobox(*(addr + 1), &y_axis_option);

    // VERIFY UNPACK ERR
    if ((err1 != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n X - AXIS");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P6);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if ((err2 != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n Y - AXIS");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P6);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_graph_t test_graph;
    test_graph.x_axis_option = x_axis_option;
    test_graph.y_axis_option = y_axis_option;

    esp_err_t err3 = new_test_set_graph(&test_graph);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    page_t page;
    new_test_get_start_page(&page);
    nextion_1_change_page(page);

end:
    if (size_received > 0)
        free(msg->content.addr);
}

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ███████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██║
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝    ██╔╝
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝    ██╔╝
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║        ██║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝        ╚═╝

*/
void new_test_p7_1_start_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 0);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_1);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_start_t test_start;
    test_start.condition = START_CONDITION_TRIGGER_IMMEDIATELY;

    esp_err_t err3 = new_test_set_start(&test_start);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_HOME);

    // RUN TEST
    test_setup_t *test_setup;
    new_test_move_setup(&test_setup);
    current_test_delete_test();
    current_test_add_new_test(test_setup);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p7_2_start_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 10);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint32_t value = (uint32_t)atoi((char *)addr);

    // VERIFY UNPACK ERR
    if (value == 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n TRIGGER VALUE");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_2);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_start_t test_start;
    test_start.condition = START_CONDITION_TIME_DELAY;
    test_start.parameters.time_delay.value = value;

    esp_err_t err3 = new_test_set_start(&test_start);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_HOME);

    // RUN TEST
    test_setup_t *test_setup;
    new_test_move_setup(&test_setup);
    current_test_delete_test();
    current_test_add_new_test(test_setup);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p7_3_start_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 23);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    first_point_taken_at_t first_point_taken_at;
    double value = atof((char *)addr + 2);
    double current_value = atof((char *)addr + 13);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);
    esp_err_t err2 = new_test_get_start_first_point_from_multiradio(*(addr + 1), &first_point_taken_at);

    // VERIFY UNPACK ERR
    if ((err1 != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n TRIGGER INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if ((err2 != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n FIRST POINT TAKEN AT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_start_t test_start;
    test_start.condition = START_CONDITION_GREATER_THAN;
    test_start.parameters.greater_than.sensor_index = sensor_index;
    test_start.parameters.greater_than.value = value;
    test_start.parameters.greater_than.current_value = current_value;
    test_start.parameters.greater_than.first_point_taken_at = first_point_taken_at;

    esp_err_t err3 = new_test_set_start(&test_start);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_HOME);

    // RUN TEST
    test_setup_t *test_setup;
    new_test_move_setup(&test_setup);
    current_test_delete_test();
    current_test_add_new_test(test_setup);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p7_3_trigger_input_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 1);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    esp_err_t err = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if ((err != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n TRIGGER INPUT*");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_3);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    err = new_test_start_GreaterThan_set_sensor_index(sensor_index);
    if ((err != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p7_4_start_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 23);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    first_point_taken_at_t first_point_taken_at;
    double value = atof((char *)addr + 2);
    double current_value = atof((char *)addr + 13);
    esp_err_t err1 = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);
    esp_err_t err2 = new_test_get_start_first_point_from_multiradio(*(addr + 1), &first_point_taken_at);

    // VERIFY UNPACK ERR
    if ((err1 != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n TRIGGER INPUT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }
    if ((err2 != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n FIRST POINT TAKEN AT");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    test_start_t test_start;
    test_start.condition = START_CONDITION_LESS_THAN;
    test_start.parameters.less_than.sensor_index = sensor_index;
    test_start.parameters.less_than.value = value;
    test_start.parameters.less_than.current_value = current_value;
    test_start.parameters.less_than.first_point_taken_at = first_point_taken_at;

    esp_err_t err3 = new_test_set_start(&test_start);
    if (err3 != ESP_OK)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // GOTO NEXT PAGE
    nextion_1_change_page(PAGE_HOME);

    // RUN TEST
    test_setup_t *test_setup;
    new_test_move_setup(&test_setup);
    current_test_delete_test();
    current_test_add_new_test(test_setup);

end:
    if (size_received > 0)
        free(msg->content.addr);
}
void new_test_p7_4_trigger_input_received_cb(msg_t *msg)
{
    // VERIFY PAYLOAD LENGTH
    int size_received = msg->size;
    bool size_ok = (size_received == 1);
    if (!size_ok)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nCOMUNICATION ERROR");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // UNPACK
    uint8_t *addr = msg->content.addr;
    uint8_t sensor_index;
    esp_err_t err = new_test_get_sensor_index_from_combobox(*addr, &sensor_index);

    // VERIFY UNPACK ERR
    if ((err != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\n TRIGGER INPUT*");
        nextion_1_message_write_nextpage(PAGE_NEW_TEST_P7_4);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

    // STORE DATA
    new_test_start_LessThan_set_sensor_index(sensor_index);
    if ((err != ESP_OK))
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        nextion_1_message_write_content("\r\nERROR:\r\nINTERNAL ERROR");
        nextion_1_message_write_nextpage(PAGE_HOME);
        nextion_1_message_write_content_color(NEXTION_COLOR_RED);
        nextion_1_change_page(PAGE_MESSAGE);
        goto end;
    }

end:
    if (size_received > 0)
        free(msg->content.addr);
}
/*
    ███████╗███████╗███╗   ██╗███████╗ ██████╗ ██████╗     ██╗   ██╗███╗   ██╗██╗████████╗     ██████╗██╗  ██╗ █████╗ ███╗   ██╗ ██████╗ ███████╗██████╗
    ██╔════╝██╔════╝████╗  ██║██╔════╝██╔═══██╗██╔══██╗    ██║   ██║████╗  ██║██║╚══██╔══╝    ██╔════╝██║  ██║██╔══██╗████╗  ██║██╔════╝ ██╔════╝██╔══██╗
    ███████╗█████╗  ██╔██╗ ██║███████╗██║   ██║██████╔╝    ██║   ██║██╔██╗ ██║██║   ██║       ██║     ███████║███████║██╔██╗ ██║██║  ███╗█████╗  ██║  ██║
    ╚════██║██╔══╝  ██║╚██╗██║╚════██║██║   ██║██╔══██╗    ██║   ██║██║╚██╗██║██║   ██║       ██║     ██╔══██║██╔══██║██║╚██╗██║██║   ██║██╔══╝  ██║  ██║
    ███████║███████╗██║ ╚████║███████║╚██████╔╝██║  ██║    ╚██████╔╝██║ ╚████║██║   ██║       ╚██████╗██║  ██║██║  ██║██║ ╚████║╚██████╔╝███████╗██████╔╝
    ╚══════╝╚══════╝╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝     ╚═════╝ ╚═╝  ╚═══╝╚═╝   ╚═╝        ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝╚═════╝

*/

void sensor_unit_changed(msg_t *msg)
{
    // UNPACK CONTENT
    content_str_t cs = (msg->content.cs);
    int index = cs.index;
    uint8_t unit_u8 = *(uint8_t *)(cs.str);
    free(cs.str);

    // VERYIFY UNIT IN RANGE
    sensor_type_t type = sensor_get_type(index);
    bool unit_in_range = helper_unit_in_range(unit_u8, type);
    if (!unit_in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        ESP_LOGE(TAG, "unit:%u", unit_u8);
        ESP_LOGE(TAG, "type:%u", type);
        ESP_LOGE(TAG, "index:%i", index);
        return;
    }

    // VERIFY INDEX IN RANGE
    bool index_in_range = helper_index_in_range(index);
    if (!index_in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        ESP_LOGE(TAG, "index:%i", index);
        return;
    }

    // CREATE NEW SENSOR CONFIGURATION && SET ON RAM
    sensor_unit_t new_unit = (sensor_unit_t)unit_u8;
    sensor_type_t old_unit = sensor_get_src_unit(index);
    double new_fc = helper_get_fc(old_unit, new_unit, sensor_get_type(index));
    sensor_config_t *config = sensor_get_configuration(index);
    config->unit_fc = new_fc;
    config->unit = new_unit;

    // SAVE NEW CONFIGURATION ON NVS
    config_sensor_save(index, config);

    // UPDATE SCREEN
    nextion_1_home_write_unit(index, sensor_get_unit_str(index));
    nextion_1_home_write_unitps(index, sensor_get_unitps_str(index));
    nextion_1_calibration_write_range(index, sensor_get_range(index));
    nextion_1_calibration_write_unit(index, sensor_get_unit_str(index));
}

/*
    ███╗   ███╗███████╗ ██████╗     ███████╗██████╗  ██████╗ ███╗   ███╗    ███████╗███████╗███╗   ██╗███████╗ ██████╗ ██████╗     ████████╗ █████╗ ███████╗██╗  ██╗
    ████╗ ████║██╔════╝██╔════╝     ██╔════╝██╔══██╗██╔═══██╗████╗ ████║    ██╔════╝██╔════╝████╗  ██║██╔════╝██╔═══██╗██╔══██╗    ╚══██╔══╝██╔══██╗██╔════╝██║ ██╔╝
    ██╔████╔██║███████╗██║  ███╗    █████╗  ██████╔╝██║   ██║██╔████╔██║    ███████╗█████╗  ██╔██╗ ██║███████╗██║   ██║██████╔╝       ██║   ███████║███████╗█████╔╝
    ██║╚██╔╝██║╚════██║██║   ██║    ██╔══╝  ██╔══██╗██║   ██║██║╚██╔╝██║    ╚════██║██╔══╝  ██║╚██╗██║╚════██║██║   ██║██╔══██╗       ██║   ██╔══██║╚════██║██╔═██╗
    ██║ ╚═╝ ██║███████║╚██████╔╝    ██║     ██║  ██║╚██████╔╝██║ ╚═╝ ██║    ███████║███████╗██║ ╚████║███████║╚██████╔╝██║  ██║       ██║   ██║  ██║███████║██║  ██╗
    ╚═╝     ╚═╝╚══════╝ ╚═════╝     ╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝    ╚══════╝╚══════╝╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝       ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝

*/

void sensor_reading_received_cb(msg_t *msg)
{
    // UNPACK
    content_reading_t cr = msg->content.cr;
    int index = cr.index;
    reading_t reading = cr.reading;

    // SET ON RAM
    sensor_set_data(index, &reading);
}
void sensor_class_received_cb(msg_t *msg)
{
    sensor_set_class(msg->content.cc.index, msg->content.cc.class);
}

/*
███╗   ██╗███████╗██╗  ██╗████████╗██╗ ██████╗ ███╗   ██╗    ████████╗██╗███╗   ███╗███████╗██████╗
████╗  ██║██╔════╝╚██╗██╔╝╚══██╔══╝██║██╔═══██╗████╗  ██║    ╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗
██╔██╗ ██║█████╗   ╚███╔╝    ██║   ██║██║   ██║██╔██╗ ██║       ██║   ██║██╔████╔██║█████╗  ██████╔╝
██║╚██╗██║██╔══╝   ██╔██╗    ██║   ██║██║   ██║██║╚██╗██║       ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗
██║ ╚████║███████╗██╔╝ ██╗   ██║   ██║╚██████╔╝██║ ╚████║       ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║
╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝       ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝

*/

void nextion_update_cb(msg_t *msg)
{

    // ESP_LOGI(TAG, "%s", __func__);
    page_t page = nextion_1_get_page();
    if (page == PAGE_HOME)
    {
        for (int index = 0; index < NUM_SENSORS; index++)
        {
            bool is_ready = sensor_is_ready(index);

            if (is_ready)
            {
                nextion_1_home_write_data(index, sensor_get_real_str(index));
                nextion_1_home_write_dataps(index, sensor_get_realps_str(index));
            }
        }
    }
    else if (page == PAGE_CALIBRATION)
    {
        for (int index = 0; index < NUM_SENSORS; index++)
        {
            bool is_ready = sensor_is_ready(index);
            bool is_enabled = sensor_is_enabled(index);

            if (is_ready)
            {
                nextion_1_calibration_write_value(index, sensor_get_real_str(index));
            }
            if (is_enabled)
            {
                nextion_1_calibration_write_counts(index, sensor_get_raw_str(index));
            }
        }
    }
    else if (page == PAGE_INPUTCALIBP3)
    {
        int index = inputCalib_get_index();

        // VERIFY SENSOR IS ENABLED
        bool is_enabled = sensor_is_enabled(index);
        RETURN_IF_TRUE(!is_enabled);

        // PRINT RAW IN ROW
        switch (inputCalib_get_row_to_fill())
        {
        case ROW_0_TO_FILL:
            nextion_1_inputcalibp3_write_raw0(sensor_get_raw_str(index));
            break;
        case ROW_1_TO_FILL:
            nextion_1_inputcalibp3_write_raw1(sensor_get_raw_str(index));
            break;
        case ROW_2_TO_FILL:
            nextion_1_inputcalibp3_write_raw2(sensor_get_raw_str(index));
            break;
        case ROW_3_TO_FILL:
            nextion_1_inputcalibp3_write_raw3(sensor_get_raw_str(index));
            break;
        case ROW_4_TO_FILL:
            nextion_1_inputcalibp3_write_raw4(sensor_get_raw_str(index));
            break;
        case ROW_5_TO_FILL:
            nextion_1_inputcalibp3_write_raw5(sensor_get_raw_str(index));
            break;
        case ROW_6_TO_FILL:
            nextion_1_inputcalibp3_write_raw6(sensor_get_raw_str(index));
            break;
        case ROW_7_TO_FILL:
            nextion_1_inputcalibp3_write_raw7(sensor_get_raw_str(index));
            break;
        case ROW_8_TO_FILL:
            nextion_1_inputcalibp3_write_raw8(sensor_get_raw_str(index));
            break;
        case ROW_9_TO_FILL:
            nextion_1_inputcalibp3_write_raw9(sensor_get_raw_str(index));
            break;
        case ROW_10_TO_FILL:
            nextion_1_inputcalibp3_write_raw10(sensor_get_raw_str(index));
            break;
        default:
            break;
        }
    }
    else if (page == PAGE_NEW_TEST_P7_3)
    {
        int index;
        new_test_start_GreaterThan_get_sensor_index(&index);
        bool is_enabled = sensor_is_enabled(index);
        bool is_calibrated = sensor_is_calibrated(index);
        bool is_free = sensor_is_free(index);

        char *content = (is_enabled && is_calibrated && is_free) ? sensor_get_real_str(index) : "";
        nextion_1_new_test_p7_3_write_current_value(content);
    }
    else if (page == PAGE_NEW_TEST_P7_4)
    {
        int index;
        new_test_start_LessThan_get_sensor_index(&index);
        bool is_enabled = sensor_is_enabled(index);
        bool is_calibrated = sensor_is_calibrated(index);
        bool is_free = sensor_is_free(index);

        char *content = (is_enabled && is_calibrated && is_free) ? sensor_get_real_str(index) : "";
        nextion_1_new_test_p7_4_write_current_value(content);
    }
    else if (page == PAGE_CURRENT_TEST_P3)
    {
    }
    else
    {
        ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    }
}

/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ████████╗██╗███╗   ███╗███████╗██████╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║          ██║   ██║██╔████╔██║█████╗  ██████╔╝
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║          ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║          ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝          ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝

*/
void test_timer_cb(msg_t *msg)
{
    for (int i = 0; i < CURRENT_TEST_NUM_MAX; i++)
    {
        bool is_configured = current_test_is_test_configured(i);
        if (!is_configured)
            continue;

        bool is_started = current_test_is_start_condition_done(i);
        if (!is_started)
        {
            // check start condition
            test_start_t start;
            switch (start.condition)
            {
            case START_CONDITION_TRIGGER_IMMEDIATELY:
                start_condition_done[i] = true;
                break;
            case START_CONDITION_TIME_DELAY:
                if (system_timestamp > setup_timestapm)
                {
                }
            case START_CONDITION_GREATER_THAN:
                int sensor_index = start.parameters.greater_than.sensor_index;
                double value = start.parameters.greater_than.value;
                double real = sensor_get_real(sensor_index);
                if (real > value)
                {
                }
            default:
                break;
            }
        }
        else
        {
            // check logging condition
            // check stop condition
        }
    }
}
