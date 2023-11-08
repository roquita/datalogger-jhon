#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "stdint.h"
#include "project_typedefs.h"
#include "stdbool.h"

typedef enum
{
    TEST_TYPE_CBR = 0,
    TEST_TYPE_LBR,
    TEST_TYPE_MARSHALL,
    TEST_TYPE_TSR,
    TEST_TYPE_SCB_ASHTO,
    TEST_TYPE_SCB_ASTM,
    TEST_TYPE_USER_DEFINED,
    TEST_TYPE_MAX,
} test_type_t;

typedef enum
{
    LOGGING_CONDITION_INTERVAL_LOGGING = 0,
    LOGGING_CONDITION_LINEAR_TIME_INTERVAL,
    LOGGING_CONDITION_INTERVAL_LOGGING_TABLE,
    LOGGING_CONDITION_ELAPSED_TIME_TABLE,
    LOGGING_CONDITION_MAX,
} logging_condition_t;

typedef enum
{
    LOGGING_DIRECTION_UP = 0,
    LOGGING_DIRECTION_DOWN,
    LOGGING_DIRECTION_MAX,
} logging_direction_t;

typedef enum
{
    STOP_CONDITION_GREATER_THAN = 0,
    STOP_CONDITION_LESS_THAN,
    STOP_CONDITION_TIME_DELAY,
    STOP_CONDITION_DISTANCE_UP,
    STOP_CONDITION_DISTANCE_DOWN,
    STOP_CONDITION_PERCENT_DROP,
    STOP_CONDITION_PERCENT_STRAIN,
    STOP_CONDITION_PERCENT_STRESS_DROP,
    STOP_CONDITION_OPERATOR_STOP,
    STOP_CONDITION_MAX,
} stop_condition_t;

typedef enum
{
    AXIS_NONE = -4,
    AXIS_TIME = -3,
    AXIS_SQR_ROOT_TIME = -2,
    AXIS_PERCENT_STRAIN = -1,
    AXIS_SENSOR_INDEX_0 = 0,
    AXIS_SENSOR_INDEX_1,
    AXIS_SENSOR_INDEX_2,
    AXIS_SENSOR_INDEX_3,
    AXIS_SENSOR_INDEX_4,
    AXIS_SENSOR_INDEX_5,
    AXIS_SENSOR_INDEX_6,
    AXIS_SENSOR_INDEX_7,
    AXIS_MAX,
} axis_option_t;

typedef enum
{
    START_CONDITION_TRIGGER_IMMEDIATELY = 0,
    START_CONDITION_TIME_DELAY,
    START_CONDITION_GREATER_THAN,
    START_CONDITION_LESS_THAN,
    START_CONDITION_MAX,
} start_condition_t;

typedef enum
{
    TAKEN_AT_TRIGGER = 0,
    TAKEN_AT_INITIAL_VALUE,
    TAKEN_AT_MAX,
} first_point_t;

typedef enum
{
    TEST_VOID = 0,
    TEST_INITIALIZED,
    TEST_CONFIGURED,
    TEST_TRIGGERING,
    TEST_RUNNING,
    TEST_DONE,
} test_status_t;

typedef struct
{
    union
    {
        struct
        {
            uint8_t s0 : 1;
            uint8_t s1 : 1;
            uint8_t s2 : 1;
            uint8_t s3 : 1;
            uint8_t s4 : 1;
            uint8_t s5 : 1;
            uint8_t s6 : 1;
            uint8_t s7 : 1;
        };
        uint8_t byte;
    } input_enable;
    union
    {
        struct
        {
            uint8_t s0 : 1;
            uint8_t s1 : 1;
            uint8_t s2 : 1;
            uint8_t s3 : 1;
            uint8_t s4 : 1;
            uint8_t s5 : 1;
            uint8_t s6 : 1;
            uint8_t s7 : 1;
        };
        uint8_t byte;
    } tara_enable;
    int sensor_num;

} test_inputs_t;

typedef struct
{
    uint8_t h; // HOUR
    uint8_t m; // MINUTE
    float s;   // SECOND - 2 DECIMALS
} test_time_t;

typedef struct
{
    logging_condition_t condition;
    union
    {
        struct
        {
            uint8_t sensor_index;          // FROM 0 TO 8
            logging_direction_t direction; // UP , DOWN
            double value;
        } interval_logging;
        struct
        {
            uint32_t value; // SECONDS
        } linear_time_interval;
        struct
        {
            uint8_t sensor_index;          // FROM 0 TO 8
            logging_direction_t direction; // UP , DOWN
            double value[30];              //  TABLE
            int size;
        } interval_logging_table;
        struct
        {
            test_time_t value[30]; // TABLE
            int size;
        } elapsed_time_table;
    } parameters;
} test_logging_t;

