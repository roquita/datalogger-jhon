
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "project_defines.h"
#include "nextion_async.h"
#include "project_tasks.h"
#include "project_typedefs.h"
#include "esp_log.h"
#include "gui.h"

#define TAG "NEXTION_1"

IdSx_t IdSx[NUM_SENSORS] = {
    {
        .home_Name = 3,
        .home_Data = 6,
        .home_Dataps = 8,
        .home_Unit = 7,
        .home_Unitps = 9,
        .home_Tara = 4,
        .home_Status = 5,
        .calibration_Switch = 4,
        .calibration_Name = 2,
        .calibration_Range = 3,
        .calibration_Limit = 1,
        .calibration_Units = 77,
        .calibration_Value = 10,
        .calibration_Counts = 11,
        .calibration_Calibrate = 56,
        .currenttestp1_Name = 34,
        .currenttestp1_Data = 35,
        .currenttestp1_Dataps = 37,
        .currenttestp1_Unit = 36,
        .currenttestp1_Unitps = 38,
        .currenttestp2_Name = 12,
        .currenttestp2_Unit = 38,
        .currenttestp2_Data = 22,
    },
    {
        .home_Name = 10,
        .home_Data = 13,
        .home_Dataps = 15,
        .home_Unit = 14,
        .home_Unitps = 16,
        .home_Tara = 11,
        .home_Status = 12,
        .calibration_Switch = 16,
        .calibration_Name = 14,
        .calibration_Range = 15,
        .calibration_Limit = 13,
        .calibration_Units = 76,
        .calibration_Value = 17,
        .calibration_Counts = 18,
        .calibration_Calibrate = 57,
        .currenttestp1_Name = 39,
        .currenttestp1_Data = 40,
        .currenttestp1_Dataps = 42,
        .currenttestp1_Unit = 41,
        .currenttestp1_Unitps = 43,
        .currenttestp2_Name = 13,
        .currenttestp2_Unit = 39,
        .currenttestp2_Data = 23,
    },
    {
        .home_Name = 17,
        .home_Data = 20,
        .home_Dataps = 22,
        .home_Unit = 21,
        .home_Unitps = 23,
        .home_Tara = 18,
        .home_Status = 19,
        .calibration_Switch = 22,
        .calibration_Name = 20,
        .calibration_Range = 21,
        .calibration_Limit = 19,
        .calibration_Units = 75,
        .calibration_Value = 23,
        .calibration_Counts = 24,
        .calibration_Calibrate = 58,
        .currenttestp1_Name = 44,
        .currenttestp1_Data = 45,
        .currenttestp1_Dataps = 47,
        .currenttestp1_Unit = 46,
        .currenttestp1_Unitps = 48,
        .currenttestp2_Name = 14,
        .currenttestp2_Unit = 40,
        .currenttestp2_Data = 24,
    },
    {
        .home_Name = 24,
        .home_Data = 27,
        .home_Dataps = 29,
        .home_Unit = 28,
        .home_Unitps = 30,
        .home_Tara = 25,
        .home_Status = 26,
        .calibration_Switch = 28,
        .calibration_Name = 26,
        .calibration_Range = 27,
        .calibration_Limit = 25,
        .calibration_Units = 74,
        .calibration_Value = 29,
        .calibration_Counts = 30,
        .calibration_Calibrate = 59,
        .currenttestp1_Name = 49,
        .currenttestp1_Data = 50,
        .currenttestp1_Dataps = 52,
        .currenttestp1_Unit = 51,
        .currenttestp1_Unitps = 53,
        .currenttestp2_Name = 15,
        .currenttestp2_Unit = 41,
        .currenttestp2_Data = 25,
    },
    {
        .home_Name = 31,
        .home_Data = 34,
        .home_Dataps = 36,
        .home_Unit = 35,
        .home_Unitps = 37,
        .home_Tara = 32,
        .home_Status = 33,
        .calibration_Switch = 34,
        .calibration_Name = 32,
        .calibration_Range = 33,
        .calibration_Limit = 31,
        .calibration_Units = 73,
        .calibration_Value = 35,
        .calibration_Counts = 36,
        .calibration_Calibrate = 60,
        .currenttestp1_Name = 14,
        .currenttestp1_Data = 15,
        .currenttestp1_Dataps = 17,
        .currenttestp1_Unit = 16,
        .currenttestp1_Unitps = 18,
        .currenttestp2_Name = 16,
        .currenttestp2_Unit = 42,
        .currenttestp2_Data = 26,
    },
    {
        .home_Name = 38,
        .home_Data = 41,
        .home_Dataps = 43,
        .home_Unit = 42,
        .home_Unitps = 44,
        .home_Tara = 39,
        .home_Status = 40,
        .calibration_Switch = 40,
        .calibration_Name = 38,
        .calibration_Range = 39,
        .calibration_Limit = 37,
        .calibration_Units = 72,
        .calibration_Value = 41,
        .calibration_Counts = 42,
        .calibration_Calibrate = 61,
        .currenttestp1_Name = 19,
        .currenttestp1_Data = 20,
        .currenttestp1_Dataps = 22,
        .currenttestp1_Unit = 21,
        .currenttestp1_Unitps = 23,
        .currenttestp2_Name = 17,
        .currenttestp2_Unit = 43,
        .currenttestp2_Data = 27,
    },
    {
        .home_Name = 45,
        .home_Data = 48,
        .home_Dataps = 50,
        .home_Unit = 49,
        .home_Unitps = 51,
        .home_Tara = 46,
        .home_Status = 47,
        .calibration_Switch = 46,
        .calibration_Name = 44,
        .calibration_Range = 45,
        .calibration_Limit = 43,
        .calibration_Units = 71,
        .calibration_Value = 47,
        .calibration_Counts = 48,
        .calibration_Calibrate = 62,
        .currenttestp1_Name = 24,
        .currenttestp1_Data = 25,
        .currenttestp1_Dataps = 27,
        .currenttestp1_Unit = 26,
        .currenttestp1_Unitps = 28,
        .currenttestp2_Name = 20,
        .currenttestp2_Unit = 44,
        .currenttestp2_Data = 28,
    },
    {
        .home_Name = 52,
        .home_Data = 55,
        .home_Dataps = 57,
        .home_Unit = 56,
        .home_Unitps = 58,
        .home_Tara = 53,
        .home_Status = 54,
        .calibration_Switch = 52,
        .calibration_Name = 50,
        .calibration_Range = 51,
        .calibration_Limit = 49,
        .calibration_Units = 70,
        .calibration_Value = 53,
        .calibration_Counts = 54,
        .calibration_Calibrate = 63,
        .currenttestp1_Name = 29,
        .currenttestp1_Data = 30,
        .currenttestp1_Dataps = 32,
        .currenttestp1_Unit = 31,
        .currenttestp1_Unitps = 33,
        .currenttestp2_Name = 21,
        .currenttestp2_Unit = 45,
        .currenttestp2_Data = 29,
    }};

