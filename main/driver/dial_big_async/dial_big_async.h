#pragma once

#include "driver/gpio.h"
#include "stdint.h"

#define DIALBIG_DEVS_MAX 4
#define DIALBIG_BITS_PER_FRAME 28
#define DIALBIG_BITS_PER_CHAR 4
#define DIALBIG_IDLE_TRESH_TICKS 10

typedef enum
{
    DIALBIG_OK,
    DIALBIG_ERROR,
    DIALBIG_WAITING,
    DIALBIG_NO_AVAILABLE_SLOT,
} dialbig_res_t;

typedef enum
{
    DIALBIG_MM,
    DIALBIG_INCH,
    DIAL_BIG_UNIT_MAX,
} dialbig_unit_t;

typedef enum{
    DIALBIG_2_DECIMALS,
    DIALBIG_4_DECIMALS,
    DIALBIG_DECIMALS_MAX,
}dialbig_decimals_t;

typedef struct
{
    dialbig_unit_t unit;
    int decimals;
    float val;
} dialbig_data_t;

typedef struct
{
    uint32_t tick_count;
    uint32_t bit;
    int slot;
} dialbig_msg_t;

typedef void (*dialbig_isr_t)(dialbig_data_t real);
typedef void (*dialbig_task_t)(void *arg);

typedef struct
{
    dialbig_isr_t data_done_cb;
    gpio_num_t data_pin;
    gpio_num_t clk_pin;
    gpio_num_t req_pin;
} dialbig_t;

dialbig_res_t dialbig_init(dialbig_t *dev);
dialbig_res_t dialbig_enable(dialbig_t *dev);
dialbig_res_t dialbig_disable(dialbig_t *dev);