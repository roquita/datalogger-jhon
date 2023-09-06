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

#define SAMPLE_RATE_MAX_HZ (50)
#define POINTS_PER_TEST_MAX (3000)
#define MULTITEST_STORAGE_MAX (1000)

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
