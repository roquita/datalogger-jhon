#pragma once

#include "project_defines.h"
#include "project_typedefs.h"
#include "configuration.h"
#include "calibration.h"
#include "stdbool.h"
#include "stdint.h"
#include "esp_log.h"
#include "esp_err.h"

bool helper_type_in_range(uint8_t type_u8);
bool helper_unit_in_range(uint8_t unit_u8, sensor_type_t type);
bool helper_sensibility_unit_in_range(uint8_t sensibility_unit_u8);
bool helper_index_in_range(int index);
bool helper_num_points_in_range(uint8_t num_points_u8);
bool helper_row_to_fill_in_Range(uint8_t row_to_fill_u8);
double helper_get_fc(sensor_unit_t old_unit, sensor_unit_t new_unit, sensor_type_t type);