#pragma once

#include "driver/gpio.h"
#include "stdint.h"

#define HX711_DEVS_MAX (4)
#define HX711_PULSES_PER_FRAME (24)

typedef enum
{
    HX711_OK,
    HX711_ERROR,
    HX711_WAITING,
    HX711_NO_AVAILABLE_SLOT,
} hx711_res_t;

typedef enum
{
    HX711_CH_A_GAIN_128,
    HX711_CH_A_GAIN_64,
    HX711_CH_B_GAIN_32,
} hx711_mux_pga_t;

typedef enum
{
    HX711_10HZ,
    HX711_80HZ,
    HX711_RATE_MAX,
} hx711_rate_hz_t;

typedef void (*hx711_isr_t)(uint32_t raw);

typedef struct
{
    hx711_mux_pga_t mux_pga;
    hx711_rate_hz_t rate_hz;
    hx711_isr_t data_done_cb;
    gpio_num_t rate_pin;
    gpio_num_t dout_pin;
    gpio_num_t sck_pin;
} hx711_t;

hx711_res_t hx711_init(hx711_t *dev);
hx711_res_t hx711_enable(hx711_t *dev);
hx711_res_t hx711_disable(hx711_t *dev);