/*
███╗   ██╗███████╗██╗  ██╗████████╗██╗ ██████╗ ███╗   ██╗     ██████╗ ██████╗ ██████╗ ███████╗
████╗  ██║██╔════╝╚██╗██╔╝╚══██╔══╝██║██╔═══██╗████╗  ██║    ██╔════╝██╔═══██╗██╔══██╗██╔════╝
██╔██╗ ██║█████╗   ╚███╔╝    ██║   ██║██║   ██║██╔██╗ ██║    ██║     ██║   ██║██████╔╝█████╗
██║╚██╗██║██╔══╝   ██╔██╗    ██║   ██║██║   ██║██║╚██╗██║    ██║     ██║   ██║██╔══██╗██╔══╝
██║ ╚████║███████╗██╔╝ ██╗   ██║   ██║╚██████╔╝██║ ╚████║    ╚██████╗╚██████╔╝██║  ██║███████╗
╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝     ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝

*/

void data_rcv_1_cb(uint8_t *buffer, int size)
{
    ESP_LOGW(TAG, "file:%s,line:%i", __FILE__, __LINE__);
    ESP_LOGI(TAG, "%*s", size, (char *)buffer);
}
void code_rcv_1_cb(nextion_cmd_t *cmd)
{
    nextion_cmd_id_t cmd_id = cmd->id;
    if (cmd_id == TOUCH_EVENT_ID)
    {
        uint8_t page = cmd->touch_event.page;
        uint8_t component_id = cmd->touch_event.component_id;
        uint8_t event = cmd->touch_event.event;

        if (page == PAGE_HOME)
        {
            // HOME LOADED
            if (component_id == 0x00)
                SEND2FRONT_EMPTY_MSG(main_queue, HOME_LOADED, portMAX_DELAY)
            // HOME - TARA 1 BUTTON
            else if (component_id == 0x04 && event == 0x03)
                SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_1_INDEX, portMAX_DELAY)
            else if (component_id == 0x04 && event == 0x04)
                SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_1_INDEX, portMAX_DELAY)
            // HOME - TARA 2 BUTTON
            else if (component_id == 0x0B && event == 0x03)
                SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_2_INDEX, portMAX_DELAY)
            else if (component_id == 0x0B && event == 0x04)
                SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_2_INDEX, portMAX_DELAY)
            // HOME - TARA 3 BUTTON
            else if (component_id == 0x12 && event == 0x03)
                SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_3_INDEX, portMAX_DELAY)
            else if (component_id == 0x12 && event == 0x04)
                SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_3_INDEX, portMAX_DELAY)
            // HOME - TARA 4 BUTTON
            else if (component_id == 0x19 && event == 0x03)
                SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_4_INDEX, portMAX_DELAY)
            else if (component_id == 0x19 && event == 0x04)
                SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_4_INDEX, portMAX_DELAY)
            else if (component_id == 0x3B && event == 0x01)
                SEND_EMPTY_MSG(main_queue, GOTO_NEW_TEST_FROM_HOME, portMAX_DELAY)
            else if (component_id == 0x3B && event == 0x02)
                SEND_EMPTY_MSG(main_queue, GOTO_CURRENT_TEST_FROM_HOME, portMAX_DELAY)
        }
        else if (page == PAGE_SYSTEM_SETTINGS)
        {
            // SYSTEM SETTINGS LOADED
            if (component_id == 0x00)
                SEND2FRONT_EMPTY_MSG(main_queue, SYSTEM_SETTINGS_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_CALIBRATION)
        {
            // CALIBRATION LOADED
            if (component_id == 0x00)
                SEND2FRONT_EMPTY_MSG(main_queue, CALIBRATION_LOADED, portMAX_DELAY)
            // CALIBRATION - SWITCH 1
            else if (component_id == 0x04 && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_1_INDEX, portMAX_DELAY)
            else if (component_id == 0x04 && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_1_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 2
            else if (component_id == 0x10 && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_2_INDEX, portMAX_DELAY)
            else if (component_id == 0x10 && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_2_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 3
            else if (component_id == 0x16 && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_3_INDEX, portMAX_DELAY)
            else if (component_id == 0x16 && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_3_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 4
            else if (component_id == 0x1C && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_4_INDEX, portMAX_DELAY)
            else if (component_id == 0x1C && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_4_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 5
            else if (component_id == 0x22 && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_5_INDEX, portMAX_DELAY)
            else if (component_id == 0x22 && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_5_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 6
            else if (component_id == 0x28 && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_6_INDEX, portMAX_DELAY)
            else if (component_id == 0x28 && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_6_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 7
            else if (component_id == 0x2E && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_7_INDEX, portMAX_DELAY)
            else if (component_id == 0x2E && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_7_INDEX, portMAX_DELAY)
            // CALIBRATION - SWITCH 8
            else if (component_id == 0x34 && event == 0x02)
                SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_8_INDEX, portMAX_DELAY)
            else if (component_id == 0x34 && event == 0x03)
                SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_8_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 1
            else if (component_id == 0x01 && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_1_INDEX, portMAX_DELAY)
            else if (component_id == 0x01 && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_1_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 2
            else if (component_id == 0x0D && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_2_INDEX, portMAX_DELAY)
            else if (component_id == 0x0D && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_2_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 3
            else if (component_id == 0x13 && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_3_INDEX, portMAX_DELAY)
            else if (component_id == 0x13 && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_3_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 4
            else if (component_id == 0x19 && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_4_INDEX, portMAX_DELAY)
            else if (component_id == 0x19 && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_4_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 5
            else if (component_id == 0x1F && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_5_INDEX, portMAX_DELAY)
            else if (component_id == 0x1F && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_5_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 6
            else if (component_id == 0x25 && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_6_INDEX, portMAX_DELAY)
            else if (component_id == 0x25 && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_6_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 7
            else if (component_id == 0x2B && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_7_INDEX, portMAX_DELAY)
            else if (component_id == 0x2B && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_7_INDEX, portMAX_DELAY)
            // CALIBRATION - LIMITS 8
            else if (component_id == 0x31 && event == 0x02)
                SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_8_INDEX, portMAX_DELAY)
            else if (component_id == 0x31 && event == 0x03)
                SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_8_INDEX, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP1)
        {
            // INPUTCALIBP1 LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, INPUTCALIBP1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP2)
        {
            // INPUTCALIBP2 LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, INPUTCALIBP2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP3)
        {
            // INPUTCALIBP3 LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, INPUTCALIBP3_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP4)
        {
            // INPUTCALIBP4 LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, INPUTCALIBP4_LOADED, portMAX_DELAY)
            // INPUTCALIBP4 SAVE
            else if (component_id == 0x0A)
                SEND_EMPTY_MSG(main_queue, INPUTCALIBP4_SAVE_PRESSED, portMAX_DELAY)
        }
        else if (page == PAGE_KEYBDA)
        {
            // INPUTCALIBP4 LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, KEYBDA_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NUMPAD_POPUP)
        {
            // NUMPAP POPUP LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NUMPAD_POPUP_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_INTRO)
        {
            // NUMPAP POPUP LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, INTRO_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCONFIGP1)
        {
            // INPUT CONFIGURATION P1 LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, INPUTCONFIGP1_LOADED, portMAX_DELAY)
            // INPUTCONFIGP1 SAVE
            else if (component_id == 0x0D)
                SEND_EMPTY_MSG(main_queue, INPUTCONFIGP1_SAVE_PRESSED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P1)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P2)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_1)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P3_1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_2)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P3_2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_3)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P3_3_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_4)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P3_4_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_LOG_SENSOR_TABLE)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, LOG_SENSOR_TABLE_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_LOG_TIME_TABLE)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, LOG_TIME_TABLE_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_1)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_2)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_3)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_3_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_4)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_4_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_5)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_5_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_6)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_6_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_7)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_7_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_8)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_8_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_9)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P4_9_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P5)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P5_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P6)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P6_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_1)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P7_1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_2)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P7_2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_3)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P7_3_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_4)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, NEW_TEST_P7_4_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_PREVIOUS_TEST_P1)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, PREVIOUS_TEST_P1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_PREVIOUS_TEST_P2)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, PREVIOUS_TEST_P2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_PREVIOUS_TEST_P3)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, PREVIOUS_TEST_P3_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_EXPORT_TEST)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, EXPORT_TEST_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_DELETE_TEST)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, DELETE_TEST_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_MESSAGE)
        {
            //  LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, MESSAGE_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_CURRENT_TEST_P1)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, CURRENT_TEST_P1_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_CURRENT_TEST_P2)
        {
            //   LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, CURRENT_TEST_P2_LOADED, portMAX_DELAY)
        }
        else if (page == PAGE_CURRENT_TEST_P3)
        {
            // LOADED
            if (component_id == 0x00)
                SEND_EMPTY_MSG(main_queue, CURRENT_TEST_P3_LOADED, portMAX_DELAY)
        }

        // UNKNOWN TOUCH EVENT
        else
        {
            ESP_LOGW(TAG, "file:%s,line:%u", __FILE__, __LINE__);
            ESP_LOGW(TAG, "page:0x%02x || component_id:0x%02x || event:0x%02x", page, component_id, event);
        }
    }
    else if (cmd_id == NUMERIC_DATA_ID)
    {
        int32_t data = cmd->numeric_data.data;
        SEND_I32_MSG(main_queue, TIME_RTC_DATA_RECEIVED, data, portMAX_DELAY);
    }
    else if (cmd_id == CUSTOM_EVENT_ID)
    {
        uint8_t page = cmd->custom_event.page;
        uint8_t component_id = cmd->custom_event.component_id;
        void *data = cmd->custom_event.data;
        int data_len = cmd->custom_event.data_len;

        if (page == PAGE_CALIBRATION)
        {
            // CALIBRATION SENSOR 1 UNIT CHANGED
            if (component_id == 0x4D)
                SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_1_INDEX, portMAX_DELAY)
            // CALIBRATION SENSOR 2 UNIT CHANGED
            else if (component_id == 0x4C)
                SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_2_INDEX, portMAX_DELAY)
            // CALIBRATION SENSOR 3  UNIT CHANGED
            else if (component_id == 0x4B)
                SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_3_INDEX, portMAX_DELAY)
            // CALIBRATION SENSOR 4  UNIT CHANGED
            else if (component_id == 0x4A)
                SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_4_INDEX, portMAX_DELAY)
        }
        else if (page == PAGE_DATETIME)
        {
            if (component_id == 0x12)
                SEND_ADDR_MSG(main_queue, DATETIME_RTC_ALL_AT_ONCE_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP1)
        {
            // INPUTCALIBP1  TYPE RECEIVED
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP1_TYPE_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP1  CAPACITY RECEIVED
            else if (component_id == 0x09)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP1_CAPACITY_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP1  CAPACITY UNIT RECEIVED
            else if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP1_CAPACITY_UNIT_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP1  SENSIBILITY RECEIVED
            else if (component_id == 0x0A)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP1_SENSIBILITY_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP1  SENSIBILITY UNIT RECEIVED
            else if (component_id == 0x0B)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP1_SENSIBILITY_UNIT_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP1 SENSOR INDEX RECEIVED
            else if (component_id == 0x13)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP1_SENSOR_INDEX_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP2)
        {
            // INPUTCALIBP2 CALIBRATION LIMIT RECEIVED
            if (component_id == 0x08)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP2_CALIBRATION_LIMIT_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP2  LIMIT ENABLE RECEIVED
            else if (component_id == 0x0D)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP2_LIMIT_ENABLE_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP3)
        {
            // INPUTCALIBP3 TABLE RECEIVED
            if (component_id == 0x1C)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP3_TABLE_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP3 NUM POINTS RECEIVED
            else if (component_id == 0x32)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP3_NUM_POINTS_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCALIBP3 ROW TO FILL RECEIVED
            else if (component_id == 0x0C)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP3_ROW_TO_FILL_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCALIBP4)
        {
            // INPUTCALIBP4 NAME RECEIVED
            if (component_id == 0x08)
                SEND_ADDR_MSG(main_queue, INPUTCALIBP4_NAME_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_INPUTCONFIGP1)
        {
            // INPUTCONFIGP1 TYPE RECEIVED
            if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_TYPE_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCONFIGP1 CAPACITY RECEIVED
            else if (component_id == 0x04)
                SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_CAPACITY_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCONFIGP1 CAPACITY UNIT RECEIVED
            else if (component_id == 0x0E)
                SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_CAPACITY_UNIT_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCONFIGP1 NAME RECEIVED
            else if (component_id == 0x09)
                SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_NAME_RECEIVED, data, data_len, portMAX_DELAY)
            // INPUTCONFIGP1  INDEX RECEIVED
            else if (component_id == 0x11)
                SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_INDEX_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P1)
        {
            if (component_id == 0x0C)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P1_TYPE_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P2)
        {
            if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P2_INPUTS_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_1)
        {
            if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P3_1_LOGGING_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_2)
        {
            if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P3_2_LOGGING_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_3)
        {
            if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P3_3_LOGGING_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P3_4)
        {
            if (component_id == 0x0F)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P3_4_LOGGING_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_1)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_1_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_2)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_2_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_3)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_3_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_4)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_4_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_5)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_5_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_6)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_6_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_7)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_7_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_8)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_8_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P4_9)
        {
            if (component_id == 0x10)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P4_9_STOP_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P6)
        {
            if (component_id == 0x12)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P6_GRAPH_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_1)
        {
            if (component_id == 0x0C)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P7_1_START_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_2)
        {
            if (component_id == 0x0C)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P7_2_START_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_3)
        {
            if (component_id == 0x0C)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P7_3_START_RECEIVED, data, data_len, portMAX_DELAY)
            else if (component_id == 0x22)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P7_3_TRIGGER_INPUT_RECEIVED, data, data_len, portMAX_DELAY)
        }
        else if (page == PAGE_NEW_TEST_P7_4)
        {
            if (component_id == 0x0C)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P7_4_START_RECEIVED, data, data_len, portMAX_DELAY)
            else if (component_id == 0x22)
                SEND_ADDR_MSG(main_queue, NEW_TEST_P7_4_TRIGGER_INPUT_RECEIVED, data, data_len, portMAX_DELAY)
        }
        // UNKNOWN CUSTOM EVENT
        else
        {
            ESP_LOGW(TAG, "file:%s,line:%u", __FILE__, __LINE__);
            ESP_LOGW(TAG, "page:0x%02x || component_id:0x%02x || data:%s", page, component_id, (char *)data);
        }
    }
    else
    {
        ESP_LOGW(TAG, "file:%s,line:%u", __FILE__, __LINE__);
        ESP_LOGW(TAG, "cmd_id: 0x%02x ", cmd_id);
    }
}

