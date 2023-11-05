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
void keybda_loaded_cb(msg_t *msg);
void numpad_popup_loaded_cb(msg_t *msg);
void intro_loaded_cb(msg_t *msg);
void inputconfigp1_loaded_cb(msg_t *msg);
void new_test_p1_loaded_cb(msg_t *msg);
void new_test_p2_loaded_cb(msg_t *msg);
void new_test_p3_1_loaded_cb(msg_t *msg);
void new_test_p3_2_loadedcb(msg_t *msg);
void new_test_p3_3_loadedcb(msg_t *msg);
void new_test_p3_4_loadedcb(msg_t *msg);
void log_sensor_table_loaded_cb(msg_t *msg);
void log_time_table_loaded_cb(msg_t *msg);
void new_test_p4_1_loaded_cb(msg_t *msg);
void new_test_p4_2_loaded_cb(msg_t *msg);
void new_test_p4_3_loaded_cb(msg_t *msg);
void new_test_p4_4_loaded_cb(msg_t *msg);
void new_test_p4_5_loaded_cb(msg_t *msg);
void new_test_p4_6_loaded_cb(msg_t *msg);
void new_test_p4_7_loaded_cb(msg_t *msg);
void new_test_p4_8_loaded_cb(msg_t *msg);
void new_test_p4_9_loaded_cb(msg_t *msg);
void new_test_p5_loaded_cb(msg_t *msg);
void new_test_p6_loaded_cb(msg_t *msg);
void new_test_p7_1_loaded_cb(msg_t *msg);
void new_test_p7_2_loaded_cb(msg_t *msg);
void new_test_p7_3_loaded_cb(msg_t *msg);
void new_test_p7_4_loaded_cb(msg_t *msg);
void previous_test_p1_loaded_cb(msg_t *msg);
void previous_test_p2_loaded_cb(msg_t *msg);
void previous_test_p3_loaded_cb(msg_t *msg);
void export_test_loaded_cb(msg_t *msg);
void delete_test_loaded_cb(msg_t *msg);
void message_loaded_cb(msg_t *msg);
void current_test_p1_loaded_cb(msg_t *msg);
void current_test_p2_loaded_cb(msg_t *msg);
void current_test_p3_loaded_cb(msg_t *msg);

// HOME
void tara_enabled_cb(msg_t *msg);
void tara_disabled_cb(msg_t *msg);
void goto_new_test_from_home_cb(msg_t *msg);
void goto_current_test_from_home_cb(msg_t *msg);

// CALIBRATION
void sensor_disabled_cb(msg_t *msg);
void sensor_enabled_cb(msg_t *msg);
void sensor_limits_disabled(msg_t *msg);
void sensor_limits_enabled(msg_t *msg);

// DATETIME
void datetime_rtc_all_at_once_received_cb(msg_t *msg);

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

// NEW TEST P1
void new_test_p1_type_received(msg_t *msg);
void new_test_p2_inputs_received(msg_t *msg);
void new_test_p3_1_logging_received_cb(msg_t *msg);
void new_test_p3_2_logging_received_cb(msg_t *msg);
void new_test_p3_3_logging_received_cb(msg_t *msg);
void new_test_p3_4_logging_received_cb(msg_t *msg);
void new_test_p4_1_stop_received_cb(msg_t *msg);
void new_test_p4_2_stop_received_cb(msg_t *msg);
void new_test_p4_3_stop_received_cb(msg_t *msg);
void new_test_p4_4_stop_received_cb(msg_t *msg);
void new_test_p4_5_stop_received_cb(msg_t *msg);
void new_test_p4_6_stop_received_cb(msg_t *msg);
void new_test_p4_7_stop_received_cb(msg_t *msg);
void new_test_p4_8_stop_received_cb(msg_t *msg);
void new_test_p4_9_stop_received_cb(msg_t *msg);
void new_test_p6_graph_received_cb(msg_t *msg);
void new_test_p7_1_start_received_cb(msg_t *msg);
void new_test_p7_2_start_received_cb(msg_t *msg);
void new_test_p7_3_start_received_cb(msg_t *msg);
void new_test_p7_3_trigger_input_received_cb(msg_t *msg);
void new_test_p7_4_start_received_cb(msg_t *msg);
void new_test_p7_4_trigger_input_received_cb(msg_t *msg);

// SENSOR UNIT CHANGED
void sensor_unit_changed(msg_t *msg);

// MSG FROM SENSOR TASK
void sensor_reading_received_cb(msg_t *msg);
void sensor_class_received_cb(msg_t *msg);

// NEXTION-TIMER
void nextion_update_cb(msg_t *msg);

//  MAIN TIMER
void periodic_100ms_cb(msg_t *msg);
void periodic_1s_cb(msg_t *msg);

// TIME TIMER
void time_rtc_start_sync_cb(msg_t *msg);
void time_rtc_data_received_cb(msg_t *msg);
void time_datetime_print_cb(msg_t *msg);