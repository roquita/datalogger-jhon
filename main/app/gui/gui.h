#pragma once
#include "project_typedefs.h"

typedef struct
{
    int home_Name;
    int home_Data;
    int home_Dataps;
    int home_Unit;
    int home_Unitps;
    int home_Tara;
    int home_Status;
    int calibration_Switch;
    int calibration_Name;
    int calibration_Range;
    int calibration_Limit;
    int calibration_Units;
    int calibration_Value;
    int calibration_Counts;
    int calibration_Calibrate;
    int currenttestp1_Name;
    int currenttestp1_Data;
    int currenttestp1_Dataps;
    int currenttestp1_Unit;
    int currenttestp1_Unitps;
    int currenttestp2_Name;
    int currenttestp2_Unit;
    int currenttestp2_Data;
} IdSx_t;

/*
████████╗██╗███╗   ███╗███████╗██████╗
╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗
   ██║   ██║██╔████╔██║█████╗  ██████╔╝
   ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗
   ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║
   ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝

*/
/*
void nextion_1_init_timer();
void nextion_1_start_timer();
void nextion_1_stop_timer();
*/
/*
 ██████╗ ███████╗███╗   ██╗███████╗██████╗  █████╗ ██╗
██╔════╝ ██╔════╝████╗  ██║██╔════╝██╔══██╗██╔══██╗██║
██║  ███╗█████╗  ██╔██╗ ██║█████╗  ██████╔╝███████║██║
██║   ██║██╔══╝  ██║╚██╗██║██╔══╝  ██╔══██╗██╔══██║██║
╚██████╔╝███████╗██║ ╚████║███████╗██║  ██║██║  ██║███████╗
 ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝

*/
void nextion_1_init_hardware();
void nextion_1_set_page(page_t new_page);
page_t nextion_1_get_page();
void nextion_1_goto_page(page_t page);

/*
██╗███╗   ██╗████████╗██████╗  ██████╗
██║████╗  ██║╚══██╔══╝██╔══██╗██╔═══██╗
██║██╔██╗ ██║   ██║   ██████╔╝██║   ██║
██║██║╚██╗██║   ██║   ██╔══██╗██║   ██║
██║██║ ╚████║   ██║   ██║  ██║╚██████╔╝
╚═╝╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝

*/

// WRITE LOADING
void nextion_1_intro_write_loading(int content);

/*
██╗  ██╗ ██████╗ ███╗   ███╗███████╗
██║  ██║██╔═══██╗████╗ ████║██╔════╝
███████║██║   ██║██╔████╔██║█████╗
██╔══██║██║   ██║██║╚██╔╝██║██╔══╝
██║  ██║╚██████╔╝██║ ╚═╝ ██║███████╗
╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝

*/

// WRITE DATA
void nextion_1_home_write_data(int index, char *content);

// WRITE DATAPS
void nextion_1_home_write_dataps(int index, char *content);

// WRITE UNIT
void nextion_1_home_write_unit(int index, char *content);

// WRITE UNITPS
void nextion_1_home_write_unitps(int index, char *content);

// WRITE NAME
void nextion_1_home_write_name(int index, char *content);

// WRITE STATUS
void nextion_1_home_write_status(int index, char *content);

/*
 ██████╗ █████╗ ██╗     ██╗██████╗ ██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝██╔══██╗██║     ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██║     ███████║██║     ██║██████╔╝██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║
██║     ██╔══██║██║     ██║██╔══██╗██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
╚██████╗██║  ██║███████╗██║██████╔╝██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
 ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

// WRITE VALUE
void nextion_1_calibration_write_value(int index, char *content);

// WRITE UNIT
void nextion_1_calibration_write_unit(int index, char *content);

// WRITE NAME
void nextion_1_calibration_write_name(int index, char *content);

// WRITE RANGE
void nextion_1_calibration_write_range(int index, char *content);

// WRITE UNITS PATH
void nextion_1_calibration_write_units_path(int index, char *content);

// WRITE UNITS VAL
void nextion_1_calibration_write_units_val(int index, int32_t content);

// WRITE LIMIT
void nextion_1_calibration_write_limit(int index, int32_t content);

// WRITE SWITCH
void nextion_1_calibration_write_switch(int index, int32_t content);

// WRITE CONTENT
void nextion_1_calibration_write_counts(int index, char *content);

/*
██████╗  █████╗ ████████╗███████╗████████╗██╗███╗   ███╗███████╗
██╔══██╗██╔══██╗╚══██╔══╝██╔════╝╚══██╔══╝██║████╗ ████║██╔════╝
██║  ██║███████║   ██║   █████╗     ██║   ██║██╔████╔██║█████╗
██║  ██║██╔══██║   ██║   ██╔══╝     ██║   ██║██║╚██╔╝██║██╔══╝
██████╔╝██║  ██║   ██║   ███████╗   ██║   ██║██║ ╚═╝ ██║███████╗
╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝   ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝

*/