/*
 ██████╗ ███████╗███╗   ██╗███████╗██████╗  █████╗ ██╗
██╔════╝ ██╔════╝████╗  ██║██╔════╝██╔══██╗██╔══██╗██║
██║  ███╗█████╗  ██╔██╗ ██║█████╗  ██████╔╝███████║██║
██║   ██║██╔══╝  ██║╚██╗██║██╔══╝  ██╔══██╗██╔══██║██║
╚██████╔╝███████╗██║ ╚████║███████╗██║  ██║██║  ██║███████╗
 ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝

*/
nextion_t nextion_1 = {
    .rx_queue_size = 10,
    .uart_buffer_size = 1024,
    .uRX_nTX_pin = uRX_nTX_PIN,
    .uTX_nRX_pin = uTX_nRX_PIN,
    .uart_num = NEXTION_UART_NUM,
    .uart_baudrate = NEXTION_BAUDRATE,
    .data_rcv_cb = data_rcv_1_cb,
    .code_rcv_cb = code_rcv_1_cb,
};

page_t page = NEXTION_PRE_INIT_PAGE;

void nextion_1_init_hardware()
{
    nextion_init(&nextion_1);
}

void nextion_1_set_page(page_t new_page)
{
    page = new_page;
}

page_t nextion_1_get_page()
{
    return page;
}
void nextion_1_goto_page(page_t page)
{
    nextion_command_page_by_pageIndex(&nextion_1, page);
}

