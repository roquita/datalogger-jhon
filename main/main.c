
#include "esp_log.h"
#include "project_defines.h"
#include "project_defines.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "project_tasks.h"
#include "project_typedefs.h"

#define TAG "MAIN"

QueueHandle_t main_queue = NULL;
QueueHandle_t sensor_queue = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "START");
    sensor_queue = xQueueCreate(10, sizeof(msg_t));
    main_queue = xQueueCreate(20, sizeof(msg_t));
    xTaskCreate(sensor_task, "sensor_task", 4 * 1024, NULL, 6, NULL);
    xTaskCreate(main_task, "main_task", 4 * 1024, NULL, 5, NULL);
}
