#include "project_defines.h"
#include "dial_short_async.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "DIAL_SHORT_ASYNC"

// void dialshort_task_i0(void *arg);
//  void dialshort_task_i1(void *arg);
//  void dialshort_task_i2(void *arg);
//  void dialshort_task_i3(void *arg);
// static dialshort_task_t tasks[DIALSHORT_DEVS_MAX] = {dialshort_task_i0};
//  static QueueHandle_t queue_handle[DIALSHORT_DEVS_MAX] = {0};
static QueueHandle_t queue_handle = NULL;
static uint32_t tick_count[DIALSHORT_DEVS_MAX] = {0};
static int bit_count[DIALSHORT_DEVS_MAX] = {0};
static uint64_t frame[DIALSHORT_DEVS_MAX] = {0};
static dialshort_t *devs[DIALSHORT_DEVS_MAX] = {0};
static int slot = 0;

static bool isr_service_installed = false;
static bool task_initialized = false;

void dialshort_global_isr(void *arg)
{
    int slot = (int)arg;
    dialshort_t *dev = devs[slot];

    dialshort_msg_t msg;
    msg.bit = (uint32_t)gpio_get_level(dev->data_pin);
    msg.tick_count = xTaskGetTickCountFromISR();
    msg.slot = slot;

    xQueueSendFromISR(queue_handle, &msg, NULL);
}

static bool dialshort_frame_to_real(dialshort_data_t *real, uint64_t frame)
{
    uint64_t reverse_frame = 0;
    for (int i = 0; i < DIALSHORT_BITS_PER_FRAME; i++)
    {
        reverse_frame = reverse_frame << 1;
        reverse_frame |= (frame & 0b1);
        frame = frame >> 1;
    }

    // GROUP BITS
    uint32_t group_one = (reverse_frame >> 48) & 0b1111;
    uint32_t group_three = (reverse_frame >> 40) & 0b1111;
    uint32_t group_four = (reverse_frame >> 36) & 0b1111;
    uint32_t group_five = (reverse_frame >> 32) & 0b1111;
    uint32_t group_six = (reverse_frame >> 28) & 0b1111;
    uint32_t group_seven = (reverse_frame >> 24) & 0b1111;
    uint32_t group_eight = (reverse_frame >> 20) & 0b1111;
    uint32_t group_nine = (reverse_frame >> 16) & 0b1111;

    // DECODE DATA
    float sign = 0.0;
    if (group_one == 0 && group_nine == 8)
    {
        sign = -1.0;
        real->unit = DIALSHORT_MM;
    }
    else if (group_one == 0 && group_nine == 0)
    {
        sign = 1.0;
        real->unit = DIALSHORT_MM;
    }
    else if (group_one == 1 && group_nine == 0)
    {
        sign = 1.0;
        real->unit = DIALSHORT_INCH;
    }
    else if (group_one == 1 && group_nine == 8)
    {
        sign = -1.0;
        real->unit = DIALSHORT_INCH;
    }
    else
    {
        ESP_LOGE(TAG, "file:%s,line:%i", __FILE__, __LINE__);
        return false;
    }

    float raw_val = group_eight * 100000.0 + group_seven * 10000.0 + group_six * 1000.0 + group_five * 100.0 + group_four * 10.0 + group_three * 1.0;
    float unsigned_val = (real->unit == DIALSHORT_MM) ? raw_val / 1000.0 : raw_val / 100000.0;
    real->val = unsigned_val * sign;

    return true;
}