/*
████████╗██╗███╗   ███╗███████╗██████╗
╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗
   ██║   ██║██╔████╔██║█████╗  ██████╔╝
   ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗
   ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║
   ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝

*/
TimerHandle_t timer_handle = NULL;
void nextion_1_timer(TimerHandle_t xTimer)
{
    SEND_EMPTY_MSG(main_queue, NEXTION_UPDATE, portMAX_DELAY);
}
void nextion_1_init_timer()
{
    assert(timer_handle == NULL);
    timer_handle = xTimerCreate("nextion_timer", pdMS_TO_TICKS(NEXTION_TIMER_PERIOD_MS), pdTRUE, NULL, nextion_1_timer);
}
void nextion_1_start_timer()
{
    assert(timer_handle != NULL);
    xTimerStart(timer_handle, portMAX_DELAY);
}

void nextion_1_stop_timer()
{
    assert(timer_handle != NULL);
    xTimerStop(timer_handle, portMAX_DELAY);
}
/*
██╗███╗   ██╗████████╗██████╗  ██████╗
██║████╗  ██║╚══██╔══╝██╔══██╗██╔═══██╗
██║██╔██╗ ██║   ██║   ██████╔╝██║   ██║
██║██║╚██╗██║   ██║   ██╔══██╗██║   ██║
██║██║ ╚████║   ██║   ██║  ██║╚██████╔╝
╚═╝╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝

*/