typedef struct
{
    stop_condition_t condition;
    union
    {
        struct
        {
            uint8_t sensor_index;
            double value; // unit
        } greater_than;
        struct
        {
            uint8_t sensor_index;
            double value; // unit
        } less_than;
        struct
        {
            uint32_t value;
        } time_delay; // secs
        struct
        {
            uint8_t sensor_index;
            double value; // unit
        } distance_up;
        struct
        {
            uint8_t sensor_index;
            double value; // unit
        } distance_down;
        struct
        {
            uint8_t sensor_index;
            double value; // %
        } percent_drop;
        struct
        {
            uint8_t sensor_index;
            double value; // %
        } percent_strain;
        struct
        {
            uint8_t sensor_index;
            double value; // %
        } percent_stress_drop;
        struct
        {
        } operator_stop;
    } parameters;
} test_stop_t;

typedef struct
{

} test_motor_t;

typedef struct
{
    axis_option_t x_axis_option;
    axis_option_t y_axis_option;
} test_graph_t;

typedef struct
{
    start_condition_t condition;
    union
    {
        struct
        {
        } trigger_immediately;
        struct
        {
            uint32_t value; // SECONDS
        } time_delay;
        struct
        {
            int sensor_index;     // FROM 0 TO 7
            double value;         // units
            double current_value; // actual value showing
            first_point_t first_point;
        } greater_than;
        struct
        {
            double value;         // units
            double current_value; // actual value showing
            int sensor_index;     // FROM 0 TO 7
            first_point_t first_point;
        } less_than;
    } parameters;
} test_start_t;

typedef struct
{
    test_type_t type;
    test_inputs_t inputs;
    test_logging_t logging;
    test_stop_t stop;
    test_motor_t motor;
    test_graph_t graph;
    test_start_t start;
    char test_name[TEST_NAME_LEN_MAX + 1]; // +1 for null end
} test_setup_t;

typedef struct
{
    char title[TEST_TITLE_LEN_MAX + 1]; // +1 for null end
    char sensor_name[NUM_SENSORS][CALIB_NAME_LEN_MAX + 1];
    char sensor_unit[NUM_SENSORS][SENSOR_UNIT_LEN_MAX + 1];
    char sensor_unitps[NUM_SENSORS][SENSOR_UNITPS_LEN_MAX + 1];
    int sensor_num_decimals[NUM_SENSORS];
    char Y_axis_name[CALIB_NAME_LEN_MAX + 1];
    char Y_axis_unit[SENSOR_UNIT_LEN_MAX + 1];
    char Y1_val[SENSOR_READING_LEN_MAX + 1];
    char Y2_val[SENSOR_READING_LEN_MAX + 1];
    char Y3_val[SENSOR_READING_LEN_MAX + 1];
    char Y4_val[SENSOR_READING_LEN_MAX + 1];
    char Y5_val[SENSOR_READING_LEN_MAX + 1];
    char Y6_val[SENSOR_READING_LEN_MAX + 1];
    char Y7_val[SENSOR_READING_LEN_MAX + 1];
    char Y8_val[SENSOR_READING_LEN_MAX + 1];
} test_aditional_info_t;

typedef struct
{
    int index;
    test_time_t time;
    double data[NUM_SENSORS];
} test_row_t;

typedef struct
{
    test_row_t row[TEST_ROWS_NUM_MAX];
    int row_num;
    double peak_values[NUM_SENSORS];
} test_table_t;

void test_set_operator_stop();
int test_get_tara_enable();
void test_print_elapsed_time();

/*
██████╗ ██╗   ██╗███╗   ██╗ █████╗ ███╗   ███╗██╗ ██████╗    ███╗   ███╗███████╗███╗   ███╗
██╔══██╗╚██╗ ██╔╝████╗  ██║██╔══██╗████╗ ████║██║██╔════╝    ████╗ ████║██╔════╝████╗ ████║
██║  ██║ ╚████╔╝ ██╔██╗ ██║███████║██╔████╔██║██║██║         ██╔████╔██║█████╗  ██╔████╔██║
██║  ██║  ╚██╔╝  ██║╚██╗██║██╔══██║██║╚██╔╝██║██║██║         ██║╚██╔╝██║██╔══╝  ██║╚██╔╝██║
██████╔╝   ██║   ██║ ╚████║██║  ██║██║ ╚═╝ ██║██║╚██████╗    ██║ ╚═╝ ██║███████╗██║ ╚═╝ ██║
╚═════╝    ╚═╝   ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝    ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝

*/