void dialshort_task(void *arg)
{
    queue_handle = xQueueCreate(DIALSHORT_BITS_PER_FRAME * DIALSHORT_DEVS_MAX * 10, sizeof(dialshort_msg_t));
    dialshort_msg_t msg;
    while (1)
    {
        xQueueReceive(queue_handle, &msg, portMAX_DELAY);

        int slot = msg.slot;
        dialshort_t *dev = devs[slot];
        uint32_t old_tick_count = tick_count[slot];
        uint32_t new_tick_count = msg.tick_count;
        uint32_t new_bit = msg.bit;

        // NEW FRAME DETECTED
        if (new_tick_count - old_tick_count >= DIALSHORT_IDLE_TRESH_TICKS)
        {
            bit_count[slot] = 0;
            frame[slot] = 0;
        }

        // ACUMULATE BITS
        bit_count[slot] = bit_count[slot] + 1;
        frame[slot] = (frame[slot] << 1) + new_bit;

        // FRAME COMPLETED
        if (bit_count[slot] == DIALSHORT_BITS_PER_FRAME)
        {
            dialshort_data_t real;
            bool valid_frame = dialshort_frame_to_real(&real, frame[slot]);

            if (valid_frame)
            {
                dev->data_done_cb(real);
            }
            else
            {
                ESP_LOGE(TAG, "file:%s,line:%i", __FILE__, __LINE__);
            }
        }
    }
}

dialshort_res_t dialshort_init(dialshort_t *dev)
{
    // CHECK AVAILABLE SLOT
    if (slot >= DIALSHORT_DEVS_MAX)
    {
        ESP_LOGE(TAG, "file:%s,line:%i", __FILE__, __LINE__);
        return DIALSHORT_NO_AVAILABLE_SLOT;
    }

    // CONFIG RATE PIN
    gpio_config_t req_conf = {};
    req_conf.intr_type = GPIO_INTR_DISABLE;
    req_conf.mode = GPIO_MODE_OUTPUT;
    req_conf.pin_bit_mask = (1ULL << (dev->req_pin));
    req_conf.pull_down_en = 0;
    req_conf.pull_up_en = 0;
    gpio_config(&req_conf);
    gpio_set_level(dev->req_pin, 1);

    // CONFIG CLk PIN
    gpio_config_t clk_conf = {};
    clk_conf.intr_type = GPIO_INTR_DISABLE;
    clk_conf.mode = GPIO_MODE_INPUT;
    clk_conf.pin_bit_mask = (1ULL << (dev->clk_pin));
    clk_conf.pull_down_en = 0;
    clk_conf.pull_up_en = 0;
    gpio_config(&clk_conf);

    // CONFIG DATA PIN
    gpio_config_t data_conf = {};
    data_conf.intr_type = GPIO_INTR_DISABLE;
    data_conf.mode = GPIO_MODE_INPUT;
    data_conf.pin_bit_mask = (1ULL << (dev->data_pin));
    data_conf.pull_down_en = 0;
    data_conf.pull_up_en = 0;
    gpio_config(&data_conf);

    // install gpio isr service
    if (!isr_service_installed)
    {
        isr_service_installed = true;
        gpio_install_isr_service(0);
    }

    // add handler for clk pin
    gpio_isr_handler_add(dev->clk_pin, dialshort_global_isr, (void *)slot);
    gpio_intr_disable(dev->clk_pin);

    // register slot
    devs[slot] = dev;
    slot++;

    // create background task
    if (!task_initialized)
    {
        task_initialized = true;
        xTaskCreate(dialshort_task, "dialshort_task", 2 * 1024, NULL, 5, NULL);
    }

    return DIALSHORT_OK;
}

dialshort_res_t dialshort_enable(dialshort_t *dev)
{
    // request on readings
    gpio_set_level(dev->req_pin, 0);

    // enable intr hw
    gpio_set_intr_type(dev->clk_pin, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(dev->clk_pin);

    return DIALSHORT_OK;
}
dialshort_res_t dialshort_disable(dialshort_t *dev)
{
    // disable intr hw
    gpio_intr_disable(dev->clk_pin);
    gpio_set_intr_type(dev->clk_pin, GPIO_INTR_DISABLE);

    // request off readigs
    gpio_set_level(dev->req_pin, 1);

    return DIALSHORT_OK;
}