void nextion_1_datetime_get_rtc_year();
void nextion_1_datetime_get_rtc_month();
void nextion_1_datetime_get_rtc_day();
void nextion_1_datetime_get_rtc_hour();
void nextion_1_datetime_get_rtc_minute();
void nextion_1_datetime_get_rtc_second();
void nextion_1_datetime_get_rtc_dateformat();
void nextion_1_datetime_get_rtc_timeformat();
void nextion_1_set_datetime(char *content);

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██████╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗╚════██╗
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝ █████╔╝
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝  ╚═══██╗
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║     ██████╔╝
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝     ╚═════╝

*/

void nextion_1_inputcalibp3_write_real0(char *content);
void nextion_1_inputcalibp3_write_real1(char *content);
void nextion_1_inputcalibp3_write_real2(char *content);
void nextion_1_inputcalibp3_write_real3(char *content);
void nextion_1_inputcalibp3_write_real4(char *content);
void nextion_1_inputcalibp3_write_real5(char *content);
void nextion_1_inputcalibp3_write_real6(char *content);
void nextion_1_inputcalibp3_write_real7(char *content);
void nextion_1_inputcalibp3_write_real8(char *content);
void nextion_1_inputcalibp3_write_real9(char *content);
void nextion_1_inputcalibp3_write_real10(char *content);

void nextion_1_inputcalibp3_write_raw0(char *content);
void nextion_1_inputcalibp3_write_raw1(char *content);
void nextion_1_inputcalibp3_write_raw2(char *content);
void nextion_1_inputcalibp3_write_raw3(char *content);
void nextion_1_inputcalibp3_write_raw4(char *content);
void nextion_1_inputcalibp3_write_raw5(char *content);
void nextion_1_inputcalibp3_write_raw6(char *content);
void nextion_1_inputcalibp3_write_raw7(char *content);
void nextion_1_inputcalibp3_write_raw8(char *content);
void nextion_1_inputcalibp3_write_raw9(char *content);
void nextion_1_inputcalibp3_write_raw10(char *content);

/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ █████╗ ██╗     ██╗██████╗     ██████╗ ██╗  ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔══██╗██║     ██║██╔══██╗    ██╔══██╗██║  ██║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ███████║██║     ██║██████╔╝    ██████╔╝███████║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██╔══██║██║     ██║██╔══██╗    ██╔═══╝ ╚════██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗██║  ██║███████╗██║██████╔╝    ██║          ██║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝     ╚═╝          ╚═╝

*/

void nextion_1_inputcalibp4_write_result(char *content);
void nextion_1_inputcalibp4_write_result_color(uint32_t content);
/*
██╗███╗   ██╗██████╗ ██╗   ██╗████████╗     ██████╗ ██████╗ ███╗   ██╗███████╗██╗ ██████╗     ██████╗  ██╗
██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝    ██╔════╝██╔═══██╗████╗  ██║██╔════╝██║██╔════╝     ██╔══██╗███║
██║██╔██╗ ██║██████╔╝██║   ██║   ██║       ██║     ██║   ██║██╔██╗ ██║█████╗  ██║██║  ███╗    ██████╔╝╚██║
██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║       ██║     ██║   ██║██║╚██╗██║██╔══╝  ██║██║   ██║    ██╔═══╝  ██║
██║██║ ╚████║██║     ╚██████╔╝   ██║       ╚██████╗╚██████╔╝██║ ╚████║██║     ██║╚██████╔╝    ██║      ██║
╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝        ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝     ╚═╝ ╚═════╝     ╚═╝      ╚═╝

*/
void nextion_1_inputconfigp1_write_result(char *content);
void nextion_1_inputconfigp1_write_result_color(uint32_t content);

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ███████╗      ██████╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██║      ╚════██╗
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝    ██╔╝       █████╔╝
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝    ██╔╝        ╚═══██╗
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║        ██║███████╗██████╔╝
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝        ╚═╝╚══════╝╚═════╝

*/

void nextion_1_new_test_p7_3_write_current_value(char *content);

/*
███╗   ██╗███████╗██╗    ██╗    ████████╗███████╗███████╗████████╗    ██████╗ ███████╗ ██╗  ██╗
████╗  ██║██╔════╝██║    ██║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██║ ██║  ██║
██╔██╗ ██║█████╗  ██║ █╗ ██║       ██║   █████╗  ███████╗   ██║       ██████╔╝    ██╔╝ ███████║
██║╚██╗██║██╔══╝  ██║███╗██║       ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝    ██╔╝  ╚════██║
██║ ╚████║███████╗╚███╔███╔╝       ██║   ███████╗███████║   ██║       ██║        ██║███████╗██║
╚═╝  ╚═══╝╚══════╝ ╚══╝╚══╝        ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝        ╚═╝╚══════╝╚═╝

*/

