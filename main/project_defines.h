#pragma once

#define SENSOR_1_INDEX (0)
#define SENSOR_2_INDEX (1)
#define SENSOR_3_INDEX (2)
#define SENSOR_4_INDEX (3)
#define SENSOR_5_INDEX (4)
#define SENSOR_6_INDEX (5)
#define SENSOR_7_INDEX (6)
#define SENSOR_8_INDEX (7)
#define SENSOR_MAX_INDEX (SENSOR_8_INDEX + 1)

#define NUM_SENSORS (8)
#define TABLE_NUM_ROWS (11)
#define TABLE_NUM_COLS (2)

// RX CMD
#define uTX_nRX_PIN (16)
#define uRX_nTX_PIN (17)
#define NEXTION_UART_NUM (UART_NUM_1)
#define NEXTION_BAUDRATE (921600)

#define BUF_SIZE (1024)

#define INITIAL_PAGE (HOME)

#define LOADCELL_1_DOUT_PIN (12)
#define LOADCELL_1_SCK_PIN (13)
#define LOADCELL_2_DOUT_PIN (14)
#define LOADCELL_2_SCK_PIN (15)
#define LOADCELL_3_DOUT_PIN (18)
#define LOADCELL_3_SCK_PIN (21)
#define LOADCELL_4_DOUT_PIN (33)
#define LOADCELL_4_SCK_PIN (34)

#define DIALBIG_1_DATA_PIN (6)
#define DIALBIG_1_CLK_PIN (7)
#define DIALBIG_1_REQ_PIN (8)
#define DIALBIG_2_DATA_PIN (9)
#define DIALBIG_2_CLK_PIN (10)
#define DIALBIG_2_REQ_PIN (11)
#define DIALBIG_3_DATA_PIN (35)
#define DIALBIG_3_CLK_PIN (36)
#define DIALBIG_3_REQ_PIN (37)
#define DIALBIG_4_DATA_PIN (38)
#define DIALBIG_4_CLK_PIN (39)
#define DIALBIG_4_REQ_PIN (40)

#define CALIB_NAME_LEN_MAX (30) // without null end

#define NVS_NAMESPACE "storage"
#define NVS_KEY_CALIB_BASE_STR "nvscalib"
#define NVS_KEY_CONFIG_BASE_STR "nvsconfig"

#define LOGERROR() ESP_LOGE(TAG, "file:%s,line:%i", __FILE__, __LINE__)
#define RETURN_IF_TRUE(a) \
    {                     \
        if (a)            \
        {                 \
            LOGERROR();   \
            return;       \
        }                 \
    }

#define NEXTION_COLOR_RED (45317)
#define NEXTION_COLOR_GREEN (2016)

#define NEXTION_DEFAULT_SENSOR_NAME "UNCALIBRATED"
#define NEXTION_DEFAULT_SENSOR_UNIT "UNIT"
#define NEXTION_DEFAULT_SENSOR_UNITPS "UNITPS"
#define NEXTION_DEFAULT_RANGE "0.0 - 0.0 N.A."
#define NEXTION_DEFAULT_PATH ""
#define NEXTION_DEFAULT_UNITS_VAL (0)
#define NEXTION_DEFAULT_LIMIT_ENABLED (false)
#define NEXTION_DEFAULT_SENSOR_ENABLE (false)

#define CURRENT_TEST_NUM_MAX (4)
#define TEST_ROWS_NUM_MAX (30)

#define TEST_NAME_LEN_MAX (30)
#define TEST_DATETIME_FORMATED_LEN_MAX (50)
#define TEST_TITLE_LEN_MAX (TEST_NAME_LEN_MAX + TEST_DATETIME_FORMATED_LEN_MAX + 3)

#define SENSOR_UNIT_LEN_MAX (10)
#define SENSOR_UNITPS_LEN_MAX (15)
#define SENSOR_READING_LEN_MAX (10)

#define CURRENT_TEST_SENSOR_NAME_DEFAULT "UNUSED"
#define CURRENT_TEST_TITLE_DEFAULT "NO TEST RUNNING"
#define CURRENT_TEST_DATA_DEFAULT "- - -"
#define CURRENT_TEST_DATAPS_DEFAULT "- - -"
#define CURRENT_TEST_UNIT_DEFAULT "unit"
#define CURRENT_TEST_UNITPS_DEFAULT "unitps"
#define CURRENT_TEST_STATUS_DEFAULT "-"
#define CURRENT_TEST_POINTS_DEFAULT "-"
#define CURRENT_TEST_TIME_DEFAULT "-"

#define NEXTION_PRE_INIT_PAGE (PAGE_INTRO)
#define NEXTION_POST_INIT_PAGE (PAGE_HOME)
#define NEXTION_TIMER_PERIOD_MS (200)

#define TIME_TIMER_PERIOD_MINUTES (10)
#define TIME_TIMER_PERIOD_MS (TIME_TIMER_PERIOD_MINUTES * 60 * 1000)

#define TEST_ROWS_NUM_MAX (30)
#define TEST_FS_SLOT_SIZE_MIN (10 * 1024) // 10kB
#define FATFS_BASE_PATH "/mem"
#define FATFS_DISK_INDEX "0:"
#define FATFS_FILETYPE_TEST "tst"
#define FATFS_FILETYPE_TEMPLATE "tmp"
#define FATFS_FILETYPE_SENSOR "s"
// #define TEST_TIMER_PERIOD_MS (100)
