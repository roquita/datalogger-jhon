
#include "sensor.h"
#include "hx711_async.h"
#include "project_defines.h"
#include "project_typedefs.h"
#include "project_tasks.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// SENSOR 1
void load_cell_data_done_1_cb(uint32_t raw)
{
    msg_t msg;
    msg.content.cr.reading.raw.tickcount = xTaskGetTickCount();
    msg.content.cr.reading.raw.value = raw;
    msg.content.cr.index = 0;
    msg.type = SENSOR_READING_RECEIVED;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
hx711_t load_cell_1 = {
    .mux_pga = HX711_CH_A_GAIN_64,
    .dout_pin = LOADCELL_1_DOUT_PIN,
    .sck_pin = LOADCELL_1_SCK_PIN,
    .data_done_cb = load_cell_data_done_1_cb,
    .rate_pin = -1,
};

// SENSOR 2
void load_cell_data_done_2_cb(uint32_t raw)
{
    msg_t msg;
    msg.content.cr.reading.raw.tickcount = xTaskGetTickCount();
    msg.content.cr.reading.raw.value = raw;
    msg.content.cr.index = 1;
    msg.type = SENSOR_READING_RECEIVED;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
hx711_t load_cell_2 = {
    .mux_pga = HX711_CH_A_GAIN_64,
    .dout_pin = LOADCELL_2_DOUT_PIN,
    .sck_pin = LOADCELL_2_SCK_PIN,
    .data_done_cb = load_cell_data_done_2_cb,
    .rate_pin = -1,
};

// SENSOR 3
void load_cell_data_done_3_cb(uint32_t raw)
{
    msg_t msg;
    msg.content.cr.reading.raw.tickcount = xTaskGetTickCount();
    msg.content.cr.reading.raw.value = raw;
    msg.content.cr.index = 2;
    msg.type = SENSOR_READING_RECEIVED;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
hx711_t load_cell_3 = {
    .mux_pga = HX711_CH_A_GAIN_64,
    .dout_pin = LOADCELL_3_DOUT_PIN,
    .sck_pin = LOADCELL_3_SCK_PIN,
    .data_done_cb = load_cell_data_done_3_cb,
    .rate_pin = -1,
};

// SENSOR 4
void load_cell_data_done_4_cb(uint32_t raw)
{
    msg_t msg;
    msg.content.cr.reading.raw.tickcount = xTaskGetTickCount();
    msg.content.cr.reading.raw.value = raw;
    msg.content.cr.index = 3;
    msg.type = SENSOR_READING_RECEIVED;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
hx711_t load_cell_4 = {
    .mux_pga = HX711_CH_A_GAIN_64,
    .dout_pin = LOADCELL_4_DOUT_PIN,
    .sck_pin = LOADCELL_4_SCK_PIN,
    .data_done_cb = load_cell_data_done_4_cb,
    .rate_pin = -1,
};

// SENSOR 5
void dial_big_data_done_1_cb(dialbig_data_t real)
{
    msg_t msg;
    msg.type = SENSOR_READING_RECEIVED;
    msg.content.cr.reading.real.value = real.val;
    msg.content.cr.reading.real.unit = ((real.unit == DIALBIG_MM) ? DISPLACEMENT_UNIT_MM : DISPLACEMENT_UNIT_IN);
    msg.content.cr.reading.real.num_decimals = real.decimals;
    msg.content.cr.reading.real.num_ints = 3;
    msg.content.cr.reading.real.tickcount = xTaskGetTickCount();
    msg.content.cr.index = 4;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
dialbig_t dial_big_1 = {
    .data_done_cb = dial_big_data_done_1_cb,
    .data_pin = DIALBIG_1_DATA_PIN,
    .clk_pin = DIALBIG_1_CLK_PIN,
    .req_pin = DIALBIG_1_REQ_PIN,
};

// SENSOR 6
void dial_big_data_done_2_cb(dialbig_data_t real)
{
    msg_t msg;
    msg.type = SENSOR_READING_RECEIVED;
    msg.content.cr.reading.real.value = real.val;
    msg.content.cr.reading.real.unit = ((real.unit == DIALBIG_MM) ? DISPLACEMENT_UNIT_MM : DISPLACEMENT_UNIT_IN);
    msg.content.cr.reading.real.num_decimals = real.decimals;
    msg.content.cr.reading.real.num_ints = 3;
    msg.content.cr.reading.real.tickcount = xTaskGetTickCount();
    msg.content.cr.index = 5;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
dialbig_t dial_big_2 = {
    .data_done_cb = dial_big_data_done_2_cb,
    .data_pin = DIALBIG_2_DATA_PIN,
    .clk_pin = DIALBIG_2_CLK_PIN,
    .req_pin = DIALBIG_2_REQ_PIN,
};

// SENSOR 7
void dial_big_data_done_3_cb(dialbig_data_t real)
{
    msg_t msg;
    msg.type = SENSOR_READING_RECEIVED;
    msg.content.cr.reading.real.value = real.val;
    msg.content.cr.reading.real.unit = ((real.unit == DIALBIG_MM) ? DISPLACEMENT_UNIT_MM : DISPLACEMENT_UNIT_IN);
    msg.content.cr.reading.real.num_decimals = real.decimals;
    msg.content.cr.reading.real.num_ints = 3;
    msg.content.cr.reading.real.tickcount = xTaskGetTickCount();
    msg.content.cr.index = 6;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
dialbig_t dial_big_3 = {
    .data_done_cb = dial_big_data_done_3_cb,
    .data_pin = DIALBIG_3_DATA_PIN,
    .clk_pin = DIALBIG_3_CLK_PIN,
    .req_pin = DIALBIG_3_REQ_PIN,
};

// SENSOR 8
void dial_big_data_done_4_cb(dialbig_data_t real)
{
    msg_t msg;
    msg.type = SENSOR_READING_RECEIVED;
    msg.content.cr.reading.real.value = real.val;
    msg.content.cr.reading.real.unit = ((real.unit == DIALBIG_MM) ? DISPLACEMENT_UNIT_MM : DISPLACEMENT_UNIT_IN);
    msg.content.cr.reading.real.num_decimals = real.decimals;
    msg.content.cr.reading.real.num_ints = 3;
    msg.content.cr.reading.real.tickcount = xTaskGetTickCount();
    msg.content.cr.index = 7;
    xQueueSend(main_queue, &msg, portMAX_DELAY);
}
dialbig_t dial_big_4 = {
    .data_done_cb = dial_big_data_done_4_cb,
    .data_pin = DIALBIG_4_DATA_PIN,
    .clk_pin = DIALBIG_4_CLK_PIN,
    .req_pin = DIALBIG_4_REQ_PIN,
};
void sensor_1_init()
{
    hx711_init(&load_cell_1);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_D;
    msg.content.cc.index = 0;*/
   // xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_2_init()
{
    hx711_init(&load_cell_2);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class= CLASS_D;
    msg.content.cc.index = 1;*/
   // xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_3_init()
{
    hx711_init(&load_cell_3);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_D;
    msg.content.cc.index = 2;*/
   // xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_4_init()
{
    hx711_init(&load_cell_4);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_D;
    msg.content.cc.index = 3;*/
   // xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_5_init()
{
    dialbig_init(&dial_big_1);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_I;
    msg.content.cc.index = 4;*/
  //  xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_6_init()
{
    dialbig_init(&dial_big_2);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_I;
    msg.content.cc.index = 5;*/
  //  xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_7_init()
{
    dialbig_init(&dial_big_3);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_I;
    msg.content.cc.index = 6;*/
  //  xQueueSend(main_queue, &msg, portMAX_DELAY);
}
void sensor_8_init()
{
    dialbig_init(&dial_big_4);
/*
    msg_t msg;
    msg.type = SENSOR_CLASS_RECEIVED;
    msg.content.cc.class = CLASS_I;
    msg.content.cc.index = 7;*/
  //  xQueueSend(main_queue, &msg, portMAX_DELAY);
}

void stask_sensor_disable_cb(msg_t *msg)
{
    int index = msg->content.i32;

    switch (index)
    {
    case 0:
        hx711_disable(&load_cell_1);
        break;
    case 1:
        hx711_disable(&load_cell_2);
        break;
    case 2:
        hx711_disable(&load_cell_3);
        break;
    case 3:
        hx711_disable(&load_cell_4);
        break;
    case 4:
        dialbig_disable(&dial_big_1);
        break;
    case 5:
        dialbig_disable(&dial_big_2);
        break;
    case 6:
        dialbig_disable(&dial_big_3);
        break;
    case 7:
        dialbig_disable(&dial_big_4);
        break;
    default:
        break;
    }
}
void stask_sensor_enable_cb(msg_t *msg)
{
    int index = msg->content.i32;

    switch (index)
    {
    case 0:
        hx711_enable(&load_cell_1);
        break;
    case 1:
        hx711_enable(&load_cell_2);
        break;
    case 2:
        hx711_enable(&load_cell_3);
        break;
    case 3:
        hx711_enable(&load_cell_4);
        break;
    case 4:
        dialbig_enable(&dial_big_1);
        break;
    case 5:
        dialbig_enable(&dial_big_2);
        break;
    case 6:
        dialbig_enable(&dial_big_3);
        break;
    case 7:
        dialbig_enable(&dial_big_4);
        break;
    default:
        break;
    }
}

