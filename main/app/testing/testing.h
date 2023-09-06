#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef enum{
    TEST_RUNNIG,
    TEST_STOPPED,
}test_status_t;


bool test_is_running();
void test_set_index1(int index);
int test_get_index1();
void test_set_index2(int index);
int test_get_index2();
bool test_execute(double real1, double real2);
uint8_t test_get_row();
void test_start();
void test_stop();