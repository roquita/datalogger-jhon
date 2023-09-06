#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "main_callback.h"
#include "sensor.h"
#include "project_tasks.h"
#include "project_defines.h"
#include "sensor.h"
#include "driver/gpio.h"

#define TAG "MAIN_TASK"

static msg_t msg;

void main_task(void *arg)
{
    ESP_LOGI(TAG, TAG);
    main_task_init();

    while (1)
    {
        xQueueReceive(main_queue, &msg, portMAX_DELAY);
        msg_type_t type = msg.type;

        // PAGES LOADED
        if (type == HOME_LOADED)
            home_loaded_cb(&msg);
        else if (type == SYSTEM_SETTINGS_LOADED)
            system_settings_loaded_cb(&msg);
        else if (type == CALIBRATION_LOADED)
            calibration_loaded_cb(&msg);
        else if (type == INPUTCALIBP1_LOADED)
            inputcalibp1_loaded_cb(&msg);
        else if (type == INPUTCALIBP2_LOADED)
            inputcalibp2_loaded_cb(&msg);
        else if (type == INPUTCALIBP3_LOADED)
            inputcalibp3_loaded_cb(&msg);
        else if (type == INPUTCALIBP4_LOADED)
            inputcalibp4_loaded_cb(&msg);
        else if (type == NUMPAD_POPUP_LOADED)
            numpad_popup_loaded_cb(&msg);
        else if (type == NEW_TEST_LOADED)
            new_test_loaded_cb(&msg);
        else if (type == INPUTCONFIGP1_LOADED)
            inputconfigp1_loaded_cb(&msg);

        // SENSOR TARA
        else if (type == TARA_ENABLED)
            tara_enabled_cb(&msg);
        else if (type == TARA_DISABLED)
            tara_disabled_cb(&msg);

        // SENSOR ENABLE
        else if (type == SENSOR_DISABLED)
            sensor_disabled_cb(&msg);
        else if (type == SENSOR_ENABLED)
            sensor_enabled_cb(&msg);

        // INPUT CALIB P1
        else if (type == INPUTCALIBP1_TYPE_RECEIVED)
            inputcalibp1_type_received_cb(&msg);
        else if (type == INPUTCALIBP1_CAPACITY_RECEIVED)
            inputcalibp1_capacity_received_cb(&msg);
        else if (type == INPUTCALIBP1_CAPACITY_UNIT_RECEIVED)
            inputcalibp1_capacity_unit_received_cb(&msg);
        else if (type == INPUTCALIBP1_SENSIBILITY_RECEIVED)
            inputcalibp1_sensibility_received_cb(&msg);
        else if (type == INPUTCALIBP1_SENSIBILITY_UNIT_RECEIVED)
            inputcalibp1_sensibility_unit_received_cb(&msg);
        else if (type == INPUTCALIBP1_SENSOR_INDEX_RECEIVED)
            inputcalibp1_sensor_index_received(&msg);
        // INPUT CALIB P2
        else if (type == INPUTCALIBP2_CALIBRATION_LIMIT_RECEIVED)
            inputcalibp2_calibration_limit_received_cb(&msg);
        else if (type == INPUTCALIBP2_LIMIT_ENABLE_RECEIVED)
            inputcalibp2_limit_enable_receive_cb(&msg);
        // INPUT CALIB P3
        else if (type == INPUTCALIBP3_TABLE_RECEIVED)
            inputcalibp3_table_received(&msg);
        else if (type == INPUTCALIBP3_NUM_POINTS_RECEIVED)
            inputcalibp3_num_points_received(&msg);
        else if (type == INPUTCALIBP3_ROW_TO_FILL_RECEIVED)
            inputcalibp3_row_to_fill_received(&msg);
        // INPUT CALIB P4
        else if (type == INPUTCALIBP4_NAME_RECEIVED)
            inputcalibp4_name_received_cb(&msg);
        else if (type == INPUTCALIBP4_SAVE_PRESSED)
            inputcalibp4_save_pressed_cb(&msg);
        // INPUT CONFIGURATION P1
        else if (type == INPUTCONFIGP1_TYPE_RECEIVED)
            inputconfigp1_type_received_cb(&msg);
        else if (type == INPUTCONFIGP1_CAPACITY_RECEIVED)
            inputconfigp1_capacity_received_cb(&msg);
        else if (type == INPUTCONFIGP1_CAPACITY_UNIT_RECEIVED)
            inputconfigp1_capacity_unit_received_cb(&msg);
        else if (type == INPUTCONFIGP1_NAME_RECEIVED)
            inputconfigp1_name_received_cb(&msg);
        else if (type == INPUTCONFIGP1_INDEX_RECEIVED)
            inputconfigp1_index_received_cb(&msg);
        else if (type == INPUTCONFIGP1_SAVE_PRESSED)
            inputconfigp1_save_pressed_cb(&msg);

        // SENSOR UNIT CHANGED
        else if (type == SENSOR_UNIT_CHANGED)
            sensor_unit_changed(&msg);

        // LIMITS ENABLED
        else if (type == LIMITS_ENABLED)
            sensor_limits_enabled(&msg);
        // LIMITS DISABLED
        else if (type == LIMITS_DISABLED)
            sensor_limits_disabled(&msg);

        // MSG FROM SENSOR TASK
        else if (type == SENSOR_READING_RECEIVED)
            sensor_reading_received_cb(&msg);
        else if (type == SENSOR_CLASS_RECEIVED)
            sensor_class_received_cb(&msg);

        // NEXTION-TIMER
        else if (type == NEXTION_UPDATE)
            nextion_update_cb(&msg);

        // NEW TEST
        else if (type == TEST_EXECUTE)
            test_execute_cb(&msg);
        else if (type == NEW_TEST_SET_INDEX1)
            new_test_set_index_1_cb(&msg);
        else if (type == NEW_TEST_SET_INDEX2)
            new_test_set_index2_cb(&msg);
        else if (type == NEW_TEST_START)
            new_test_start_cb(&msg);
        else if (type == NEW_TEST_STOP)
            new_test_stop_cb(&msg);
        else if (type == NEW_TEST_DOWNLOAD)
            new_test_download_cb(&msg);

        else
        {
            ESP_LOGE(TAG, "file:%s,line:%u", __FILE__, __LINE__);
        }
    }
}
/*
1 2 3 4
5 6 7 8
*/