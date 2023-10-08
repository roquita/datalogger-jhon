#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// USED WHEN THERE IS NO CONTENT
#define SEND_EMPTY_MSG(queue_dest, msg_type, timeout_ticks) \
    {                                                       \
        msg_t msg;                                          \
        msg.type = msg_type;                                \
        xQueueSend(queue_dest, &msg, timeout_ticks);        \
    }
// USED WHEN THERE IS NO CONTENT
#define SEND2FRONT_EMPTY_MSG(queue_dest, msg_type, timeout_ticks) \
    {                                                             \
        msg_t msg;                                                \
        msg.type = msg_type;                                      \
        xQueueSendToFront(queue_dest, &msg, timeout_ticks);       \
    }
// USED WHEN THERE IS DYNAMIC CONTENT FROM GUI
#define SEND_ADDR_MSG(_queue_dest, _msg_type, _content, _size, _timeout_ticks) \
    {                                                                          \
        msg_t msg;                                                             \
        msg.type = _msg_type;                                                  \
        msg.content.addr = _content;                                           \
        msg.size = _size;                                                      \
        xQueueSend(_queue_dest, &msg, _timeout_ticks);                         \
    }
// USED TO SEND I32 STATIC CONTENT FROM MCU
#define SEND_I32_MSG(_queue_dest, _msg_type, _i32, _timeout_ticks) \
    {                                                              \
        msg_t msg;                                                 \
        msg.type = _msg_type;                                      \
        msg.content.i32 = _i32;                                    \
        xQueueSend(_queue_dest, &msg, _timeout_ticks);             \
    }
// USED TO SEND DYNAMIC CONTENT AND INDEX FROM GUI
#define SEND_CONTENT_STR_MSG(_queue_dest, _msg_type, _str, _strlen, _index, _timeout_ticks) \
    {                                                                                       \
        msg_t msg;                                                                          \
        msg.type = _msg_type;                                                               \
        msg.content.cs.str = _str;                                                          \
        msg.content.cs.index = _index;                                                      \
        msg.size = _strlen;                                                                 \
        xQueueSend(_queue_dest, &msg, _timeout_ticks);                                      \
    }
extern QueueHandle_t main_queue;
extern QueueHandle_t tx_queue;
extern QueueHandle_t sensor_queue;
void main_task(void *arg);
void sensor_task(void *arg);
void nextion_rx_task(void *arg);
void nextion_tx_task(void *arg);
