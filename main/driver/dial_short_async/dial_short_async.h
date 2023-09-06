#pragma once

#include "driver/gpio.h"
#include "stdint.h"

#define DIALSHORT_DEVS_MAX 4
#define DIALSHORT_BITS_PER_FRAME 52
#define DIALSHORT_BITS_PER_CHAR 4
#define DIALSHORT_IDLE_TRESH_TICKS 10

typedef enum
{
    DIALSHORT_OK,
    DIALSHORT_ERROR,
    DIALSHORT_WAITING,
    DIALSHORT_NO_AVAILABLE_SLOT,
} dialshort_res_t;

typedef enum
{
    DIALSHORT_MM,
    DIALSHORT_INCH,
} dialshort_unit_t;

typedef struct
{
    dialshort_unit_t unit;
    float val;
} dialshort_data_t;

typedef struct
{
    uint32_t tick_count;
    uint32_t bit;
    int slot;
} dialshort_msg_t;

typedef void (*dialshort_isr_t)(dialshort_data_t real);
typedef void (*dialshort_task_t)(void *arg);

typedef struct
{
    dialshort_isr_t data_done_cb;
    gpio_num_t data_pin;
    gpio_num_t clk_pin;
    gpio_num_t req_pin;
} dialshort_t;

typedef struct
{
    dialshort_t *dev;
    int slot;
} dialshort_context_t;

dialshort_res_t dialshort_init(dialshort_t *dev);
dialshort_res_t dialshort_enable(dialshort_t *dev);
dialshort_res_t dialshort_disable(dialshort_t *dev);