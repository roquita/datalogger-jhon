#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "project_tasks.h"
#include "sensor.h"
#include "gui.h"
#include "calibration.h"
#include "configuration.h"
#include "esp_err.h"
#include "testing.h"
#include "helpers.h"

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
        nextion_1_home_write_name(index, sensor_get_name(index));
        nextion_1_home_write_unit(index, sensor_get_unit_str(index));
        nextion_1_home_write_unitps(index, sensor_get_unitps_str(index));
        nextion_1_calibration_write_name(index, sensor_get_name(index));
        nextion_1_calibration_write_range(index, sensor_get_range(index));
        nextion_1_calibration_write_units_path(index, sensor_get_units_path(index));
        nextion_1_calibration_write_units_val(index, sensor_get_units_val(index));
        nextion_1_calibration_write_unit(index, sensor_get_unit_str(index));
        nextion_1_calibration_write_limit(index, sensor_is_limit_enabled(index));
        bool is_enabled = sensor_is_enabled(index);
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
void numpad_popup_loaded_cb(msg_t *msg)
{
    nextion_1_set_page(PAGE_NUMPAD_POPUP);
    nextion_1_stop_timer();
}
void new_test_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(PAGE_NEW_TEST);
    nextion_1_stop_timer();
}
void inputconfigp1_loaded_cb(msg_t *msg)
{
    ESP_LOGI(TAG, "%s", __func__);
    nextion_1_set_page(INPUTCONFIGP1_LOADED);
    nextion_1_stop_timer();
}

/*
████████╗ █████╗ ██████╗  █████╗
╚══██╔══╝██╔══██╗██╔══██╗██╔══██╗
   ██║   ███████║██████╔╝███████║
   ██║   ██╔══██║██╔══██╗██╔══██║
   ██║   ██║  ██║██║  ██║██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝

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
███████╗███████╗███╗   ██╗███████╗ ██████╗ ██████╗     ███████╗███╗   ██╗ █████╗ ██████╗ ██╗     ███████╗
██╔════╝██╔════╝████╗  ██║██╔════╝██╔═══██╗██╔══██╗    ██╔════╝████╗  ██║██╔══██╗██╔══██╗██║     ██╔════╝
███████╗█████╗  ██╔██╗ ██║███████╗██║   ██║██████╔╝    █████╗  ██╔██╗ ██║███████║██████╔╝██║     █████╗
╚════██║██╔══╝  ██║╚██╗██║╚════██║██║   ██║██╔══██╗    ██╔══╝  ██║╚██╗██║██╔══██║██╔══██╗██║     ██╔══╝
███████║███████╗██║ ╚████║███████║╚██████╔╝██║  ██║    ███████╗██║ ╚████║██║  ██║██████╔╝███████╗███████╗
╚══════╝╚══════╝╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝    ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝

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
    bool unit_in_range = helper_unit_in_range(unit_u8, sensor_get_type(index));
    if (!unit_in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // VERIFY INDEX IN RANGE
    bool index_in_range = helper_index_in_range(index);
    if (!index_in_range)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
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
    ██╗     ██╗███╗   ███╗██╗████████╗███████╗    ███████╗███╗   ██╗ █████╗ ██████╗ ██╗     ███████╗██████╗
    ██║     ██║████╗ ████║██║╚══██╔══╝██╔════╝    ██╔════╝████╗  ██║██╔══██╗██╔══██╗██║     ██╔════╝██╔══██╗
    ██║     ██║██╔████╔██║██║   ██║   ███████╗    █████╗  ██╔██╗ ██║███████║██████╔╝██║     █████╗  ██║  ██║
    ██║     ██║██║╚██╔╝██║██║   ██║   ╚════██║    ██╔══╝  ██║╚██╗██║██╔══██║██╔══██╗██║     ██╔══╝  ██║  ██║
    ███████╗██║██║ ╚═╝ ██║██║   ██║   ███████║    ███████╗██║ ╚████║██║  ██║██████╔╝███████╗███████╗██████╔╝
    ╚══════╝╚═╝╚═╝     ╚═╝╚═╝   ╚═╝   ╚══════╝    ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚═════╝

*/
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
██╗     ██╗███╗   ███╗██╗████████╗███████╗    ██████╗ ██╗███████╗ █████╗ ██████╗ ██╗     ███████╗██████╗
██║     ██║████╗ ████║██║╚══██╔══╝██╔════╝    ██╔══██╗██║██╔════╝██╔══██╗██╔══██╗██║     ██╔════╝██╔══██╗
██║     ██║██╔████╔██║██║   ██║   ███████╗    ██║  ██║██║███████╗███████║██████╔╝██║     █████╗  ██║  ██║
██║     ██║██║╚██╔╝██║██║   ██║   ╚════██║    ██║  ██║██║╚════██║██╔══██║██╔══██╗██║     ██╔══╝  ██║  ██║
███████╗██║██║ ╚═╝ ██║██║   ██║   ███████║    ██████╔╝██║███████║██║  ██║██████╔╝███████╗███████╗██████╔╝
╚══════╝╚═╝╚═╝     ╚═╝╚═╝   ╚═╝   ╚══════╝    ╚═════╝ ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚═════╝

*/
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
    else
    {
        ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    }
}

/*
    ███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗
    ████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝
    ██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║
    ██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║
    ██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║
    ╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝

*/

void test_execute_cb(msg_t *msg)
{
    // ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    bool is_runnig = test_is_running();
    RETURN_IF_TRUE(!is_runnig);

    int index1 = test_get_index1();
    int index2 = test_get_index2();
    uint8_t row = test_get_row();
    bool res = test_execute(sensor_get_real(index1), sensor_get_real(index2));
    if (res == true)
    {
        switch (row)
        {
        case 0:
            nextion_1_newtest_write_table01(sensor_get_real_str(index2));
            break;
        case 1:
            nextion_1_newtest_write_table11(sensor_get_real_str(index2));
            break;
        case 2:
            nextion_1_newtest_write_table21(sensor_get_real_str(index2));
            break;
        case 3:
            nextion_1_newtest_write_table31(sensor_get_real_str(index2));
            break;
        case 4:
            nextion_1_newtest_write_table41(sensor_get_real_str(index2));
            break;
        case 5:
            nextion_1_newtest_write_table51(sensor_get_real_str(index2));
            break;
        case 6:
            nextion_1_newtest_write_table61(sensor_get_real_str(index2));
            break;
        case 7:
            nextion_1_newtest_write_table71(sensor_get_real_str(index2));
            break;
        case 8:
            nextion_1_newtest_write_table81(sensor_get_real_str(index2));
            break;
        case 9:
            nextion_1_newtest_write_table91(sensor_get_real_str(index2));
            test_stop();
            break;
        default:
            break;
        }
    }
}
void new_test_set_index_1_cb(msg_t *msg)
{
    // ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    int index = *(uint8_t *)(msg->content.addr);
    test_set_index1(index);
    free(msg->content.addr);
}
void new_test_set_index2_cb(msg_t *msg)
{
    // ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    int index = *(uint8_t *)(msg->content.addr);
    test_set_index2(index);
    free(msg->content.addr);
}
void new_test_start_cb(msg_t *msg)
{
    // ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    test_start();
}
void new_test_stop_cb(msg_t *msg)
{
    // ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
    test_stop();
}
void new_test_download_cb(msg_t *msg)
{
    // ESP_LOGW(TAG, "%s:line %d", __FILE__, __LINE__);
}