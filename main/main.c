
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
TaskHandle_t main_task_h = NULL;
TaskHandle_t sensor_task_h = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "START");
    sensor_queue = xQueueCreate(10, sizeof(msg_t));
    main_queue = xQueueCreate(20, sizeof(msg_t));
    xTaskCreate(sensor_task, "sensor_task", 4 * 1024, NULL, 6, &sensor_task_h);
    xTaskCreate(main_task, "main_task", 4 * 1024, NULL, 5, &main_task_h);

    while (1)
    {
        // MONITOREAR HEAP
        vTaskDelay(pdMS_TO_TICKS(10000));
        uint32_t free_heap = xPortGetFreeHeapSize();
        int largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        int idleSHWM = uxTaskGetStackHighWaterMark(NULL);
        int mainSHWM = uxTaskGetStackHighWaterMark(main_task_h);
        int sensorSHWM = uxTaskGetStackHighWaterMark(sensor_task_h);

        ESP_LOGE(TAG, "lrgsFreeBlck=%i || freeHeap=%lu || idleSHWM=%i ||  mainSHWM=%i || sensorSHWM=%i", largestFreeBlock, free_heap, idleSHWM, mainSHWM, sensorSHWM);

       // char buffer[2048];
       // vTaskGetRunTimeStats(buffer);
       // printf("STATS:\n\r%s\n\r", buffer);
    }
}