// WRITE LOADING
void nextion_1_intro_write_loading(int content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_INTRO, 2, content);
}
/*
██╗  ██╗ ██████╗ ███╗   ███╗███████╗
██║  ██║██╔═══██╗████╗ ████║██╔════╝
███████║██║   ██║██╔████╔██║█████╗
██╔══██║██║   ██║██║╚██╔╝██║██╔══╝
██║  ██║╚██████╔╝██║ ╚═╝ ██║███████╗
╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝

*/

// WRITE DATA
void nextion_1_home_write_data(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_HOME, IdSx[index].home_Data, content);
}

// WRITE DATAPS
void nextion_1_home_write_dataps(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_HOME, IdSx[index].home_Dataps, content);
}

// WRITE UNIT
void nextion_1_home_write_unit(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_HOME, IdSx[index].home_Unit, content);
}

// WRITE UNITPS
void nextion_1_home_write_unitps(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_HOME, IdSx[index].home_Unitps, content);
}

// WRITE NAME
void nextion_1_home_write_name(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_HOME, IdSx[index].home_Name, content);
}

// WRITE STATUS
void nextion_1_home_write_status(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_HOME, IdSx[index].home_Status, content);
}

/*
 ██████╗ █████╗ ██╗     ██╗██████╗ ██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝██╔══██╗██║     ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██║     ███████║██║     ██║██████╔╝██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║
██║     ██╔══██║██║     ██║██╔══██╗██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
╚██████╗██║  ██║███████╗██║██████╔╝██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
 ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

// WRITE VALUE
void nextion_1_calibration_write_value(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Value, content);
}

// WRITE UNIT
void nextion_1_calibration_write_unit(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Units, content);
}

// WRITE NAME
void nextion_1_calibration_write_name(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Name, content);
}

// WRITE RANGE
void nextion_1_calibration_write_range(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Range, content);
}

// WRITE UNITS PATH
void nextion_1_calibration_write_units_path(int index, char *content)
{
    nextion_set_path_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Units, content);
}

// WRITE UNITS VAL
void nextion_1_calibration_write_units_val(int index, int32_t content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Units, content);
}

// WRITE LIMIT
void nextion_1_calibration_write_limit(int index, int32_t content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Limit, content);
}

// WRITE SWITCH
void nextion_1_calibration_write_switch(int index, int32_t content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Switch, content);
}

void nextion_1_calibration_write_counts(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CALIBRATION, IdSx[index].calibration_Counts, content);
}

/*
██████╗  █████╗ ████████╗███████╗████████╗██╗███╗   ███╗███████╗
██╔══██╗██╔══██╗╚══██╔══╝██╔════╝╚══██╔══╝██║████╗ ████║██╔════╝
██║  ██║███████║   ██║   █████╗     ██║   ██║██╔████╔██║█████╗
██║  ██║██╔══██║   ██║   ██╔══╝     ██║   ██║██║╚██╔╝██║██╔══╝
██████╔╝██║  ██║   ██║   ███████╗   ██║   ██║██║ ╚═╝ ██║███████╗
╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝   ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝

*/

