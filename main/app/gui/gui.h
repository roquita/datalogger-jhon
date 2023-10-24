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
    int home_Use;
    int calibration_Switch;
    int calibration_Name;
    int calibration_Range;
    int calibration_Limit;
    int calibration_Units;
    int calibration_Value;
    int calibration_Counts;
    int calibration_Calibrate;
    int current_test_p2_Value;
} IdSx_t;

void nextion_1_init();
void nextion_1_start_timer();
void nextion_1_stop_timer();
void nextion_1_set_page(page_t new_page);
page_t nextion_1_get_page();

/*
 ██████╗ ███████╗███╗   ██╗███████╗██████╗  █████╗ ██╗
██╔════╝ ██╔════╝████╗  ██║██╔════╝██╔══██╗██╔══██╗██║
██║  ███╗█████╗  ██╔██╗ ██║█████╗  ██████╔╝███████║██║
██║   ██║██╔══╝  ██║╚██╗██║██╔══╝  ██╔══██╗██╔══██║██║
╚██████╔╝███████╗██║ ╚████║███████╗██║  ██║██║  ██║███████╗
 ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝

*/

void nextion_1_change_page(page_t page);

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
void nextion_1_message_write_nextpage(page_t content);
void nextion_1_message_write_content_color(uint32_t content);

/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ██████╗  ██╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗███║
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║       ██████╔╝╚██║
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝  ██║
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║       ██║      ██║
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝      ╚═╝

*/

/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ██████╗ ██████╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██╗
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║       ██████╔╝ █████╔╝
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝ ██╔═══╝
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║       ██║     ███████╗
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚══════╝

*/
void nextion_1_current_test_p2_append_index(char *content);
void nextion_1_current_test_p2_append_time(char *content);
void nextion_1_current_test_p2_append_sensor(int index, char *content);

/*
 ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗    ████████╗███████╗███████╗████████╗    ██████╗ ██████╗
██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗╚════██╗
██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║          ██║   █████╗  ███████╗   ██║       ██████╔╝ █████╔╝
██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║          ██║   ██╔══╝  ╚════██║   ██║       ██╔═══╝  ╚═══██╗
╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║          ██║   ███████╗███████║   ██║       ██║     ██████╔╝
 ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝          ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚═╝     ╚═════╝

*/
void nextion_1_current_test_p3_write_waveform(uint8_t content);
void nextion_1_current_test_p3_append_value(int content);

