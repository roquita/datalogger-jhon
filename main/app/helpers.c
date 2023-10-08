#include "project_defines.h"
#include "project_typedefs.h"
#include "configuration.h"
#include "calibration.h"
#include "stdbool.h"
#include "stdint.h"
#include "esp_log.h"
#include "esp_err.h"
#include "new_test.h"

#define TAG "HELPERS"

bool helper_type_in_range(uint8_t type_u8)
{
    uint8_t t_min = (uint8_t)DISPLACEMENT;
    uint8_t t_max = (uint8_t)VOLUME;
    return ((type_u8 >= t_min) && (type_u8 <= t_max));
}

bool helper_unit_in_range(uint8_t unit_u8, sensor_type_t type)
{
    if (type == DISPLACEMENT)
    {
        uint8_t u = (uint8_t)(unit_u8);
        uint8_t u_min = (uint8_t)(DISPLACEMENT_UNIT_CM);
        uint8_t u_max = (uint8_t)(DISPLACEMENT_UNIT_MM);
        return ((u >= u_min && u <= u_max) ? true : false);
    }
    else if (type == LOAD)
    {
        uint8_t u = (uint8_t)(unit_u8);
        uint8_t u_min = (uint8_t)(LOAD_UNIT_KN);
        uint8_t u_max = (uint8_t)(LOAD_UNIT_KGF);
        return ((u >= u_min && u <= u_max) ? true : false);
    }
    else if (type == PRESSURE)
    {
        uint8_t u = (uint8_t)(unit_u8);
        uint8_t u_min = (uint8_t)(PRESSURE_UNIT_KPA);
        uint8_t u_max = (uint8_t)(PRESSURE_UNIT_KGF_CM2);
        return ((u >= u_min && u <= u_max) ? true : false);
    }
    else if (type == VOLUME)
    {
        uint8_t u = (uint8_t)(unit_u8);
        uint8_t u_min = (uint8_t)(VOLUME_UNIT_CM3);
        uint8_t u_max = (uint8_t)(VOLUME_UNIT_IN3);
        return ((u >= u_min && u <= u_max) ? true : false);
    }
    else
    {
        return false;
    }
}
bool helper_sensibility_unit_in_range(uint8_t sensibility_unit_u8)
{
    uint8_t s_min = (uint8_t)SENSIBILITY_UNIT_MV_V;
    uint8_t s_max = (uint8_t)SENSIBILITY_UNIT_V_V;
    return ((sensibility_unit_u8 >= s_min) && (sensibility_unit_u8 <= s_max));
}
bool helper_index_in_range(int index)
{
    return ((index >= SENSOR_1_INDEX) && (index < SENSOR_MAX_INDEX));
}
bool helper_num_points_in_range(uint8_t num_points_u8)
{
    return ((num_points_u8 == ((uint8_t)NUM_POINTS_1)) || (num_points_u8 == ((uint8_t)NUM_POINTS_5)) || (num_points_u8 == ((uint8_t)NUM_POINTS_10)) || (num_points_u8 == ((uint8_t)NUM_POINTS_CUSTOM)));
}
bool helper_row_to_fill_in_Range(uint8_t row_to_fill_u8)
{
    uint8_t r_min = (uint8_t)ROW_0_TO_FILL;
    uint8_t r_max = (uint8_t)ROW_NOT_TO_FILL;
    return ((row_to_fill_u8 >= r_min) && (row_to_fill_u8 <= r_max));
}
//***************************************** TO:
//******************[to][from]    cm        in         mm
double displacement_fc[3][3] = {{1.000000, 2.5399986, 0.10000}, // cm
                                {0.393701, 1.0000000, 0.03937}, // in   FROM:
                                {10.00000, 25.400000, 1.0000}}; // mm

//******************************************* TO:
//**********[to][from]      kN          lbf         N          kgf
double load_fc[4][4] = {{1.0000000, 0.0044482, 0.0010000, 0.0098066}, // kN
                        {224.80900, 1.0000000, 0.2248089, 2.2046244}, // lbf   FROM:
                        {1000.0000, 4.4482216, 1.0000000, 9.8066520}, // N
                        {101.97162, 0.4535920, 0.1019716, 1.000000}}; // kgf

//*********************************************** TO:
//***************[to][from]     kPa         psi         ksf          MPa      kgf/cm2
double pressure_fc[5][5] = {{1.0000000, 6.8947448, 47.8803374, 1000.000000, 98.0392156}, // kPa
                            {0.1450380, 1.0000000, 6.94443960, 145.0389429, 14.2233348}, // psi   FROM:
                            {0.0208854, 0.1440001, 1.00000000, 20.88541630, 2.04812680}, // ksf
                            {0.0010000, 0.0068947, 0.04788030, 1.000000000, 0.09806800}, // MPa
                            {0.0102000, 0.0703070, 0.48825100, 10.19700000, 1.0000000}}; // kgf/cm2

//******************************** TO:
//************[to][from]     cm3           in3
double volume_fc[2][2] = {{1.00000000, 16.387064}, // cm3
                          {0.06102374, 1.000000}}; // in3   FROM:

double helper_get_fc(sensor_unit_t old_unit, sensor_unit_t new_unit, sensor_type_t type)
{
    if (type == DISPLACEMENT)
    {
        return displacement_fc[new_unit][old_unit];
    }
    if (type == LOAD)
    {
        return load_fc[new_unit - LOAD_UNIT_KN][old_unit - LOAD_UNIT_KN];
    }
    if (type == PRESSURE)
    {
        return pressure_fc[new_unit - PRESSURE_UNIT_KPA][old_unit - PRESSURE_UNIT_KPA];
    }
    if (type == VOLUME)
    {
        return volume_fc[new_unit - VOLUME_UNIT_CM3][old_unit - VOLUME_UNIT_CM3];
    }

    return 1.0;
}

char *sensor_unit_str[] = {"cm", "in", "mm", "kN", "lbf", "N",
                           "kgf", "kPa", "psi", "ksf", "MPa",
                           "kgf/cm2", "cm3", "in3"};


