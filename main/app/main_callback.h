#pragma once

#include "project_defines.h"
#include "project_typedefs.h"

void main_task_init();

// PAGES LOADED
void home_loaded_cb(msg_t *msg);
void system_settings_loaded_cb(msg_t *msg);
void calibration_loaded_cb(msg_t *msg);
void inputcalibp1_loaded_cb(msg_t *msg);
void inputcalibp2_loaded_cb(msg_t *msg);
void inputcalibp3_loaded_cb(msg_t *msg);
void inputcalibp4_loaded_cb(msg_t *msg);
void numpad_popup_loaded_cb(msg_t *msg);
void new_test_loaded_cb(msg_t *msg);
void inputconfigp1_loaded_cb(msg_t *msg);

// TARA
void tara_enabled_cb(msg_t *msg);
void tara_disabled_cb(msg_t *msg);

// SENSOR ENABLE
void sensor_disabled_cb(msg_t *msg);
void sensor_enabled_cb(msg_t *msg);

// INPUT CALIB P1
void inputcalibp1_type_received_cb(msg_t *msg);
void inputcalibp1_capacity_received_cb(msg_t *msg);
void inputcalibp1_capacity_unit_received_cb(msg_t *msg);
void inputcalibp1_sensibility_received_cb(msg_t *msg);
void inputcalibp1_sensibility_unit_received_cb(msg_t *msg);
void inputcalibp1_sensor_index_received(msg_t *msg);
// INPUT CALIB P2
void inputcalibp2_calibration_limit_received_cb(msg_t *msg);
void inputcalibp2_limit_enable_receive_cb(msg_t *msg);
// INPUT CALIB P3
void inputcalibp3_table_received(msg_t *msg);
void inputcalibp3_num_points_received(msg_t *msg);
void inputcalibp3_row_to_fill_received(msg_t *msg);
// INPUT CALIB P4
void inputcalibp4_name_received_cb(msg_t *msg);
void inputcalibp4_save_pressed_cb(msg_t *msg);
// INPUT CONFIGURATION P1
void inputconfigp1_type_received_cb(msg_t *msg);
void inputconfigp1_capacity_received_cb(msg_t *msg);
void inputconfigp1_capacity_unit_received_cb(msg_t *msg);
void inputconfigp1_name_received_cb(msg_t *msg);
void inputconfigp1_index_received_cb(msg_t *msg);
void inputconfigp1_save_pressed_cb(msg_t *msg);

// SENSOR UNIT CHANGED
void sensor_unit_changed(msg_t *msg);

// LIMITS ENABLED
void sensor_limits_enabled(msg_t *msg);

// LIMITS DISABLED
void sensor_limits_disabled(msg_t *msg);

// MSG FROM SENSOR TASK
void sensor_reading_received_cb(msg_t *msg);
void sensor_class_received_cb(msg_t *msg);

// NEXTION-TIMER
void nextion_update_cb(msg_t *msg);

// NEW TEST
void test_execute_cb(msg_t *msg);
void new_test_set_index_1_cb(msg_t *msg);
void new_test_set_index2_cb(msg_t *msg);
void new_test_start_cb(msg_t *msg);
void new_test_stop_cb(msg_t *msg);
void new_test_download_cb(msg_t *msg);