void nextion_1_datetime_get_rtc_year()
{
    nextion_command_get_by_sysVar(&nextion_1, "rtc0");
}
void nextion_1_datetime_get_rtc_month()
{
    nextion_command_get_by_sysVar(&nextion_1, "rtc1");
}
void nextion_1_datetime_get_rtc_day()
{
    nextion_command_get_by_sysVar(&nextion_1, "rtc2");
}
void nextion_1_datetime_get_rtc_hour()
{
    nextion_command_get_by_sysVar(&nextion_1, "rtc3");
}
void nextion_1_datetime_get_rtc_minute()
{
    nextion_command_get_by_sysVar(&nextion_1, "rtc4");
}
void nextion_1_datetime_get_rtc_second()
{
    nextion_command_get_by_sysVar(&nextion_1, "rtc5");
}
void nextion_1_datetime_get_rtc_dateformat()
{
    nextion_command_get_by_objId(&nextion_1, PAGE_DATETIME, 20, "val");
}
void nextion_1_datetime_get_rtc_timeformat()
{
    nextion_command_get_by_objId(&nextion_1, PAGE_DATETIME, 21, "val");
}

void nextion_1_set_datetime(char *content)
{
    nextion_set_txt_from_objName_local(&nextion_1, "t255", content);
}

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██████╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗╚════██╗
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝ █████╔╝
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝  ╚═══██╗
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║     ██████╔╝
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝     ╚═════╝

*/

void nextion_1_inputcalibp3_write_real0(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 28, content);
}
void nextion_1_inputcalibp3_write_real1(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 29, content);
}
void nextion_1_inputcalibp3_write_real2(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 30, content);
}
void nextion_1_inputcalibp3_write_real3(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 31, content);
}
void nextion_1_inputcalibp3_write_real4(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 32, content);
}
void nextion_1_inputcalibp3_write_real5(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 33, content);
}
void nextion_1_inputcalibp3_write_real6(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 34, content);
}
void nextion_1_inputcalibp3_write_real7(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 35, content);
}
void nextion_1_inputcalibp3_write_real8(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 36, content);
}
void nextion_1_inputcalibp3_write_real9(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 37, content);
}
void nextion_1_inputcalibp3_write_real10(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 38, content);
}

void nextion_1_inputcalibp3_write_raw0(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 39, content);
}
void nextion_1_inputcalibp3_write_raw1(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 40, content);
}
void nextion_1_inputcalibp3_write_raw2(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 41, content);
}
void nextion_1_inputcalibp3_write_raw3(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 42, content);
}
void nextion_1_inputcalibp3_write_raw4(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 43, content);
}
void nextion_1_inputcalibp3_write_raw5(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 44, content);
}
void nextion_1_inputcalibp3_write_raw6(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 45, content);
}
void nextion_1_inputcalibp3_write_raw7(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 46, content);
}
void nextion_1_inputcalibp3_write_raw8(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 47, content);
}
void nextion_1_inputcalibp3_write_raw9(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 48, content);
}
void nextion_1_inputcalibp3_write_raw10(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP3, 49, content);
}

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██╗  ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗██║  ██║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝███████║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝ ╚════██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║          ██║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝          ╚═╝

*/

void nextion_1_inputcalibp4_write_result(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCALIBP4, 13, content);
}
void nextion_1_inputcalibp4_write_result_color(uint32_t content)
{
    nextion_set_pco_from_objId(&nextion_1, PAGE_INPUTCALIBP4, 13, content);
}
/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ ██████╗ ███╗   ██╗███████╗██╗ ██████╗     ██████╗  ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔═══██╗████╗  ██║██╔════╝██║██╔════╝     ██╔══██╗███║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ██║   ██║██╔██╗ ██║█████╗  ██║██║  ███╗    ██████╔╝╚██║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██║   ██║██║╚██╗██║██╔══╝  ██║██║   ██║    ██╔═══╝  ██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗╚██████╔╝██║ ╚████║██║     ██║╚██████╔╝    ██║      ██║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝     ╚═╝ ╚═════╝     ╚═╝      ╚═╝

*/
void nextion_1_inputconfigp1_write_result(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_INPUTCONFIGP1, 16, content);
}
void nextion_1_inputconfigp1_write_result_color(uint32_t content)
{
    nextion_set_pco_from_objId(&nextion_1, PAGE_INPUTCONFIGP1, 16, content);
}

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ███████╗      ██████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██║      ╚════██╗
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝    ██╔╝       █████╔╝
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝    ██╔╝        ╚═══██╗
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║        ██║███████╗██████╔╝
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝        ╚═╝╚══════╝╚═════╝

*/

void nextion_1_new_test_p7_3_write_current_value(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST_P7_3, 28, content);
}

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ███████╗ ██╗  ██╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██║ ██║  ██║
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝    ██╔╝ ███████║
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝    ██╔╝  ╚════██║
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║        ██║███████╗██║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝        ╚═╝╚══════╝╚═╝

*/

void nextion_1_new_test_p7_4_write_current_value(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST_P7_4, 28, content);
}

/*
███╗   ███╗███████╗███████╗███████╗ █████╗  ██████╗ ███████╗
████╗ ████║██╔════╝██╔════╝██╔════╝██╔══██╗██╔════╝ ██╔════╝
██╔████╔██║█████╗  ███████╗███████╗███████║██║  ███╗█████╗
██║╚██╔╝██║██╔══╝  ╚════██║╚════██║██╔══██║██║   ██║██╔══╝
██║ ╚═╝ ██║███████╗███████║███████║██║  ██║╚██████╔╝███████╗
╚═╝     ╚═╝╚══════╝╚══════╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝

*/
void nextion_1_message_write_content(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_MESSAGE, 2, content);
}
void nextion_1_message_write_nextpage(page_t content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_MESSAGE, 6, content);
}
void nextion_1_message_write_content_color(uint32_t content)
{
    nextion_set_pco_from_objId(&nextion_1, PAGE_MESSAGE, 2, content);
}
/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ██████╗  ██╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗███║
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║       ██████╔╝╚██║
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝  ██║
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║       ██║      ██║
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝      ╚═╝

