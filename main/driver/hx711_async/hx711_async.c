#include "hx711_async.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_rom_sys.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"

#define TAG "HX711"

static hx711_t *devs[HX711_DEVS_MAX] = {0};
static int slot = 0;

static bool isr_service_installed = false;

#define NOP() __asm__ __volatile__("nop")
#define nNOP(x)                     \
    {                               \
        for (int i = 0; i < x; i++) \
        {                           \
            NOP();                  \
        }                           \
    }

// todo: add critical section
#define PULSE(sck_pin)              \
    {                               \
                                    \
        taskENTER_CRITICAL(&spin);  \
        gpio_set_level(sck_pin, 1); \
        nNOP(10);                   \
        taskEXIT_CRITICAL(&spin);   \
        gpio_set_level(sck_pin, 0); \
        nNOP(10);                   \
    }

void hx711_pendfunc(void *arg1, uint32_t arg2)
{
    portMUX_TYPE spin = portMUX_INITIALIZER_UNLOCKED;
    int slot = (int)arg1;
    hx711_t *dev = devs[slot];

    // GET BITS
    uint32_t count = 0;
    for (int i = 0; i < HX711_PULSES_PER_FRAME; i++)
    {
        PULSE(dev->sck_pin);

        count = count << 1;
        if (gpio_get_level(dev->dout_pin))
            count++;
    }

    // NEXT REQUEST
    if (dev->mux_pga == HX711_CH_A_GAIN_128)
    {
        PULSE(dev->sck_pin);
    }
    else if (dev->mux_pga == HX711_CH_A_GAIN_64)
    {
        PULSE(dev->sck_pin);
        PULSE(dev->sck_pin);
        PULSE(dev->sck_pin);
    }
    else
    {
        PULSE(dev->sck_pin);
        PULSE(dev->sck_pin);
    }

    // DECODE
    count = count ^ 0x800000;

    // USER CALLBACK
    dev->data_done_cb(count);

    // RE-ENABLE INTERRUPT
    gpio_intr_enable(dev->dout_pin);

    // portYIELD();
}
void IRAM_ATTR hx711_global_isr(void *arg)
{

    // DISABLE INTERRUPT TO PREVENT INNECESARY CALLS WHILE PROCCESING CURRENT CALL
    int slot = (int)arg;
    gpio_intr_disable(devs[slot]->dout_pin);

    // APPEND FUNCTION ON FREERTOS TIMER TASK TO PROCCESS CURRENT CALL
    BaseType_t xHigherPriorityTaskWoken = false;
    xTimerPendFunctionCallFromISR(hx711_pendfunc, arg, 0, &xHigherPriorityTaskWoken);

    // FORCE CONTEXT SWITCH TO TIMER TASK(PRIO 25 on MENUCONFIG)
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

hx711_res_t hx711_init(hx711_t *dev)
{
    // CHECK AVAILABLE SLOT
    if (slot >= HX711_DEVS_MAX)
    {
        ESP_LOGE(TAG, "file:%s,line:%i", __FILE__, __LINE__);
        return HX711_NO_AVAILABLE_SLOT;
    }

    // CONFIG RATE PIN
    if (dev->rate_pin != -1)
    {
        gpio_config_t rate_conf = {};
        rate_conf.intr_type = GPIO_INTR_DISABLE;
        rate_conf.mode = GPIO_MODE_OUTPUT;
        rate_conf.pin_bit_mask = (1ULL << (dev->rate_pin));
        rate_conf.pull_down_en = 0;
        rate_conf.pull_up_en = 0;
        gpio_config(&rate_conf);

        if (dev->rate_hz == HX711_10HZ)
        {
            gpio_set_level(dev->rate_pin, 0);
        }
        else
        { // HX711_80HZ
            gpio_set_level(dev->rate_pin, 1);
        }
    }

    // CONFIG SCK PIN
    gpio_config_t sck_conf = {};
    sck_conf.intr_type = GPIO_INTR_DISABLE;
    sck_conf.mode = GPIO_MODE_OUTPUT;
    sck_conf.pin_bit_mask = (1ULL << (dev->sck_pin));
    sck_conf.pull_down_en = 0;
    sck_conf.pull_up_en = 0;
    gpio_config(&sck_conf);

    // CONFIG DOUT PIN
    gpio_config_t dout_conf = {};
    dout_conf.intr_type = GPIO_INTR_DISABLE;
    dout_conf.mode = GPIO_MODE_INPUT;
    dout_conf.pin_bit_mask = (1ULL << (dev->dout_pin));
    dout_conf.pull_down_en = 0;
    dout_conf.pull_up_en = 0;
    gpio_config(&dout_conf);

    // install gpio isr service
    if (!isr_service_installed)
    {
        isr_service_installed = true;
        gpio_install_isr_service(0);
    }

    // add handler for dout pin
    gpio_isr_handler_add(dev->dout_pin, hx711_global_isr, (void *)slot);
    gpio_intr_disable(dev->dout_pin);

    // register slot
    devs[slot] = dev;
    slot++;

    return HX711_OK;
}
hx711_res_t hx711_enable(hx711_t *dev)
{
    // power on
    gpio_set_level(dev->sck_pin, 0);

    // intr type
    gpio_set_intr_type(dev->dout_pin, GPIO_INTR_LOW_LEVEL);

    // enable intr hw
    gpio_intr_enable(dev->dout_pin);

    return HX711_OK;
}
hx711_res_t hx711_disable(hx711_t *dev)
{
    // disable intr hw
    gpio_intr_disable(dev->dout_pin);

    // intr type
    gpio_set_intr_type(dev->dout_pin, GPIO_INTR_DISABLE);

    // power off
    gpio_set_level(dev->sck_pin, 1);

    return HX711_OK;
}
