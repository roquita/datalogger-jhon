
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
        .home_Use = 5,
        .calibration_Switch = 4,
        .calibration_Name = 2,
        .calibration_Range = 3,
        .calibration_Limit = 1,
        .calibration_Units = 77,
        .calibration_Value = 10,
        .calibration_Counts = 11,
        .calibration_Calibrate = 56,
    },
    {
        .home_Name = 10,
        .home_Data = 13,
        .home_Dataps = 15,
        .home_Unit = 14,
        .home_Unitps = 16,
        .home_Tara = 11,
        .home_Use = 12,
        .calibration_Switch = 16,
        .calibration_Name = 14,
        .calibration_Range = 15,
        .calibration_Limit = 13,
        .calibration_Units = 76,
        .calibration_Value = 17,
        .calibration_Counts = 18,
        .calibration_Calibrate = 57,
    },
    {
        .home_Name = 17,
        .home_Data = 20,
        .home_Dataps = 22,
        .home_Unit = 21,
        .home_Unitps = 23,
        .home_Tara = 18,
        .home_Use = 19,
        .calibration_Switch = 22,
        .calibration_Name = 20,
        .calibration_Range = 21,
        .calibration_Limit = 19,
        .calibration_Units = 75,
        .calibration_Value = 23,
        .calibration_Counts = 24,
        .calibration_Calibrate = 58,
    },
    {
        .home_Name = 24,
        .home_Data = 27,
        .home_Dataps = 29,
        .home_Unit = 28,
        .home_Unitps = 30,
        .home_Tara = 25,
        .home_Use = 26,
        .calibration_Switch = 28,
        .calibration_Name = 26,
        .calibration_Range = 27,
        .calibration_Limit = 25,
        .calibration_Units = 74,
        .calibration_Value = 29,
        .calibration_Counts = 30,
        .calibration_Calibrate = 59,
    },
    {
        .home_Name = 31,
        .home_Data = 34,
        .home_Dataps = 36,
        .home_Unit = 35,
        .home_Unitps = 37,
        .home_Tara = 32,
        .home_Use = 33,
        .calibration_Switch = 34,
        .calibration_Name = 32,
        .calibration_Range = 33,
        .calibration_Limit = 31,
        .calibration_Units = 73,
        .calibration_Value = 35,
        .calibration_Counts = 36,
        .calibration_Calibrate = 60,
    },
    {
        .home_Name = 38,
        .home_Data = 41,
        .home_Dataps = 43,
        .home_Unit = 42,
        .home_Unitps = 44,
        .home_Tara = 39,
        .home_Use = 40,
        .calibration_Switch = 40,
        .calibration_Name = 38,
        .calibration_Range = 39,
        .calibration_Limit = 37,
        .calibration_Units = 72,
        .calibration_Value = 41,
        .calibration_Counts = 42,
        .calibration_Calibrate = 61,
    },
    {
        .home_Name = 45,
        .home_Data = 48,
        .home_Dataps = 50,
        .home_Unit = 49,
        .home_Unitps = 51,
        .home_Tara = 46,
        .home_Use = 47,
        .calibration_Switch = 46,
        .calibration_Name = 44,
        .calibration_Range = 45,
        .calibration_Limit = 43,
        .calibration_Units = 71,
        .calibration_Value = 47,
        .calibration_Counts = 48,
        .calibration_Calibrate = 62,
    },
    {
        .home_Name = 52,
        .home_Data = 55,
        .home_Dataps = 57,
        .home_Unit = 56,
        .home_Unitps = 58,
        .home_Tara = 53,
        .home_Use = 54,
        .calibration_Switch = 52,
        .calibration_Name = 50,
        .calibration_Range = 51,
        .calibration_Limit = 49,
        .calibration_Units = 70,
        .calibration_Value = 53,
        .calibration_Counts = 54,
        .calibration_Calibrate = 63,
    }};

