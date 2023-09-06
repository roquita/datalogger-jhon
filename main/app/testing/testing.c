#include "testing.h"
#include "stdbool.h"
#include "project_defines.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "project_tasks.h"
#include "project_typedefs.h"
#include "stdint.h"

TimerHandle_t timer_test = NULL;
test_status_t test_status = TEST_STOPPED;
int _index1 = 0;
int _index2 = 0;
static double table[10][2];
static uint8_t row = 0;

void test_timer_cb(TimerHandle_t xTimer)
{
    SEND_EMPTY_MSG(main_queue, TEST_EXECUTE, portMAX_DELAY);
}

bool test_is_running()
{
    return test_status == TEST_RUNNIG;
}
void test_set_index1(int index)
{
    if (index >= 0 && index < NUM_SENSORS)
        _index1 = index;
}
int test_get_index1()
{
    return _index1;
}
void test_set_index2(int index)
{
    if (index >= 0 && index < NUM_SENSORS)
        _index2 = index;
}
int test_get_index2()
{
    return _index2;
}
bool test_execute(double real1, double real2)
{
    bool res = false;
    if (real1 >= table[row][0])
    {
        table[row][0] = real1;
        table[row][1] = real2;
        row++;
        res = true;
    }
    if (row >= 10)
    {
        row = 0;
    }
    return res;
}
uint8_t test_get_row()
{
    return row;
}
void test_start()
{

    table[0][0] = 0.000;
    table[1][0] = 0.635;
    table[2][0] = 1.270;
    table[3][0] = 1.905;
    table[4][0] = 2.540;
    table[5][0] = 3.810;
    table[6][0] = 5.080;
    table[7][0] = 6.350;
    table[8][0] = 7.620;
    table[9][0] = 10.160;
    table[0][1] = 0.0;
    table[1][1] = 0.0;
    table[2][1] = 0.0;
    table[3][1] = 0.0;
    table[4][1] = 0.0;
    table[5][1] = 0.0;
    table[6][1] = 0.0;
    table[7][1] = 0.0;
    table[8][1] = 0.0;
    table[9][1] = 0.0;
    test_status = TEST_RUNNIG;
    timer_test = xTimerCreate("timer_test", pdMS_TO_TICKS(100), pdTRUE, NULL, test_timer_cb);
    xTimerStart(timer_test, portMAX_DELAY);
}
void test_stop()
{
    if (test_status != TEST_RUNNIG)
        return;

    test_status = TEST_STOPPED;
    xTimerDelete(timer_test, portMAX_DELAY);
}