void free_test_setup(test_setup_t *test_setup);
void free_test_table(test_table_t *test_table);

/*
███████╗███████╗████████╗██╗   ██╗██████╗
██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
███████╗█████╗     ██║   ██║   ██║██████╔╝
╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
███████║███████╗   ██║   ╚██████╔╝██║
╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝

*/

esp_err_t test_set_type(test_type_t *type);
esp_err_t test_set_inputs(test_inputs_t *inputs);
esp_err_t test_set_logging(test_logging_t *logging);
esp_err_t test_set_stop(test_stop_t *stop);
esp_err_t test_set_motor(test_motor_t *motor);
esp_err_t test_set_graph(test_graph_t *graph);
esp_err_t test_set_start(test_start_t *start);

void test_print_type(void);
void test_print_inputs(void);
void test_print_logging(void);
void test_print_stop(void);
void test_print_motor(void);
void test_print_graph(void);
void test_print_start(void);

esp_err_t test_input_get_sensor_num_from_enablers(int *sensor_num, uint8_t enablers);
esp_err_t test_get_sensor_index_from_combobox(int combobox_val, uint8_t *sensor_index);
esp_err_t test_get_logging_direction_from_combobox(int combobox_val, logging_direction_t *direction);
esp_err_t test_get_graph_option_from_combobox(int combobox_val, axis_option_t *axis_option);
esp_err_t test_get_start_first_point_from_multiradio(int multiradio_val, first_point_t *first_point);
esp_err_t test_start_GreaterThan_set_sensor_index(int sensor_index);
esp_err_t test_start_GreaterThan_get_sensor_index(int *sensor_index);
esp_err_t test_start_LessThan_set_sensor_index(int sensor_index);
esp_err_t test_start_LessThan_get_sensor_index(int *sensor_index);
esp_err_t test_get_type_from_multiradio(int multiradio_val, test_type_t *test_type);

esp_err_t test_set_logging_page(page_t page);
esp_err_t test_set_stop_page(page_t page);
esp_err_t test_set_start_page(page_t page);

esp_err_t test_get_logging_page(page_t *page);
esp_err_t test_get_stop_page(page_t *page);
esp_err_t test_get_start_page(page_t *page);

/*
████████╗ █████╗ ██████╗ ██╗     ███████╗
╚══██╔══╝██╔══██╗██╔══██╗██║     ██╔════╝
   ██║   ███████║██████╔╝██║     █████╗
   ██║   ██╔══██║██╔══██╗██║     ██╔══╝
   ██║   ██║  ██║██████╔╝███████╗███████╗
   ╚═╝   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝

*/
esp_err_t test_table_add_row(int slot);
/*
██████╗ ██╗      ██████╗ ████████╗████████╗██╗███╗   ██╗ ██████╗
██╔══██╗██║     ██╔═══██╗╚══██╔══╝╚══██╔══╝██║████╗  ██║██╔════╝
██████╔╝██║     ██║   ██║   ██║      ██║   ██║██╔██╗ ██║██║  ███╗
██╔═══╝ ██║     ██║   ██║   ██║      ██║   ██║██║╚██╗██║██║   ██║
██║     ███████╗╚██████╔╝   ██║      ██║   ██║██║ ╚████║╚██████╔╝
╚═╝     ╚══════╝ ╚═════╝    ╚═╝      ╚═╝   ╚═╝╚═╝  ╚═══╝ ╚═════╝

*/
void current_test_plot_status(int slot);
void current_test_plot_last_data_p1(void);
void current_test_plot_last_data_p2_p3(int slot);
void current_test_load_foreground_slot(void);
void current_test_reset_default_p1_p2_p3();