void nextion_1_new_test_p7_4_write_current_value(char *content);

/*
███╗   ███╗███████╗███████╗███████╗ █████╗  ██████╗ ███████╗
████╗ ████║██╔════╝██╔════╝██╔════╝██╔══██╗██╔════╝ ██╔════╝
██╔████╔██║█████╗  ███████╗███████╗███████║██║  ███╗█████╗
██║╚██╔╝██║██╔══╝  ╚════██║╚════██║██╔══██║██║   ██║██╔══╝
██║ ╚═╝ ██║███████╗███████║███████║██║  ██║╚██████╔╝███████╗
╚═╝     ╚═╝╚══════╝╚══════╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝

*/
void nextion_1_message_write_content(char *content);
void nextion_1_message_append_content(char *content);
void nextion_1_message_write_nextpage(page_t content);
void nextion_1_message_write_content_color(uint32_t content);

/*
██████╗  ██╗     ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗
██╔══██╗███║    ██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝
██████╔╝╚██║    ██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║
██╔═══╝  ██║    ██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║
██║      ██║    ╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║
╚═╝      ╚═╝     ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝

*/

// TITLE
void nextion_1_current_test_p1_write_title(char *content);
void nextion_1_current_test_p1_append_title(char *content);

// SENSOR NAME
void nextion_1_current_test_p1_write_sensor_name(int index, char *content);

// WRITE DATA
void nextion_1_current_test_p1_write_data(int index, char *content);

// WRITE DATAPS
void nextion_1_current_test_p1_write_dataps(int index, char *content);

// UNIT
void nextion_1_current_test_p1_write_unit(int index, char *content);

// UNITPS
void nextion_1_current_test_p1_write_unitps(int index, char *content);

// STATUS
void nextion_1_current_test_p1_write_status(char *content);

// POINTs
void nextion_1_current_test_p1_write_points(int content);

// TIME
void nextion_1_current_test_p1_write_time(char *content);

/*
██████╗ ██████╗      ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗
██╔══██╗╚════██╗    ██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝
██████╔╝ █████╔╝    ██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║
██╔═══╝ ██╔═══╝     ██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║
██║     ███████╗    ╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║
╚═╝     ╚══════╝     ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝

*/

// TITLE
void nextion_1_current_test_p2_write_title(char *content);
void nextion_1_current_test_p2_append_title(char *content);

// NAME
void nextion_1_current_test_p2_write_sensor_name(int index, char *content);

// UNIT
void nextion_1_current_test_p2_write_unit(int index, char *content);

// TABLE INDEX
void nextion_1_current_test_p2_append_index(char *content);
void nextion_1_current_test_p2_clean_index();

// TABLE TIME
void nextion_1_current_test_p2_append_time(char *content);
void nextion_1_current_test_p2_clean_time();

// TABLE DATA
void nextion_1_current_test_p2_append_sensor_data(int index, char *content);
void nextion_1_current_test_p2_clean_sensor_data(int index);

// STATUS
void nextion_1_current_test_p2_write_status(char *content);

// POINTs
void nextion_1_current_test_p2_write_points(int content);

// TIME
void nextion_1_current_test_p2_write_time(char *content);

/*
██████╗ ██████╗      ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗
██╔══██╗╚════██╗    ██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝
██████╔╝ █████╔╝    ██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║
██╔═══╝  ╚═══██╗    ██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║
██║     ██████╔╝    ╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║
╚═╝     ╚═════╝      ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝

*/

// TITLE
void nextion_1_current_test_p3_write_title(char *content);
void nextion_1_current_test_p3_append_title(char *content);

// void nextion_1_current_test_p3_write_waveform(uint8_t content);

//  DATA
void nextion_1_current_test_p3_write_slope(int pos, int content);
void nextion_1_current_test_p3_clean_sensor_data();
void nextion_1_current_test_p3_append_data(int content);

// STATUS
void nextion_1_current_test_p3_write_status(char *content);

// POINTs
void nextion_1_current_test_p3_write_points(int content);

// TIME
void nextion_1_current_test_p3_write_time(char *content);

// Y axis
void nextion_1_current_test_p3_write_y_axis_name(char *content);
void nextion_1_current_test_p3_write_y_axis_unit(char *content);
void nextion_1_current_test_p3_write_Y1(char *content);
void nextion_1_current_test_p3_write_Y2(char *content);
void nextion_1_current_test_p3_write_Y3(char *content);
void nextion_1_current_test_p3_write_Y4(char *content);
void nextion_1_current_test_p3_write_Y5(char *content);
void nextion_1_current_test_p3_write_Y6(char *content);
void nextion_1_current_test_p3_write_Y7(char *content);
void nextion_1_current_test_p3_write_Y8(char *content);
