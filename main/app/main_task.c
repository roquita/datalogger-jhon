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
        if (type > MIN_LOADED && type < MAX_LOADED)
        {
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
            else if (type == KEYBDA_LOADED)
                keybda_loaded_cb(&msg);
            else if (type == NUMPAD_POPUP_LOADED)
                numpad_popup_loaded_cb(&msg);
            else if (type == INTRO_LOADED)
                intro_loaded_cb(&msg);
            else if (type == INPUTCONFIGP1_LOADED)
                inputconfigp1_loaded_cb(&msg);
            else if (type == NEW_TEST_P1_LOADED)
                new_test_p1_loaded_cb(&msg);
            else if (type == NEW_TEST_P2_LOADED)
                new_test_p2_loaded_cb(&msg);
            else if (type == NEW_TEST_P3_1_LOADED)
                new_test_p3_1_loaded_cb(&msg);
            else if (type == NEW_TEST_P3_2_LOADED)
                new_test_p3_2_loadedcb(&msg);
            else if (type == NEW_TEST_P3_3_LOADED)
                new_test_p3_3_loadedcb(&msg);
            else if (type == NEW_TEST_P3_4_LOADED)
                new_test_p3_4_loadedcb(&msg);
            else if (type == LOG_SENSOR_TABLE_LOADED)
                log_sensor_table_loaded_cb(&msg);
            else if (type == LOG_TIME_TABLE_LOADED)
                log_time_table_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_1_LOADED)
                new_test_p4_1_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_2_LOADED)
                new_test_p4_2_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_3_LOADED)
                new_test_p4_3_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_4_LOADED)
                new_test_p4_4_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_5_LOADED)
                new_test_p4_5_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_6_LOADED)
                new_test_p4_6_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_7_LOADED)
                new_test_p4_7_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_8_LOADED)
                new_test_p4_8_loaded_cb(&msg);
            else if (type == NEW_TEST_P4_9_LOADED)
                new_test_p4_9_loaded_cb(&msg);
            else if (type == NEW_TEST_P5_LOADED)
                new_test_p5_loaded_cb(&msg);
            else if (type == NEW_TEST_P6_LOADED)
                new_test_p6_loaded_cb(&msg);
            else if (type == NEW_TEST_P7_1_LOADED)
                new_test_p7_1_loaded_cb(&msg);
            else if (type == NEW_TEST_P7_2_LOADED)
                new_test_p7_2_loaded_cb(&msg);
            else if (type == NEW_TEST_P7_3_LOADED)
                new_test_p7_3_loaded_cb(&msg);
            else if (type == NEW_TEST_P7_4_LOADED)
                new_test_p7_4_loaded_cb(&msg);
            else if (type == PREVIOUS_TEST_P1_LOADED)
                previous_test_p1_loaded_cb(&msg);
            else if (type == PREVIOUS_TEST_P2_LOADED)
                previous_test_p2_loaded_cb(&msg);
            else if (type == PREVIOUS_TEST_P3_LOADED)
                previous_test_p3_loaded_cb(&msg);
            else if (type == EXPORT_TEST_LOADED)
                export_test_loaded_cb(&msg);
            else if (type == DELETE_TEST_LOADED)
                delete_test_loaded_cb(&msg);
            else if (type == MESSAGE_LOADED)
                message_loaded_cb(&msg);
            else if (type == CURRENT_TEST_P1_LOADED)
                current_test_p1_loaded_cb(&msg);
            else if (type == CURRENT_TEST_P2_LOADED)
                current_test_p2_loaded_cb(&msg);
            else if (type == CURRENT_TEST_P3_LOADED)
                current_test_p3_loaded_cb(&msg);
        }

        // HOME
        else if (type == TARA_ENABLED)
            tara_enabled_cb(&msg);
        else if (type == TARA_DISABLED)
            tara_disabled_cb(&msg);

        // CALIBRATION
        else if (type == SENSOR_DISABLED)
            sensor_disabled_cb(&msg);
        else if (type == SENSOR_ENABLED)
            sensor_enabled_cb(&msg);
        else if (type == LIMITS_DISABLED)
            sensor_limits_disabled(&msg);
        else if (type == LIMITS_ENABLED)
            sensor_limits_enabled(&msg);

        // INPUT CALIBRATION
        else if (type > INPUT_CALIBRATION_MIN && type < INPUT_CALIBRATION_MAX)
        {
            // INPUT CALIB P1
            if (type == INPUTCALIBP1_TYPE_RECEIVED)
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
        }

        // NEW TEST
        else if (type > NEW_TEST_MIN && type < NEW_TEST_MAX)
        {
            if (type == NEW_TEST_P1_TYPE_RECEIVED)
                new_test_p1_type_received(&msg);
            else if (type == NEW_TEST_P2_INPUTS_RECEIVED)
                new_test_p2_inputs_received(&msg);
            else if (type == NEW_TEST_P3_1_LOGGING_RECEIVED)
                new_test_p3_1_logging_received_cb(&msg);
            else if (type == NEW_TEST_P3_2_LOGGING_RECEIVED)
                new_test_p3_2_logging_received_cb(&msg);
            else if (type == NEW_TEST_P3_3_LOGGING_RECEIVED)
                new_test_p3_3_logging_received_cb(&msg);
            else if (type == NEW_TEST_P3_4_LOGGING_RECEIVED)
                new_test_p3_4_logging_received_cb(&msg);
            else if (type == NEW_TEST_P4_1_STOP_RECEIVED)
                new_test_p4_1_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_2_STOP_RECEIVED)
                new_test_p4_2_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_3_STOP_RECEIVED)
                new_test_p4_3_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_4_STOP_RECEIVED)
                new_test_p4_4_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_5_STOP_RECEIVED)
                new_test_p4_5_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_6_STOP_RECEIVED)
                new_test_p4_6_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_7_STOP_RECEIVED)
                new_test_p4_7_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_8_STOP_RECEIVED)
                new_test_p4_8_stop_received_cb(&msg);
            else if (type == NEW_TEST_P4_9_STOP_RECEIVED)
                new_test_p4_9_stop_received_cb(&msg);
            else if (type == NEW_TEST_P6_GRAPH_RECEIVED)
                new_test_p6_graph_received_cb(&msg);
            else if (type == NEW_TEST_P7_1_START_RECEIVED)
                new_test_p7_1_start_received_cb(&msg);
            else if (type == NEW_TEST_P7_2_START_RECEIVED)
                new_test_p7_2_start_received_cb(&msg);
            else if (type == NEW_TEST_P7_3_START_RECEIVED)
                new_test_p7_3_start_received_cb(&msg);
            else if (type == NEW_TEST_P7_3_TRIGGER_INPUT_RECEIVED)
                new_test_p7_3_trigger_input_received_cb(&msg);
            else if (type == NEW_TEST_P7_4_START_RECEIVED)
                new_test_p7_4_start_received_cb(&msg);
            else if (type == NEW_TEST_P7_4_TRIGGER_INPUT_RECEIVED)
                new_test_p7_4_trigger_input_received_cb(&msg);
        }

        // SENSOR UNIT CHANGED
        else if (type == SENSOR_UNIT_CHANGED)
            sensor_unit_changed(&msg);

        // MSG FROM SENSOR TASK
        else if (type == SENSOR_READING_RECEIVED)
            sensor_reading_received_cb(&msg);
        else if (type == SENSOR_CLASS_RECEIVED)
            sensor_class_received_cb(&msg);

        // NEXTION-TIMER
        else if (type == NEXTION_UPDATE)
            nextion_update_cb(&msg);

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