*/
// TITLE
void nextion_1_current_test_p1_write_title(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, 7, content);
}
void nextion_1_current_test_p1_append_title(char *content)
{
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, 7, content);
}

// SENSOR NAME
void nextion_1_current_test_p1_write_sensor_name(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, IdSx[index].currenttestp1_Name, content);
}

// WRITE DATA
void nextion_1_current_test_p1_write_data(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, IdSx[index].currenttestp1_Data, content);
}

// WRITE DATAPS
void nextion_1_current_test_p1_write_dataps(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, IdSx[index].currenttestp1_Dataps, content);
}

// UNIT
void nextion_1_current_test_p1_write_unit(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, IdSx[index].currenttestp1_Unit, content);
}

// UNITPS
void nextion_1_current_test_p1_write_unitps(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, IdSx[index].currenttestp1_Unitps, content);
}

// STATUS
void nextion_1_current_test_p1_write_status(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, 12, content);
}

// POINTs
void nextion_1_current_test_p1_write_points(int content)
{
    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%i", content);
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, 6, buffer);
}

// TIME
void nextion_1_current_test_p1_write_time(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P1, 13, content);
}

/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ██████╗ ██████╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██╗
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║       ██████╔╝ █████╔╝
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝ ██╔═══╝
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║       ██║     ███████╗
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚══════╝

*/
// TITLE
void nextion_1_current_test_p2_write_title(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 9, content);
}
void nextion_1_current_test_p2_append_title(char *content)
{
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 9, content);
}

// NAME
void nextion_1_current_test_p2_write_sensor_name(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Name, content);
}

// UNIT
void nextion_1_current_test_p2_write_unit(int index, char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Unit, content);
}

// TABLE INDEX
void nextion_1_current_test_p2_append_index(char *content)
{
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 8, content);
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 8, "\r\n");
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 8, "----");
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 8, "\r\n");
}
void nextion_1_current_test_p2_clean_index()
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 8, "");
}

// TABLE TIME
void nextion_1_current_test_p2_append_time(char *content)
{
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 18, content);
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 18, "\r\n");
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 18, "----");
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 18, "\r\n");
}
void nextion_1_current_test_p2_clean_time()
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 18, "");
}

// TABLE DATA
void nextion_1_current_test_p2_append_sensor_data(int index, char *content)
{
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Data, content);
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Data, "\r\n");
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Data, "----");
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Data, "\r\n");
}
void nextion_1_current_test_p2_clean_sensor_data(int index)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, IdSx[index].currenttestp2_Data, "");
}

// STATUS
void nextion_1_current_test_p2_write_status(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 35, content);
}

// POINTs
void nextion_1_current_test_p2_write_points(int content)
{
    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%i", content);
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 33, buffer);
}

// TIME
void nextion_1_current_test_p2_write_time(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P2, 36, content);
}
/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ██████╗ ██████╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██╗
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║       ██████╔╝ █████╔╝
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝  ╚═══██╗
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║       ██║     ██████╔╝
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚═════╝

*/

// TITLE
void nextion_1_current_test_p3_write_title(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 16, content);
}
void nextion_1_current_test_p3_append_title(char *content)
{
    nextion_append_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 16, content);
}

/*
void nextion_1_current_test_p3_write_waveform(uint8_t content)
{
    nextion_add_single_value_to_waveform(&nextion_1, 8, 0, content);
}
*/

// CLEAN DATA
void nextion_1_current_test_p3_write_slope(int pos, int content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 23 + pos, content);
}
void nextion_1_current_test_p3_clean_sensor_data()
{
    nextion_set_val_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 83, 150);
    nextion_set_en_from_objName_local(&nextion_1, "tm1", true);
}

// APPEND DATA
void nextion_1_current_test_p3_append_data(int content)
{
    nextion_set_val_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 83, content);
    nextion_set_en_from_objName_local(&nextion_1, "tm1", true);
}

// STATUS
void nextion_1_current_test_p3_write_status(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 21, content);
}

// POINTs
void nextion_1_current_test_p3_write_points(int content)
{
    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%i", content);
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 19, buffer);
}

// TIME
void nextion_1_current_test_p3_write_time(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 22, content);
}

// Y axis
void nextion_1_current_test_p3_write_sensor_name(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 86, content);
}
void nextion_1_current_test_p3_write_sensor_unit(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 17, content);
}
void nextion_1_current_test_p3_write_Y1(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 8, content);
}
void nextion_1_current_test_p3_write_Y2(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 9, content);
}
void nextion_1_current_test_p3_write_Y3(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 10, content);
}
void nextion_1_current_test_p3_write_Y4(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 11, content);
}
void nextion_1_current_test_p3_write_Y5(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 12, content);
}
void nextion_1_current_test_p3_write_Y6(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 13, content);
}
void nextion_1_current_test_p3_write_Y7(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 14, content);
}
void nextion_1_current_test_p3_write_Y8(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_CURRENT_TEST_P3, 15, content);
}