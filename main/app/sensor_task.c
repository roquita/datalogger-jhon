#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "sensor_callback.h"
#include "sensor.h"
#include "project_tasks.h"
#include "project_defines.h"
#include "sensor.h"
#include "driver/gpio.h"

#define TAG "SENSOR_TASK"

static msg_t msg;

void sensor_task(void *arg)
{
    ESP_LOGI(TAG, TAG);

    sensor_1_init();
    sensor_2_init();
    sensor_3_init();
    sensor_4_init();
    sensor_5_init();
    sensor_6_init();
    sensor_7_init();
    sensor_8_init();

    while (1)
    {
        xQueueReceive(sensor_queue, &msg, portMAX_DELAY);
        msg_type_t type = msg.type;

        // HOME
        if (type == SENSOR_DISABLED)
            stask_sensor_disable_cb(&msg);
        else if (type == SENSOR_ENABLED)
            stask_sensor_enable_cb(&msg);
        else
        {
            ESP_LOGE(TAG, "file:%s,line:%u", __FILE__, __LINE__);
        }
    }
}