/*
███████╗████████╗ █████╗ ██████╗ ████████╗     ██████╗ ██████╗ ███╗   ██╗██████╗ ██╗████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝    ██╔════╝██╔═══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██║██╔═══██╗████╗  ██║
███████╗   ██║   ███████║██████╔╝   ██║       ██║     ██║   ██║██╔██╗ ██║██║  ██║██║   ██║   ██║██║   ██║██╔██╗ ██║
╚════██║   ██║   ██╔══██║██╔══██╗   ██║       ██║     ██║   ██║██║╚██╗██║██║  ██║██║   ██║   ██║██║   ██║██║╚██╗██║
███████║   ██║   ██║  ██║██║  ██║   ██║       ╚██████╗╚██████╔╝██║ ╚████║██████╔╝██║   ██║   ██║╚██████╔╝██║ ╚████║
╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝        ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/
esp_err_t test_start_condition_trigger_immediately(int slot);
esp_err_t test_start_condition_time_delay(int slot);
esp_err_t test_start_condition_greater_than(int slot);
esp_err_t test_start_condition_less_than(int slot);

/*
███████╗████████╗ ██████╗ ██████╗      ██████╗ ██████╗ ███╗   ██╗██████╗ ██╗████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗    ██╔════╝██╔═══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██║██╔═══██╗████╗  ██║
███████╗   ██║   ██║   ██║██████╔╝    ██║     ██║   ██║██╔██╗ ██║██║  ██║██║   ██║   ██║██║   ██║██╔██╗ ██║
╚════██║   ██║   ██║   ██║██╔═══╝     ██║     ██║   ██║██║╚██╗██║██║  ██║██║   ██║   ██║██║   ██║██║╚██╗██║
███████║   ██║   ╚██████╔╝██║         ╚██████╗╚██████╔╝██║ ╚████║██████╔╝██║   ██║   ██║╚██████╔╝██║ ╚████║
╚══════╝   ╚═╝    ╚═════╝ ╚═╝          ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/
esp_err_t test_stop_condition_greater_than(int slot);
esp_err_t test_stop_condition_less_than(int slot);
esp_err_t test_stop_condition_time_delay(int slot);
esp_err_t test_stop_condition_distance_up(int slot);
esp_err_t test_stop_condition_distance_down(int slot);
esp_err_t test_stop_condition_percent_drop(int slot);
esp_err_t test_stop_condition_percent_strain(int slot);
esp_err_t test_stop_condition_percent_stress_drop(int slot);
esp_err_t test_stop_condition_operator_stop(int slot);

/*
██╗      ██████╗  ██████╗  ██████╗ ██╗███╗   ██╗ ██████╗      ██████╗ ██████╗ ███╗   ██╗██████╗ ██╗████████╗██╗ ██████╗ ███╗   ██╗
██║     ██╔═══██╗██╔════╝ ██╔════╝ ██║████╗  ██║██╔════╝     ██╔════╝██╔═══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██║██╔═══██╗████╗  ██║
██║     ██║   ██║██║  ███╗██║  ███╗██║██╔██╗ ██║██║  ███╗    ██║     ██║   ██║██╔██╗ ██║██║  ██║██║   ██║   ██║██║   ██║██╔██╗ ██║
██║     ██║   ██║██║   ██║██║   ██║██║██║╚██╗██║██║   ██║    ██║     ██║   ██║██║╚██╗██║██║  ██║██║   ██║   ██║██║   ██║██║╚██╗██║
███████╗╚██████╔╝╚██████╔╝╚██████╔╝██║██║ ╚████║╚██████╔╝    ╚██████╗╚██████╔╝██║ ╚████║██████╔╝██║   ██║   ██║╚██████╔╝██║ ╚████║
╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝      ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/
esp_err_t test_logging_condition_interval_logging(int slot);
esp_err_t test_logging_condition_linear_time_interval(int slot);
esp_err_t test_logging_condition_interval_logging_table(int slot);
esp_err_t test_logging_condition_elapsed_time_table(int slot);

/*
███████╗████████╗ ██████╗ ██████╗  █████╗  ██████╗ ███████╗
██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗██╔══██╗██╔════╝ ██╔════╝
███████╗   ██║   ██║   ██║██████╔╝███████║██║  ███╗█████╗
╚════██║   ██║   ██║   ██║██╔══██╗██╔══██║██║   ██║██╔══╝
███████║   ██║   ╚██████╔╝██║  ██║██║  ██║╚██████╔╝███████╗
╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝

*/
void test_init_filesystem(void);
void test_init_usb(void);
void test_save_on_usb(int slot);
void test_save_on_filesystem(int slot);
void usb_device_connected(int device_address);
void usb_device_disconnected();

/*
███████╗██╗      ██████╗ ████████╗
██╔════╝██║     ██╔═══██╗╚══██╔══╝
███████╗██║     ██║   ██║   ██║
╚════██║██║     ██║   ██║   ██║
███████║███████╗╚██████╔╝   ██║
╚══════╝╚══════╝ ╚═════╝    ╚═╝

*/
esp_err_t test_is_an_slot_in_execution();
esp_err_t test_get_available_slot(int *slot);
void test_deinit_all_slot_initialized();
void test_deinit_slot_configured(int slot);
esp_err_t test_init_available_slot();
void test_complete_initialized_slot(void);
void test_run_slot(int slot);
void test_run_all(void);