page_t page = PAGE_HOME;

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

        // HOME
        if (page == 0 && component_id == 0x00)
            SEND2FRONT_EMPTY_MSG(main_queue, HOME_LOADED, portMAX_DELAY)
        // HOME - TARA 1 BUTTON
        else if (page == 0 && component_id == 0x04 && event == 0x03)
            SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_1_INDEX, portMAX_DELAY)
        else if (page == 0 && component_id == 0x04 && event == 0x04)
            SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_1_INDEX, portMAX_DELAY)
        // HOME - TARA 2 BUTTON
        else if (page == 0 && component_id == 0x0B && event == 0x03)
            SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_2_INDEX, portMAX_DELAY)
        else if (page == 0 && component_id == 0x0B && event == 0x04)
            SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_2_INDEX, portMAX_DELAY)
        // HOME - TARA 3 BUTTON
        else if (page == 0 && component_id == 0x12 && event == 0x03)
            SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_3_INDEX, portMAX_DELAY)
        else if (page == 0 && component_id == 0x12 && event == 0x04)
            SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_3_INDEX, portMAX_DELAY)
        // HOME - TARA 4 BUTTON
        else if (page == 0 && component_id == 0x19 && event == 0x03)
            SEND_I32_MSG(main_queue, TARA_DISABLED, SENSOR_4_INDEX, portMAX_DELAY)
        else if (page == 0 && component_id == 0x19 && event == 0x04)
            SEND_I32_MSG(main_queue, TARA_ENABLED, SENSOR_4_INDEX, portMAX_DELAY)
        // SYSTEM SETTINGS LOADED
        else if (page == 1 && component_id == 0x00)
            SEND2FRONT_EMPTY_MSG(main_queue, SYSTEM_SETTINGS_LOADED, portMAX_DELAY)
        // CALIBRATION LOADED
        else if (page == 2 && component_id == 0x00)
            SEND2FRONT_EMPTY_MSG(main_queue, CALIBRATION_LOADED, portMAX_DELAY)
        // CALIBRATION - SWITCH 1
        else if (page == 2 && component_id == 0x04 && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_1_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x04 && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_1_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 2
        else if (page == 2 && component_id == 0x10 && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_2_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x10 && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_2_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 3
        else if (page == 2 && component_id == 0x16 && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_3_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x16 && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_3_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 4
        else if (page == 2 && component_id == 0x1C && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_4_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x1C && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_4_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 5
        else if (page == 2 && component_id == 0x22 && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_5_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x22 && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_5_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 6
        else if (page == 2 && component_id == 0x28 && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_6_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x28 && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_6_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 7
        else if (page == 2 && component_id == 0x2E && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_7_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x2E && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_7_INDEX, portMAX_DELAY)
        // CALIBRATION - SWITCH 8
        else if (page == 2 && component_id == 0x34 && event == 0x02)
            SEND_I32_MSG(main_queue, SENSOR_DISABLED, SENSOR_8_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x34 && event == 0x03)
            SEND_I32_MSG(main_queue, SENSOR_ENABLED, SENSOR_8_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 1
        else if (page == 2 && component_id == 0x01 && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_1_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x01 && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_1_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 2
        else if (page == 2 && component_id == 0x0D && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_2_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x0D && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_2_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 3
        else if (page == 2 && component_id == 0x13 && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_3_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x13 && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_3_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 4
        else if (page == 2 && component_id == 0x19 && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_4_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x19 && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_4_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 5
        else if (page == 2 && component_id == 0x1F && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_5_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x1F && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_5_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 6
        else if (page == 2 && component_id == 0x25 && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_6_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x25 && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_6_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 7
        else if (page == 2 && component_id == 0x2B && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_7_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x2B && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_7_INDEX, portMAX_DELAY)
        // CALIBRATION - LIMITS 8
        else if (page == 2 && component_id == 0x31 && event == 0x02)
            SEND_I32_MSG(main_queue, LIMITS_DISABLED, SENSOR_8_INDEX, portMAX_DELAY)
        else if (page == 2 && component_id == 0x31 && event == 0x03)
            SEND_I32_MSG(main_queue, LIMITS_ENABLED, SENSOR_8_INDEX, portMAX_DELAY)

        // INPUTCALIBP1 LOADED
        else if (page == 8 && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, INPUTCALIBP1_LOADED, portMAX_DELAY)
        // INPUTCALIBP2 LOADED
        else if (page == 9 && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, INPUTCALIBP2_LOADED, portMAX_DELAY)
        // INPUTCALIBP3 LOADED
        else if (page == 10 && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, INPUTCALIBP3_LOADED, portMAX_DELAY)
        // INPUTCALIBP4 LOADED
        else if (page == 11 && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, INPUTCALIBP4_LOADED, portMAX_DELAY)
        // INPUTCALIBP4 SAVE
        else if (page == 0x0B && component_id == 0x0A)
            SEND_EMPTY_MSG(main_queue, INPUTCALIBP4_SAVE_PRESSED, portMAX_DELAY)

        // INPUT CONFIGURATION P1 LOADED
        else if (page == 0x0F && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, INPUTCONFIGP1_LOADED, portMAX_DELAY)
        // INPUTCONFIGP1 SAVE
        else if (page == 0x0F && component_id == 0x0D)
            SEND_EMPTY_MSG(main_queue, INPUTCONFIGP1_SAVE_PRESSED, portMAX_DELAY)

        // NUMPAP POPUP LOADED
        else if (page == 0x03 && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, NUMPAD_POPUP_LOADED, portMAX_DELAY)

        // NEW TEST LOADED
        else if (page == 0x0D && component_id == 0x00)
            SEND_EMPTY_MSG(main_queue, NEW_TEST_LOADED, portMAX_DELAY)
        // NEW TEST START
        else if (page == 0x0D && component_id == 0x28)
            SEND_EMPTY_MSG(main_queue, NEW_TEST_START, portMAX_DELAY)
        // NEW TEST STOP
        else if (page == 0x0D && component_id == 0x29)
            SEND_EMPTY_MSG(main_queue, NEW_TEST_STOP, portMAX_DELAY)
        // NEW TEST DOWNLOAD
        else if (page == 0x0D && component_id == 0x2A)
            SEND_EMPTY_MSG(main_queue, NEW_TEST_DOWNLOAD, portMAX_DELAY)

        // UNKNOWN TOUCH EVENT
        else
        {
            ESP_LOGW(TAG, "file:%s,line:%u", __FILE__, __LINE__);
            ESP_LOGW(TAG, "page:0x%02x || component_id:0x%02x || event:0x%02x", page, component_id, event);
        }
    }
    else if (cmd_id == CUSTOM_EVENT_ID)
    {
        uint8_t page = cmd->custom_event.page;
        uint8_t component_id = cmd->custom_event.component_id;
        void *data = cmd->custom_event.data;
        int data_len = cmd->custom_event.data_len;

        // CALIBRATION SENSOR 1 UNIT CHANGED
        if (page == 2 && component_id == 0x4D)
            SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_1_INDEX, portMAX_DELAY)
        // CALIBRATION SENSOR 2 UNIT CHANGED
        else if (page == 2 && component_id == 0x4C)
            SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_2_INDEX, portMAX_DELAY)
        // CALIBRATION SENSOR 3  UNIT CHANGED
        else if (page == 2 && component_id == 0x4B)
            SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_3_INDEX, portMAX_DELAY)
        // CALIBRATION SENSOR 4  UNIT CHANGED
        else if (page == 2 && component_id == 0x4A)
            SEND_CONTENT_STR_MSG(main_queue, SENSOR_UNIT_CHANGED, data, data_len, SENSOR_4_INDEX, portMAX_DELAY)
        // INPUTCALIBP1  TYPE RECEIVED
        else if (page == 8 && component_id == 0x10)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP1_TYPE_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP1  CAPACITY RECEIVED
        else if (page == 8 && component_id == 0x09)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP1_CAPACITY_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP1  CAPACITY UNIT RECEIVED
        else if (page == 8 && component_id == 0x0F)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP1_CAPACITY_UNIT_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP1  SENSIBILITY RECEIVED
        else if (page == 8 && component_id == 0x0A)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP1_SENSIBILITY_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP1  SENSIBILITY UNIT RECEIVED
        else if (page == 8 && component_id == 0x0B)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP1_SENSIBILITY_UNIT_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP1 SENSOR INDEX RECEIVED
        else if (page == 8 && component_id == 0x13)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP1_SENSOR_INDEX_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP2 CALIBRATION LIMIT RECEIVED
        else if (page == 9 && component_id == 0x08)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP2_CALIBRATION_LIMIT_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP2  LIMIT ENABLE RECEIVED
        else if (page == 9 && component_id == 0x0D)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP2_LIMIT_ENABLE_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP3 TABLE RECEIVED
        else if (page == 0x0A && component_id == 0x1C)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP3_TABLE_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP3 NUM POINTS RECEIVED
        else if (page == 0x0A && component_id == 0x32)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP3_NUM_POINTS_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP3 ROW TO FILL RECEIVED
        else if (page == 0x0A && component_id == 0x0C)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP3_ROW_TO_FILL_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCALIBP4 NAME RECEIVED
        else if (page == 0x0B && component_id == 0x08)
            SEND_ADDR_MSG(main_queue, INPUTCALIBP4_NAME_RECEIVED, data, data_len, portMAX_DELAY)

        // INPUTCONFIGP1 TYPE RECEIVED
        else if (page == 0x0F && component_id == 0x0F)
            SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_TYPE_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCONFIGP1 CAPACITY RECEIVED
        else if (page == 0x0F && component_id == 0x04)
            SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_CAPACITY_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCONFIGP1 CAPACITY UNIT RECEIVED
        else if (page == 0x0F && component_id == 0x0E)
            SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_CAPACITY_UNIT_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCONFIGP1 NAME RECEIVED
        else if (page == 0x0F && component_id == 0x09)
            SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_NAME_RECEIVED, data, data_len, portMAX_DELAY)
        // INPUTCONFIGP1  INDEX RECEIVED
        else if (page == 0x0F && component_id == 0x11)
            SEND_ADDR_MSG(main_queue, INPUTCONFIGP1_INDEX_RECEIVED, data, data_len, portMAX_DELAY)

        // NEW TEST SET INDEX 1
        else if (page == 0x0D && component_id == 0x2E)
            SEND_ADDR_MSG(main_queue, NEW_TEST_SET_INDEX1, data, data_len, portMAX_DELAY)
        // NEW TEST SET INDEX 2
        else if (page == 0x0D && component_id == 0x2D)
            SEND_ADDR_MSG(main_queue, NEW_TEST_SET_INDEX2, data, data_len, portMAX_DELAY)

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

void nextion_1_timer(TimerHandle_t xTimer)
{
    SEND_EMPTY_MSG(main_queue, NEXTION_UPDATE, portMAX_DELAY);
}

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

TimerHandle_t timer_handle = NULL;

void nextion_1_init()
{
    nextion_init(&nextion_1);
    timer_handle = xTimerCreate("nextion_timer", pdMS_TO_TICKS(200), pdTRUE, NULL, nextion_1_timer);
}

void nextion_1_start_timer()
{
    xTimerStart(timer_handle, portMAX_DELAY);
}

void nextion_1_stop_timer()
{
    xTimerStop(timer_handle, portMAX_DELAY);
}

void nextion_1_set_page(page_t new_page)
{
    page = new_page;
}

page_t nextion_1_get_page()
{
    return page;
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
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝

*/

void nextion_1_newtest_write_table01(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 30, content);
}
void nextion_1_newtest_write_table11(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 31, content);
}
void nextion_1_newtest_write_table21(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 32, content);
}
void nextion_1_newtest_write_table31(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 33, content);
}
void nextion_1_newtest_write_table41(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 34, content);
}
void nextion_1_newtest_write_table51(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 35, content);
}
void nextion_1_newtest_write_table61(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 36, content);
}
void nextion_1_newtest_write_table71(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 37, content);
}
void nextion_1_newtest_write_table81(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 38, content);
}
void nextion_1_newtest_write_table91(char *content)
{
    nextion_set_txt_from_objId(&nextion_1, PAGE_NEW_TEST, 39, content);
}
