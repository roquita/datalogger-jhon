
#include <string.h>
#include "project_defines.h"
#include "project_typedefs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "project_tasks.h"
#include "test.h"
#include "time_esp32s2.h"
#include "gui.h"
#include "math.h"
#include <sys/stat.h>
#include <errno.h>

// usb
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "msc_host.h"
#include "msc_host_vfs.h"
#include "ffconf.h"
#include "esp_vfs.h"
#include "errno.h"
#include "hal/usb_hal.h"
#include "driver/gpio.h"
#include <esp_vfs_fat.h>

#define TAG "TEST"

static test_setup_t _test_setup[CURRENT_TEST_NUM_MAX];
static page_t _loggging_page;
static page_t _stop_page;
static page_t _start_page;

static test_table_t _test_table[CURRENT_TEST_NUM_MAX];
static double _test_setup_timestamp[CURRENT_TEST_NUM_MAX];
static double _test_start_timestamp[CURRENT_TEST_NUM_MAX];
static double _test_stop_timestamp[CURRENT_TEST_NUM_MAX];
static test_status_t _test_status[CURRENT_TEST_NUM_MAX] = {TEST_VOID};
static int _initialized_slot = CURRENT_TEST_NUM_MAX;
static int _foreground_slot = CURRENT_TEST_NUM_MAX;
static bool _operator_stop[CURRENT_TEST_NUM_MAX];

static test_aditional_info_t _test_aditional_info[CURRENT_TEST_NUM_MAX];

static char buffer[60];

static char csv[5000];
static int csv_size;

void test_set_operator_stop()
{
    if (_foreground_slot == CURRENT_TEST_NUM_MAX)
        return;

    _operator_stop[_foreground_slot] = true;
}
static void test_generate_csv(int slot)
{
    assert(_test_status[slot] == TEST_DONE);

    // global var
    test_setup_t *setup = _test_setup + slot;
    test_table_t *table = _test_table + slot;
    test_aditional_info_t *info = _test_aditional_info + slot;
    double setup_timestamp = *(_test_setup_timestamp + slot);
    double start_timestamp = *(_test_setup_timestamp + slot);
    double stop_timestamp = *(_test_stop_timestamp + slot);

    csv_size = 0;
    //  line: test name
    csv_size += sprintf(csv + csv_size, "TEST NAME;%.*s;\n", TEST_NAME_LEN_MAX, setup->test_name);
    //  line: setup datetime formated
    csv_size += sprintf(csv + csv_size, "SETUP DATETIME;%s;\n", time_get_datetime_formated(true, (int64_t)setup_timestamp));
    //  line: start datetime formated
    csv_size += sprintf(csv + csv_size, "START DATETIME;%s;\n", time_get_datetime_formated(true, (int64_t)start_timestamp));
    //  line: stop datetime formated
    csv_size += sprintf(csv + csv_size, "STOP DATETIME;%s;\n", time_get_datetime_formated(true, (int64_t)stop_timestamp));
    //  line: date format
    csv_size += sprintf(csv + csv_size, "DATE FORMAT;%s;\n", time_get_date_format_str());
    //  line: time format
    csv_size += sprintf(csv + csv_size, "DATE TIME;%s;\n", time_get_time_format_str());
    //  line: void
    csv_size += sprintf(csv + csv_size, ";\n");
    //  line: table headers (index - time - sensors name)
    csv_size += sprintf(csv + csv_size, ";INDEX;TIME;");

    for (int i = 0; i < NUM_SENSORS; i++)
    {
        csv_size += sprintf(csv + csv_size, "%.*s;", CALIB_NAME_LEN_MAX, info->sensor_name[i]);
    }
    csv_size += sprintf(csv + csv_size, "\n");

    // line: sensor units
    csv_size += sprintf(csv + csv_size, ";;;");
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        csv_size += sprintf(csv + csv_size, "%.*s;", SENSOR_UNIT_LEN_MAX, info->sensor_unit[i]);
    }
    csv_size += sprintf(csv + csv_size, "\n");

    // line: peak values
    csv_size += sprintf(csv + csv_size, "PEAK VALUES;;;");
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        csv_size += sprintf(csv + csv_size, "%.*lf;", info->sensor_num_decimals[i], table->peak_values[i]);
    }

    csv_size += sprintf(csv + csv_size, "\n");
    // lines: table
    test_row_t *row = table->row;
    for (int i = 0; i < (table->row_num); i++)
    {
        csv_size += sprintf(csv + csv_size, ";");
        csv_size += sprintf(csv + csv_size, "%04i;", row->index);
        csv_size += sprintf(csv + csv_size, "%02u:%02u:%02.2f;", row->time.h, row->time.m, row->time.s); /**/
        double *data = row->data;
        int *num_decimals = info->sensor_num_decimals;
        for (int j = 0; j < NUM_SENSORS; j++)
        {
            csv_size += sprintf(csv + csv_size, "%.*lf;", *num_decimals, *data);
            data++;
            num_decimals++;
        }
        csv_size += sprintf(csv + csv_size, "\n");
        row++;

        if (csv_size > (sizeof(csv) - 100))
        {
            ESP_LOGW(TAG, "%s, line:%d", __FILE__, __LINE__);
            break;
        }
    }

    ESP_LOGI(TAG, "CSV SIZE: %i", csv_size);
    ESP_LOGI(TAG, "CSV:\n%s", csv);
}

static char *test_get_status_str(int slot)
{
    assert((slot >= 0) && (slot < CURRENT_TEST_NUM_MAX));

    test_status_t status = _test_status[slot];

    switch (_test_status[slot])
    {
    case TEST_VOID:
        return "ERROR 1";
    case TEST_INITIALIZED:
        return "ERROR 2";
    case TEST_CONFIGURED:
        return "NO TRIGGER";
    case TEST_RUNNING:
        return "RUNNING";
    case TEST_DONE:
        return "DONE";
    default:
        return "ERROR 3";
    }
}
static void test_lock_sensors(int slot)
{
    assert(_test_status[slot] == TEST_CONFIGURED);
    int input_enable = _test_setup[slot].inputs.input_enable.byte;
    for (int i = 0; i <= NUM_SENSORS; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            sensor_set_as_busy(i);
            nextion_1_home_write_status(i, sensor_get_status_str(i));
            nextion_1_home_write_data(i, "- - -");
            nextion_1_home_write_dataps(i, "- - -");
        }
        input_enable >>= 1;
    }
}
static void test_unlock_sensors(int slot)
{
    assert(_test_status[slot] == TEST_DONE);

    int input_enable = _test_setup[slot].inputs.input_enable.byte;
    for (int i = 0; i <= NUM_SENSORS; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            sensor_set_as_free(i);
            nextion_1_home_write_status(i, sensor_get_status_str(i));
        }
        input_enable >>= 1;
    }
}
static char *test_get_elapsed_time(int64_t timestamp)
{
    int64_t elapsed_time = time_get_timestamp() - timestamp;
    int hours = elapsed_time / 3600;
    int minutes = (elapsed_time % 3600) / 60;
    int seconds = elapsed_time % 60;
    snprintf(buffer, sizeof(buffer), "%02i:%02i:%02i", hours, minutes, seconds);
    return buffer;
}
void test_define_aditional_info(int slot)
{
    assert(_test_status[slot] == TEST_CONFIGURED);

    // global vars
    test_setup_t *test_setup = _test_setup + slot;
    test_aditional_info_t *aditional_info = _test_aditional_info + slot;

    // test inputs
    test_inputs_t *test_inputs = &(test_setup->inputs);

    // test graph
    test_graph_t *test_graph = &(test_setup->graph);
    axis_option_t x_axis_option = test_graph->x_axis_option;
    axis_option_t y_axis_option = test_graph->y_axis_option;

    // title
    snprintf(aditional_info->title, sizeof(aditional_info->title), "%s - %s",
             test_setup->test_name, time_get_datetime_formated(true, (int64_t)_test_setup_timestamp[slot]));

    // sensor name
    // sensor unit
    {
        int input_enable = test_inputs->input_enable.byte;
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            bool input_enabled = (input_enable & 1) == 1;
            snprintf(aditional_info->sensor_name[i], sizeof(aditional_info->sensor_name[i]), "%s",
                     input_enabled ? sensor_get_name(i) : CURRENT_TEST_SENSOR_NAME_DEFAULT);
            snprintf(aditional_info->sensor_unit[i], sizeof(aditional_info->sensor_unit[i]), "%s",
                     input_enabled ? sensor_get_unit_str(i) : CURRENT_TEST_UNIT_DEFAULT);
            snprintf(aditional_info->sensor_unitps[i], sizeof(aditional_info->sensor_unitps[i]), "%s",
                     input_enabled ? sensor_get_unit_str(i) : CURRENT_TEST_UNITPS_DEFAULT);
            aditional_info->sensor_num_decimals[i] = input_enabled ? sensor_get_num_decimals(i) : 0;
            input_enable >>= 1;
        }
    }

    // y axis name
    // y1_val to y8_val
    bool is_sensor = ((y_axis_option >= AXIS_SENSOR_INDEX_0) && (y_axis_option < AXIS_MAX));
    if (is_sensor)
    {
        int sensor_index = y_axis_option;

        snprintf(aditional_info->Y_axis_name, sizeof(aditional_info->Y_axis_name), "%s", sensor_get_name(sensor_index));
        snprintf(aditional_info->Y_axis_unit, sizeof(aditional_info->Y_axis_unit), "%s", sensor_get_unit_str(sensor_index));

        double capacity = sensor_get_capacity(sensor_index);
        int num_decimals = sensor_get_num_decimals(sensor_index);

        snprintf(aditional_info->Y1_val, sizeof(aditional_info->Y1_val), "%.*lf", num_decimals, 1.0 * capacity / 8.0);
        snprintf(aditional_info->Y2_val, sizeof(aditional_info->Y2_val), "%.*lf", num_decimals, 2.0 * capacity / 8.0);
        snprintf(aditional_info->Y3_val, sizeof(aditional_info->Y3_val), "%.*lf", num_decimals, 3.0 * capacity / 8.0);
        snprintf(aditional_info->Y4_val, sizeof(aditional_info->Y4_val), "%.*lf", num_decimals, 4.0 * capacity / 8.0);
        snprintf(aditional_info->Y5_val, sizeof(aditional_info->Y5_val), "%.*lf", num_decimals, 5.0 * capacity / 8.0);
        snprintf(aditional_info->Y6_val, sizeof(aditional_info->Y6_val), "%.*lf", num_decimals, 6.0 * capacity / 8.0);
        snprintf(aditional_info->Y7_val, sizeof(aditional_info->Y7_val), "%.*lf", num_decimals, 7.0 * capacity / 8.0);
        snprintf(aditional_info->Y8_val, sizeof(aditional_info->Y8_val), "%.*lf", num_decimals, 8.0 * capacity / 8.0);
    }
}

int test_get_tara_enable()
{
    return _test_setup[_initialized_slot].inputs.tara_enable.byte;
}
void test_print_elapsed_time()
{
    if (_foreground_slot == CURRENT_TEST_NUM_MAX)
        return;

    char *elapsed_time = test_get_elapsed_time((int64_t)_test_setup_timestamp[_foreground_slot]);
    nextion_1_current_test_p1_write_time(elapsed_time);
    nextion_1_current_test_p2_write_time(elapsed_time);
    nextion_1_current_test_p3_write_time(elapsed_time);
}

/*
███████╗███████╗████████╗██╗   ██╗██████╗
██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
███████╗█████╗     ██║   ██║   ██║██████╔╝
╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
███████║███████╗   ██║   ╚██████╔╝██║
╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝

*/
esp_err_t test_set_type(test_type_t *type)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    _test_setup[_initialized_slot].type = *type;
    test_print_type();

    return ESP_OK;
}
esp_err_t test_set_inputs(test_inputs_t *inputs)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    memcpy(&(_test_setup[_initialized_slot].inputs), inputs, sizeof(test_inputs_t));
    test_print_inputs();

    return ESP_OK;
}
esp_err_t test_set_logging(test_logging_t *logging)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    memcpy(&(_test_setup[_initialized_slot].logging), logging, sizeof(test_logging_t));
    test_print_logging();

    return ESP_OK;
}
esp_err_t test_set_stop(test_stop_t *stop)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    memcpy(&(_test_setup[_initialized_slot].stop), stop, sizeof(test_stop_t));
    test_print_stop();

    return ESP_OK;
}
esp_err_t test_set_motor(test_motor_t *motor)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    memcpy(&(_test_setup[_initialized_slot].motor), motor, sizeof(test_motor_t));
    test_print_motor();

    return ESP_OK;
}
esp_err_t test_set_graph(test_graph_t *graph)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    memcpy(&(_test_setup[_initialized_slot].graph), graph, sizeof(test_graph_t));
    test_print_graph();

    return ESP_OK;
}
esp_err_t test_set_start(test_start_t *start)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    memcpy(&(_test_setup[_initialized_slot].start), start, sizeof(test_start_t));
    test_print_start();

    return ESP_OK;
}
void test_print_type(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    switch (_test_setup[_initialized_slot].type)
    {
    case TEST_TYPE_CBR:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_CBR");
        break;
    case TEST_TYPE_LBR:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_LBR");
        break;
    case TEST_TYPE_MARSHALL:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_MARSHALL");
        break;
    case TEST_TYPE_TSR:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_TSR");
        break;
    case TEST_TYPE_SCB_ASHTO:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_SCB_ASHTO");
        break;
    case TEST_TYPE_SCB_ASTM:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_SCB_ASTM");
        break;
    case TEST_TYPE_USER_DEFINED:
        ESP_LOGI(TAG, "  TYPE: %s", "TEST_TYPE_USER_DEFINED");
        break;
    default:
        ESP_LOGI(TAG, "  TYPE: %d", _test_setup[_initialized_slot].type);
        break;
    }
}
void test_print_inputs(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    ESP_LOGI(TAG, "  INPUTS: [1 YES, 0 NO]");
    ESP_LOGI(TAG, "     0- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s0, _test_setup[_initialized_slot].inputs.tara_enable.s0);
    ESP_LOGI(TAG, "     1- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s1, _test_setup[_initialized_slot].inputs.tara_enable.s1);
    ESP_LOGI(TAG, "     2- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s2, _test_setup[_initialized_slot].inputs.tara_enable.s2);
    ESP_LOGI(TAG, "     3- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s3, _test_setup[_initialized_slot].inputs.tara_enable.s3);
    ESP_LOGI(TAG, "     4- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s4, _test_setup[_initialized_slot].inputs.tara_enable.s4);
    ESP_LOGI(TAG, "     5- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s5, _test_setup[_initialized_slot].inputs.tara_enable.s5);
    ESP_LOGI(TAG, "     6- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s6, _test_setup[_initialized_slot].inputs.tara_enable.s6);
    ESP_LOGI(TAG, "     7- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot].inputs.input_enable.s7, _test_setup[_initialized_slot].inputs.tara_enable.s7);
}
void test_print_logging(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    ESP_LOGI(TAG, "  LOGGING:");
    switch (_test_setup[_initialized_slot].logging.condition)
    {
    case LOGGING_CONDITION_INTERVAL_LOGGING:
        ESP_LOGI(TAG, "  condition: INTERVAL_LOGGING");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].logging.parameters.interval_logging.sensor_index);
        ESP_LOGI(TAG, "  direction: %d [0 UP, 1 DOWN]", _test_setup[_initialized_slot].logging.parameters.interval_logging.direction);
        ESP_LOGI(TAG, "  value: %lf (unit)", _test_setup[_initialized_slot].logging.parameters.interval_logging.value);
        break;
    case LOGGING_CONDITION_LINEAR_TIME_INTERVAL:
        ESP_LOGI(TAG, "  condition: LINEAR_TIME_INTERVAL");
        ESP_LOGI(TAG, "  value: %lu (s)", _test_setup[_initialized_slot].logging.parameters.linear_time_interval.value);
        break;
    case LOGGING_CONDITION_INTERVAL_LOGGING_TABLE:
        ESP_LOGI(TAG, "  condition: INTERVAL_LOGGING_TABLE");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].logging.parameters.interval_logging_table.sensor_index);
        ESP_LOGI(TAG, "  direction: %d [0 UP, 1 DOWN]", _test_setup[_initialized_slot].logging.parameters.interval_logging_table.direction);
        ESP_LOGI(TAG, "  value:");
        for (int i = 0; i < 30; i++)
        {
            ESP_LOGI(TAG, "     [%i]=%lf (unit)", i, _test_setup[_initialized_slot].logging.parameters.interval_logging_table.value[i]);
        }
        break;
    case LOGGING_CONDITION_ELAPSED_TIME_TABLE:
        ESP_LOGI(TAG, "  condition: ELAPSED_TIME_TABLE");
        ESP_LOGI(TAG, "  value:");
        for (int i = 0; i < 30; i++)
        {
            test_time_t *value = &(_test_setup[_initialized_slot].logging.parameters.elapsed_time_table.value[i]);
            ESP_LOGI(TAG, "     [%i]=> h=%d, m=%d, s=%.2f", i, value->h, value->m, value->s);
        }
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup[_initialized_slot].logging.condition);
        break;
    }
}
void test_print_stop(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    ESP_LOGI(TAG, "  STOP:");
    switch (_test_setup[_initialized_slot].stop.condition)
    {
    case STOP_CONDITION_GREATER_THAN:
        ESP_LOGI(TAG, "  condition: GREATER_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.greater_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.greater_than.value);
        break;
    case STOP_CONDITION_LESS_THAN:
        ESP_LOGI(TAG, "  condition: LESS_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.less_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.less_than.value);
        break;
    case STOP_CONDITION_TIME_DELAY:
        ESP_LOGI(TAG, "  condition: TIME_DELAY");
        ESP_LOGI(TAG, "  value: %lu", _test_setup[_initialized_slot].stop.parameters.time_delay.value);
        break;
    case STOP_CONDITION_DISTANCE_UP:
        ESP_LOGI(TAG, "  condition: DISTANCE_UP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.distance_up.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.distance_up.value);
        break;
    case STOP_CONDITION_DISTANCE_DOWN:
        ESP_LOGI(TAG, "  condition: DISTANCE_DOWN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.distance_down.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.distance_down.value);
        break;
    case STOP_CONDITION_PERCENT_DROP:
        ESP_LOGI(TAG, "  condition: PERCENT_DROP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.percent_drop.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.percent_drop.value);
        break;
    case STOP_CONDITION_PERCENT_STRAIN:
        ESP_LOGI(TAG, "  condition: PERCENT_STRAIN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.percent_strain.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.percent_strain.value);
        break;
    case STOP_CONDITION_PERCENT_STRESS_DROP:
        ESP_LOGI(TAG, "  condition: PERCENT_STRESS_DROP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].stop.parameters.percent_stress_drop.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].stop.parameters.percent_stress_drop.value);
        break;
    case STOP_CONDITION_OPERATOR_STOP:
        ESP_LOGI(TAG, "  condition: OPERATOR_STOP");
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup[_initialized_slot].stop.condition);
        break;
    }
}
void test_print_motor(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);
}
void test_print_graph(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    switch (_test_setup[_initialized_slot].graph.x_axis_option)
    {
    case AXIS_NONE:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_NONE");
        break;
    case AXIS_TIME:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_TIME");
        break;
    case AXIS_SQR_ROOT_TIME:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SQR_ROOT_TIME");
        break;
    case AXIS_PERCENT_STRAIN:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_PERCENT_STRAIN");
        break;
    case AXIS_SENSOR_INDEX_0:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_0");
        break;
    case AXIS_SENSOR_INDEX_1:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_1");
        break;
    case AXIS_SENSOR_INDEX_2:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_2");
        break;
    case AXIS_SENSOR_INDEX_3:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_3");
        break;
    case AXIS_SENSOR_INDEX_4:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_4");
        break;
    case AXIS_SENSOR_INDEX_5:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_5");
        break;
    case AXIS_SENSOR_INDEX_6:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_6");
        break;
    case AXIS_SENSOR_INDEX_7:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS_SENSOR_INDEX_7");
        break;
    default:
        ESP_LOGI(TAG, "  GRAPH: X_AXIS %d", _test_setup[_initialized_slot].graph.x_axis_option);
        break;
    }

    switch (_test_setup[_initialized_slot].graph.y_axis_option)
    {
    case AXIS_NONE:
        ESP_LOGI(TAG, "  GRAPH : Y_AXIS_NONE");
        break;
    case AXIS_TIME:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_TIME");
        break;
    case AXIS_SQR_ROOT_TIME:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SQR_ROOT_TIME");
        break;
    case AXIS_PERCENT_STRAIN:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_PERCENT_STRAIN");
        break;
    case AXIS_SENSOR_INDEX_0:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_0");
        break;
    case AXIS_SENSOR_INDEX_1:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_1");
        break;
    case AXIS_SENSOR_INDEX_2:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_2");
        break;
    case AXIS_SENSOR_INDEX_3:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_3");
        break;
    case AXIS_SENSOR_INDEX_4:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_4");
        break;
    case AXIS_SENSOR_INDEX_5:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_5");
        break;
    case AXIS_SENSOR_INDEX_6:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_6");
        break;
    case AXIS_SENSOR_INDEX_7:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS_SENSOR_INDEX_7");
        break;
    default:
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS %d", _test_setup[_initialized_slot].graph.y_axis_option);
        break;
    }
}
void test_print_start(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    ESP_LOGI(TAG, "  START:");
    switch (_test_setup[_initialized_slot].start.condition)
    {
    case START_CONDITION_TRIGGER_IMMEDIATELY:
        ESP_LOGI(TAG, "  condition: TRIGGER_IMMEDIATELY");
        break;
    case START_CONDITION_TIME_DELAY:
        ESP_LOGI(TAG, "  condition: TIME_DELAY");
        ESP_LOGI(TAG, "  value: %lu", _test_setup[_initialized_slot].start.parameters.time_delay.value);
        break;
    case START_CONDITION_GREATER_THAN:
        ESP_LOGI(TAG, "  condition: GREATER_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].start.parameters.greater_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].start.parameters.greater_than.value);
        ESP_LOGI(TAG, "  current_value: %lf", _test_setup[_initialized_slot].start.parameters.greater_than.current_value);
        ESP_LOGI(TAG, "  first_point: %d [0 AT TRIGGER, 1 AT INITIAL VALUE]", _test_setup[_initialized_slot].start.parameters.greater_than.first_point);
        break;
    case START_CONDITION_LESS_THAN:
        ESP_LOGI(TAG, "  condition: LESS_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot].start.parameters.less_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot].start.parameters.less_than.value);
        ESP_LOGI(TAG, "  current_value: %lf", _test_setup[_initialized_slot].start.parameters.less_than.current_value);
        ESP_LOGI(TAG, "  first_point: %d [0 AT TRIGGER, 1 AT INITIAL VALUE]", _test_setup[_initialized_slot].start.parameters.less_than.first_point);
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup[_initialized_slot].logging.condition);
        break;
    }
}
esp_err_t test_input_get_sensor_num_from_enablers(int *sensor_num, uint8_t enablers)
{
    int counter = 0;
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        if ((enablers & 1) == 1)
            counter++;
        enablers >>= 1;
    }
    *sensor_num = counter;
    return ESP_OK;
}
esp_err_t test_get_sensor_index_from_combobox(int combobox_val, uint8_t *sensor_index)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    uint16_t input_enable = _test_setup[_initialized_slot].inputs.input_enable.byte;
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {

        if ((input_enable & 1) == 1)
        {
            combobox_val--;
            if (combobox_val < 0)
            {
                *sensor_index = i;
                return ESP_OK;
            }
        }

        input_enable >>= 1;
    }
    ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
    return ESP_FAIL;
}

esp_err_t test_get_logging_direction_from_combobox(int combobox_val, logging_direction_t *direction)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    if (combobox_val == 0)
    {
        *direction = LOGGING_DIRECTION_UP;
        return ESP_OK;
    }
    else if (combobox_val == 1)
    {
        *direction = LOGGING_DIRECTION_DOWN;
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
}
esp_err_t test_get_graph_option_from_combobox(int combobox_val, axis_option_t *axis_option)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    if ((combobox_val >= 0) && (combobox_val <= 3))
    {
        *axis_option = (axis_option_t)(combobox_val - 4);
        return ESP_OK;
    }
    else if ((combobox_val >= 4) && (combobox_val <= 11))
    {
        combobox_val -= 4;
        uint16_t input_enable = _test_setup[_initialized_slot].inputs.input_enable.byte;
        for (uint8_t i = 0; i < NUM_SENSORS; i++)
        {

            if ((input_enable & 1) == 1)
            {
                combobox_val--;
                if (combobox_val < 0)
                {
                    *axis_option = (axis_option_t)i;
                    return ESP_OK;
                }
            }

            input_enable >>= 1;
        }
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }
}
esp_err_t test_get_start_first_point_from_multiradio(int multiradio_val, first_point_t *first_point)
{
    if (multiradio_val == 0)
    {
        *first_point = TAKEN_AT_TRIGGER;
        return ESP_OK;
    }
    else if (multiradio_val == 1)
    {
        *first_point = TAKEN_AT_INITIAL_VALUE;
        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
}
esp_err_t test_start_GreaterThan_set_sensor_index(int sensor_index)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    ESP_LOGI(TAG, "Greater than, set sensor_index: %u", sensor_index);
    _test_setup[_initialized_slot].start.parameters.greater_than.sensor_index = sensor_index;
    return ESP_OK;
}
esp_err_t test_start_GreaterThan_get_sensor_index(int *sensor_index)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    *sensor_index = _test_setup[_initialized_slot].start.parameters.greater_than.sensor_index;
    return ESP_OK;
}
esp_err_t test_start_LessThan_set_sensor_index(int sensor_index)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);

    ESP_LOGI(TAG, "Less than, set sensor_index: %u", sensor_index);
    _test_setup[_initialized_slot].start.parameters.less_than.sensor_index = sensor_index;
    return ESP_OK;
}
esp_err_t test_start_LessThan_get_sensor_index(int *sensor_index)
{
    // if (_test_setup == NULL)
    //    return ESP_ERR_NO_MEM;

    *sensor_index = _test_setup[_initialized_slot].start.parameters.less_than.sensor_index;
    return ESP_OK;
}
esp_err_t test_get_type_from_multiradio(int multiradio_val, test_type_t *test_type)
{
    if ((multiradio_val >= TEST_TYPE_CBR) && (multiradio_val <= TEST_TYPE_SCB_ASTM))
    {
        *test_type = (test_type_t)multiradio_val;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t test_set_logging_page(page_t page)
{
    _loggging_page = page;
    return ESP_OK;
}
esp_err_t test_set_stop_page(page_t page)
{
    _stop_page = page;
    return ESP_OK;
}
esp_err_t test_set_start_page(page_t page)
{
    _start_page = page;
    return ESP_OK;
}
esp_err_t test_get_logging_page(page_t *page)
{
    *page = _loggging_page;
    return ESP_OK;
}
esp_err_t test_get_stop_page(page_t *page)
{
    *page = _stop_page;
    return ESP_OK;
}
esp_err_t test_get_start_page(page_t *page)
{
    *page = _start_page;
    return ESP_OK;
}

/*
████████╗ █████╗ ██████╗ ██╗     ███████╗
╚══██╔══╝██╔══██╗██╔══██╗██║     ██╔════╝
   ██║   ███████║██████╔╝██║     █████╗
   ██║   ██╔══██║██╔══██╗██║     ██╔══╝
   ██║   ██║  ██║██████╔╝███████╗███████╗
   ╚═╝   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝

*/
esp_err_t test_table_add_row(int slot)
{
    assert((slot >= 0) && (slot < CURRENT_TEST_NUM_MAX));
    assert(_test_status[slot] >= TEST_TRIGGERING);

    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_row_t *first_row = test_table->row;
    int row_num = test_table->row_num;
    int sensor_num = test_setup->inputs.sensor_num;
    double *peak_values = test_table->peak_values;
    uint8_t input_enable = test_setup->inputs.input_enable.byte;
    uint8_t tara_enable = test_setup->inputs.tara_enable.byte;

    // GET ROW TO FILL
    test_row_t *row = (first_row) + row_num;

    // SET NEW-ROW-INDEX
    row->index = row_num;

    // SET NEW-ROW-TIME
    double elapsed_ftime = time_get_ftimestamp() - _test_setup_timestamp[slot];
    int64_t elapsed_time = (int64_t)elapsed_ftime;
    double ms = elapsed_ftime - elapsed_time;
    row->time.h = elapsed_time / 3600;
    row->time.m = (elapsed_time % 3600) / 60;
    row->time.s = (float)(elapsed_time % 60) + (float)ms;

    // SET NEW-ROW-DATA AND TABLE-PEAK-VALUES
    double *data = row->data;
    // set data and update peak values if sensor was choosen for testing
    for (int sensor_index = 0; sensor_index < NUM_SENSORS; sensor_index++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        bool tara_enabled = (tara_enable & 1) == 1;

        double real = (input_enabled ? sensor_get_real(sensor_index, tara_enabled) : 0.0);
        *data = real;
        if (real > (*peak_values))
            *peak_values = real;

        peak_values++;
        data++;
        input_enable >>= 1;
        tara_enable >>= 1;
    }

    // INCREASE ROW NUM
    test_table->row_num = row_num + 1;

    return ESP_OK;
}
/*
██████╗ ██╗      ██████╗ ████████╗████████╗██╗███╗   ██╗ ██████╗
██╔══██╗██║     ██╔═══██╗╚══██╔══╝╚══██╔══╝██║████╗  ██║██╔════╝
██████╔╝██║     ██║   ██║   ██║      ██║   ██║██╔██╗ ██║██║  ███╗
██╔═══╝ ██║     ██║   ██║   ██║      ██║   ██║██║╚██╗██║██║   ██║
██║     ███████╗╚██████╔╝   ██║      ██║   ██║██║ ╚████║╚██████╔╝
╚═╝     ╚══════╝ ╚═════╝    ╚═╝      ╚═╝   ╚═╝╚═╝  ╚═══╝ ╚═════╝

*/
void current_test_plot_status(int slot)
{
    assert((slot >= 0) && (slot < CURRENT_TEST_NUM_MAX));

    nextion_1_current_test_p1_write_status(test_get_status_str(slot));
    nextion_1_current_test_p2_write_status(test_get_status_str(slot));
    nextion_1_current_test_p3_write_status(test_get_status_str(slot));
}
void current_test_plot_points(int slot)
{
    assert((slot >= 0) && (slot < CURRENT_TEST_NUM_MAX));

    test_table_t *test_table = &(_test_table[slot]);
    nextion_1_current_test_p1_write_points(test_table->row_num);
    nextion_1_current_test_p2_write_points(test_table->row_num);
    nextion_1_current_test_p3_write_points(test_table->row_num);
}
void current_test_plot_last_data_p1(void)
{
    if (_foreground_slot == CURRENT_TEST_NUM_MAX)
        return;

    // test vars
    test_inputs_t *test_inputs = &(_test_setup[_foreground_slot].inputs);
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;

    for (int sensor_index = 0; sensor_index < NUM_SENSORS; sensor_index++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        bool tara_enabled = (tara_enable & 1) == 1;
        if (input_enabled)
        {
            char *real_str = sensor_get_real_str(sensor_index, tara_enabled);
            nextion_1_current_test_p1_write_data(sensor_index, real_str);
            char *realps_str = sensor_get_realps_str(sensor_index);
            nextion_1_current_test_p1_write_dataps(sensor_index, realps_str);
        }

        input_enable >>= 1;
        tara_enable >>= 1;
    }
}
void current_test_plot_last_data_p2_p3(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = &(_test_setup[slot]);
    test_table_t *test_table = &(_test_table[slot]);
    test_start_t *test_start = &(test_setup->start);
    test_graph_t *test_graph = &(test_setup->graph);

    // test vars
    uint8_t input_enable = test_setup->inputs.input_enable.byte;
    uint8_t tara_enable = test_setup->inputs.tara_enable.byte;
    axis_option_t x_axis_option = test_graph->x_axis_option;
    axis_option_t y_axis_option = test_graph->y_axis_option;
    int row_num = test_table->row_num;
    assert(row_num >= 1);
    test_row_t *last_row = (test_table->row) + row_num - 1;
    double *data = last_row->data;

    // PRINT INDEX AND TIME
    {
        int row_index = last_row->index;
        test_time_t time = last_row->time;

        snprintf(buffer, sizeof(buffer), "%i", row_index);
        nextion_1_current_test_p2_append_index(buffer);

        snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.2f", time.h, time.m, time.s);
        nextion_1_current_test_p2_append_time(buffer);
    }

    // PRINT ON TABLE AND GRAPH
    for (int sensor_index = 0; sensor_index < NUM_SENSORS; sensor_index++)
    {
        // GLOBAL
        bool input_enabled = (input_enable & 1) == 1;
        bool tara_enabled = (tara_enable & 1) == 1;

        // TABLE
        if (input_enabled)
            snprintf(buffer, sizeof(buffer), "%.*lf", sensor_get_num_decimals(sensor_index), data[sensor_index]);
        else
            snprintf(buffer, sizeof(buffer), "%s", "");

        nextion_1_current_test_p2_append_sensor_data(sensor_index, buffer);

        // GRAPH
        bool sensor_selected_for_graph = (y_axis_option == (uint32_t)sensor_index);
        if (sensor_selected_for_graph)
        {
            double real = data[sensor_index];
            double capacity = sensor_get_capacity(sensor_index);
            int value = 100.0 * real / capacity;
            value = (value > 100) ? 100 : value;
            value = (value < 0) ? 0 : value;
            nextion_1_current_test_p3_append_data(value);
        }

        input_enable >>= 1;
        tara_enable >>= 1;
    }
}
void current_test_load_foreground_slot(void)
{
    assert(_foreground_slot != CURRENT_TEST_NUM_MAX);
    assert(_test_status[_foreground_slot] >= TEST_TRIGGERING);

    // GLOBAL VARS
    test_table_t *test_table = _test_table + _foreground_slot;
    test_setup_t *test_setup = _test_setup + _foreground_slot;
    test_aditional_info_t *aditional_info = _test_aditional_info + _foreground_slot;
    double test_setup_timestamp = *(_test_setup_timestamp + _foreground_slot);

    // test vars
    int input_enable = test_setup->inputs.input_enable.byte;
    axis_option_t x_axis_option = test_setup->graph.x_axis_option;
    axis_option_t y_axis_option = test_setup->graph.y_axis_option;

    // set title on p1 p2 p3
    nextion_1_current_test_p1_write_title(aditional_info->title);
    nextion_1_current_test_p2_write_title(aditional_info->title);
    nextion_1_current_test_p3_write_title(aditional_info->title);

    // set status on p1 p2 p3
    nextion_1_current_test_p1_write_status(test_get_status_str(_foreground_slot));
    nextion_1_current_test_p2_write_status(test_get_status_str(_foreground_slot));
    nextion_1_current_test_p3_write_status(test_get_status_str(_foreground_slot));

    // set points on p1 p2 p3
    nextion_1_current_test_p1_write_points(test_table->row_num);
    nextion_1_current_test_p2_write_points(test_table->row_num);
    nextion_1_current_test_p3_write_points(test_table->row_num);

    // set time on p1 p2 p3
    char *elapsed_time = test_get_elapsed_time((int64_t)test_setup_timestamp);
    nextion_1_current_test_p1_write_time(elapsed_time);
    nextion_1_current_test_p2_write_time(elapsed_time);
    nextion_1_current_test_p3_write_time(elapsed_time);

    // set Y axis info on p3
    nextion_1_current_test_p3_write_y_axis_name(aditional_info->Y_axis_name);
    nextion_1_current_test_p3_write_y_axis_unit(aditional_info->Y_axis_unit);

    nextion_1_current_test_p3_write_Y1(aditional_info->Y1_val);
    nextion_1_current_test_p3_write_Y2(aditional_info->Y2_val);
    nextion_1_current_test_p3_write_Y3(aditional_info->Y3_val);
    nextion_1_current_test_p3_write_Y4(aditional_info->Y4_val);
    nextion_1_current_test_p3_write_Y5(aditional_info->Y5_val);
    nextion_1_current_test_p3_write_Y6(aditional_info->Y6_val);
    nextion_1_current_test_p3_write_Y7(aditional_info->Y7_val);
    nextion_1_current_test_p3_write_Y8(aditional_info->Y8_val);

    // set sensors info on p1 p2
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        // p1
        nextion_1_current_test_p1_write_sensor_name(i, aditional_info->sensor_name[i]);
        nextion_1_current_test_p1_write_data(i, CURRENT_TEST_DATA_DEFAULT);
        nextion_1_current_test_p1_write_unit(i, aditional_info->sensor_unit[i]);
        nextion_1_current_test_p1_write_dataps(i, CURRENT_TEST_DATAPS_DEFAULT);
        nextion_1_current_test_p1_write_unitps(i, aditional_info->sensor_unitps[i]);

        // p2
        nextion_1_current_test_p2_write_sensor_name(i, aditional_info->sensor_name[i]);
        nextion_1_current_test_p2_write_unit(i, aditional_info->sensor_unit[i]);
        nextion_1_current_test_p2_clean_sensor_data(i);
    }

    // load sensor data on p2
    {
        nextion_1_current_test_p2_clean_index();
        nextion_1_current_test_p2_clean_time();

        test_row_t *row = test_table->row;
        int row_num = test_table->row_num;
        for (int i = 0; i < row_num; i++)
        {
            // index
            snprintf(buffer, sizeof(buffer), "%i", row->index);
            nextion_1_current_test_p2_append_index(buffer);

            // time
            snprintf(buffer, sizeof(buffer), "%2i:%2i:%02.2f", row->time.h, row->time.m, row->time.s);
            nextion_1_current_test_p2_append_time(buffer);

            // sensor
            int enablers = input_enable;
            double *data = row->data;
            for (int sensor_index = 0; sensor_index < NUM_SENSORS; sensor_index++)
            {
                bool input_enabled = (enablers & 1) == 1;

                if (input_enabled)
                    snprintf(buffer, sizeof(buffer), "%.*lf", sensor_get_num_decimals(sensor_index), *data);
                else
                    snprintf(buffer, sizeof(buffer), "%s", "");

                nextion_1_current_test_p2_append_sensor_data(sensor_index, buffer);

                data++;
                enablers >>= 1;
            }

            // next row
            row++;
        }
    }

    // load sensor data on p3
    {
        nextion_1_current_test_p3_clean_sensor_data();

        bool sensor_selected_for_graph = ((y_axis_option >= AXIS_SENSOR_INDEX_0) && (y_axis_option < AXIS_MAX));
        if (!sensor_selected_for_graph)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return;
        }

        int sensor_index = y_axis_option;
        int row_num = test_table->row_num;
        int excedent_row_num = (row_num > 60) ? row_num - 60 : 0;
        test_row_t *first_row = test_table->row + excedent_row_num;

        for (int i = 0; i < (row_num - excedent_row_num); i++)
        {
            double real = (first_row->data)[sensor_index];
            double capacity = sensor_get_capacity(sensor_index);
            int value = 100.0 * real / capacity;
            value = (value > 100) ? 100 : value;
            value = (value < 0) ? 0 : value;
            nextion_1_current_test_p3_write_slope(i, value);

            // next row
            first_row++;
        }
    }
}
void current_test_reset_default_p1_p2_p3()
{
}

/*
███████╗████████╗ █████╗ ██████╗ ████████╗     ██████╗ ██████╗ ███╗   ██╗██████╗ ██╗████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝    ██╔════╝██╔═══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██║██╔═══██╗████╗  ██║
███████╗   ██║   ███████║██████╔╝   ██║       ██║     ██║   ██║██╔██╗ ██║██║  ██║██║   ██║   ██║██║   ██║██╔██╗ ██║
╚════██║   ██║   ██╔══██║██╔══██╗   ██║       ██║     ██║   ██║██║╚██╗██║██║  ██║██║   ██║   ██║██║   ██║██║╚██╗██║
███████║   ██║   ██║  ██║██║  ██║   ██║       ╚██████╗╚██████╔╝██║ ╚████║██████╔╝██║   ██║   ██║╚██████╔╝██║ ╚████║
╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝        ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

esp_err_t test_start_condition_trigger_immediately(int slot)
{
    // EXECUTE CONDITION
    _test_status[slot] = TEST_RUNNING;
    _test_start_timestamp[slot] = time_get_timestamp();

    // add first row
    test_table_add_row(slot);

    if (_foreground_slot == slot)
    {
        current_test_plot_points(slot);
        current_test_plot_status(slot);
        current_test_plot_last_data_p2_p3(slot);
    }

    return ESP_OK;
}
esp_err_t test_start_condition_time_delay(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_start_t *test_start = &(test_setup->start);

    // EXECUTE CONDITION
    uint32_t value = test_start->parameters.time_delay.value;
    if (time_get_ftimestamp() - _test_setup_timestamp[slot] > (value * 1.0))
    {
        _test_status[slot] = TEST_RUNNING;
        _test_start_timestamp[slot] = time_get_timestamp();

        // add first row
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
            current_test_plot_status(slot);
        }
    }

    return ESP_OK;
}
esp_err_t test_start_condition_greater_than(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_start_t *test_start = &(test_setup->start);
    test_inputs_t *test_input = &(test_setup->inputs);

    // EXECUTE CONDITION
    // start parameters
    int sensor_index = test_start->parameters.greater_than.sensor_index;
    double value = test_start->parameters.greater_than.value;
    double current_value = test_start->parameters.greater_than.current_value;
    first_point_t first_point = test_start->parameters.greater_than.first_point;
    int tara_enable = test_input->tara_enable.byte;

    // check for first point at initial value
    bool need_first_point = (first_point == TAKEN_AT_INITIAL_VALUE) && (test_table->row_num == 0);
    if (need_first_point == true)
    {
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
        }
    }

    // check for condition
    bool condition_done = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1) > value;
    if (condition_done)
    {
        _test_status[slot] = TEST_RUNNING;
        _test_start_timestamp[slot] = time_get_ftimestamp();

        // check for first point at trigger
        bool need_first_point = (first_point == TAKEN_AT_TRIGGER) && (test_table->row_num == 0);
        if (need_first_point == true)
        {
            test_table_add_row(slot);

            if (_foreground_slot == slot)
            {
                current_test_plot_points(slot);
                current_test_plot_last_data_p2_p3(slot);
            }
        }
        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }

    return ESP_OK;
}
esp_err_t test_start_condition_less_than(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_start_t *test_start = &(test_setup->start);
    test_inputs_t *test_input = &(test_setup->inputs);

    // EXECUTE CONDITION
    // start parameters
    int sensor_index = test_start->parameters.less_than.sensor_index;
    double value = test_start->parameters.less_than.value;
    double current_value = test_start->parameters.less_than.current_value;
    first_point_t first_point = test_start->parameters.less_than.first_point;
    int tara_enable = test_input->tara_enable.byte;

    // check for first point at initial value
    bool need_first_point = (first_point == TAKEN_AT_INITIAL_VALUE) && (test_table->row_num == 0);
    if (need_first_point == true)
    {
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
        }
    }

    // check for condition
    bool condition_done = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1) > value;
    if (condition_done)
    {
        _test_status[slot] = TEST_RUNNING;
        _test_start_timestamp[slot] = time_get_ftimestamp();

        // check for first point at trigger
        bool need_first_point = (first_point == TAKEN_AT_TRIGGER) && (test_table->row_num == 0);
        if (need_first_point == true)
        {
            test_table_add_row(slot);

            if (_foreground_slot == slot)
            {
                current_test_plot_points(slot);
                current_test_plot_last_data_p2_p3(slot);
            }
        }
        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }

    return ESP_OK;
}

/*
███████╗████████╗ ██████╗ ██████╗      ██████╗ ██████╗ ███╗   ██╗██████╗ ██╗████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗    ██╔════╝██╔═══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██║██╔═══██╗████╗  ██║
███████╗   ██║   ██║   ██║██████╔╝    ██║     ██║   ██║██╔██╗ ██║██║  ██║██║   ██║   ██║██║   ██║██╔██╗ ██║
╚════██║   ██║   ██║   ██║██╔═══╝     ██║     ██║   ██║██║╚██╗██║██║  ██║██║   ██║   ██║██║   ██║██║╚██╗██║
███████║   ██║   ╚██████╔╝██║         ╚██████╗╚██████╔╝██║ ╚████║██████╔╝██║   ██║   ██║╚██████╔╝██║ ╚████║
╚══════╝   ╚═╝    ╚═════╝ ╚═╝          ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/
esp_err_t test_stop_condition_greater_than(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_stop_t *test_stop = &(test_setup->stop);
    test_inputs_t *test_input = &(test_setup->inputs);

    // test vars
    int sensor_index = test_stop->parameters.greater_than.sensor_index;
    double value = test_stop->parameters.greater_than.value;
    int tara_enable = test_input->tara_enable.byte;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition
    bool condition_done = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1) > value;
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_less_than(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_stop_t *test_stop = &(test_setup->stop);
    test_inputs_t *test_input = &(test_setup->inputs);

    // test vars
    int sensor_index = test_stop->parameters.less_than.sensor_index;
    double value = test_stop->parameters.less_than.value;
    int tara_enable = test_input->tara_enable.byte;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition
    bool condition_done = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1) < value;
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_time_delay(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_stop_t *test_stop = &(test_setup->stop);

    // test vars
    uint32_t value = test_stop->parameters.time_delay.value;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition
    bool condition_done = (time_get_ftimestamp() - _test_start_timestamp[slot] > (value * 1.0));
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_distance_up(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.distance_up.sensor_index;
    double value = test_stop->parameters.distance_up.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition

    // verify condition
    double first_value = data[sensor_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = (latest_value - first_value > value);
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_distance_down(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.distance_down.sensor_index;
    double value = test_stop->parameters.distance_down.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition

    // verify condition
    double first_value = data[sensor_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = (first_value - latest_value > value);
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_percent_drop(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.percent_drop.sensor_index;
    double value = test_stop->parameters.percent_drop.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition

    // verify condition
    double first_value = data[sensor_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    double percent_drop = ((first_value - latest_value) / first_value) * 100.0;
    bool condition_done = (percent_drop > value);
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_percent_strain(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.percent_strain.sensor_index;
    double value = test_stop->parameters.percent_strain.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition

    // verify condition
    double first_value = data[sensor_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    double percent_strain = (fabs(latest_value - first_value) / first_value) * 100.0;
    bool condition_done = (percent_strain > value);
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_stop_condition_percent_stress_drop(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.percent_stress_drop.sensor_index;
    double value = test_stop->parameters.percent_stress_drop.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition

    // verify condition
    double first_value = data[sensor_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    double percent_stress_drop = ((first_value - latest_value) / first_value) * 100.0;
    bool condition_done = (percent_stress_drop > value);
    if (condition_done || (_operator_stop[slot] == true))
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}

esp_err_t test_stop_condition_operator_stop(int slot)
{
    // GLOBAL VARS
    test_table_t *test_table = _test_table + slot;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    if (_operator_stop[slot] == true)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}

/*
██╗      ██████╗  ██████╗  ██████╗ ██╗███╗   ██╗ ██████╗      ██████╗ ██████╗ ███╗   ██╗██████╗ ██╗████████╗██╗ ██████╗ ███╗   ██╗
██║     ██╔═══██╗██╔════╝ ██╔════╝ ██║████╗  ██║██╔════╝     ██╔════╝██╔═══██╗████╗  ██║██╔══██╗██║╚══██╔══╝██║██╔═══██╗████╗  ██║
██║     ██║   ██║██║  ███╗██║  ███╗██║██╔██╗ ██║██║  ███╗    ██║     ██║   ██║██╔██╗ ██║██║  ██║██║   ██║   ██║██║   ██║██╔██╗ ██║
██║     ██║   ██║██║   ██║██║   ██║██║██║╚██╗██║██║   ██║    ██║     ██║   ██║██║╚██╗██║██║  ██║██║   ██║   ██║██║   ██║██║╚██╗██║
███████╗╚██████╔╝╚██████╔╝╚██████╔╝██║██║ ╚████║╚██████╔╝    ╚██████╗╚██████╔╝██║ ╚████║██████╔╝██║   ██║   ██║╚██████╔╝██║ ╚████║
╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝      ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

esp_err_t test_logging_condition_interval_logging(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_logging_t *test_logging = &(test_setup->logging);
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    int sensor_index = test_logging->parameters.interval_logging.sensor_index;
    logging_direction_t direction = test_logging->parameters.interval_logging.direction;
    double value = test_logging->parameters.interval_logging.value;

    // execute condition
    // verify row num max
    if (row_num >= TEST_ROWS_NUM_MAX)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }

        return ESP_OK;
    }

    // verify condition
    double first_value = data[sensor_index];
    double next_step = first_value + (row_num * value) * ((direction == LOGGING_DIRECTION_UP) ? 1.0 : -1.0);
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = ((direction == LOGGING_DIRECTION_UP) ? (latest_value > next_step) : (latest_value < next_step));
    if (condition_done)
    {
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_logging_condition_linear_time_interval(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_logging_t *test_logging = &(test_setup->logging);
    uint32_t value = test_logging->parameters.linear_time_interval.value;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    // execute condition
    // verify row num max
    if (row_num >= TEST_ROWS_NUM_MAX)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }

        return ESP_OK;
    }

    // verify condition
    double next_step = _test_start_timestamp[slot] + (row_num * value * 1.0);
    bool condition_done = (time_get_ftimestamp() > next_step);
    if (condition_done)
    {
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_logging_condition_interval_logging_table(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_logging_t *test_logging = &(test_setup->logging);
    test_row_t *first_row = test_table->row;
    double *data = first_row->data;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    int sensor_index = test_logging->parameters.interval_logging_table.sensor_index;
    logging_direction_t direction = test_logging->parameters.interval_logging_table.direction;
    double *value = test_logging->parameters.interval_logging_table.value;
    int size = test_logging->parameters.interval_logging_table.size;

    assert(size >= 1);

    // execute condition
    // verify row num max
    if (row_num >= TEST_ROWS_NUM_MAX)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }

        return ESP_OK;
    }

    // verify condition
    double first_value = data[sensor_index];
    int value_index = (row_num - 1) % size;
    int cycles_num = (row_num - 1) / size;
    double next_step = first_value + (cycles_num * value[size - 1] + value[value_index]) * ((direction == LOGGING_DIRECTION_UP) ? 1.0 : -1.0);
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = ((direction == LOGGING_DIRECTION_UP) ? (latest_value > next_step) : (latest_value < next_step));
    if (condition_done)
    {
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
        }
    }
    return ESP_OK;
}
esp_err_t test_logging_condition_elapsed_time_table(int slot)
{
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;

    // test vars
    test_logging_t *test_logging = &(test_setup->logging);
    int row_num = test_table->row_num;

    assert(row_num >= 1);

    test_time_t *value = test_logging->parameters.elapsed_time_table.value;
    int size = test_logging->parameters.elapsed_time_table.size;

    assert(size >= 1);

    // execute condition
    // verify row num max
    if (row_num >= TEST_ROWS_NUM_MAX)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_ftimestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }

        return ESP_OK;
    }

    // verify condition
    int value_index = (row_num - 1) % size;
    int cycles_num = (row_num - 1) / size;
    double t = (value[size - 1].h * 3600.0) + (value[size - 1].m * 60.0) + (value[size - 1].s);
    double t1 = (value[value_index].h * 3600.0) + (value[value_index].m * 60.0) + (value[value_index].s);
    double next_step = _test_start_timestamp[slot] + (cycles_num * t + t1);
    bool condition_done = (time_get_ftimestamp() > next_step);
    if (condition_done)
    {
        test_table_add_row(slot);

        if (_foreground_slot == slot)
        {
            current_test_plot_points(slot);
            current_test_plot_last_data_p2_p3(slot);
        }
    }
    return ESP_OK;
}
/*
███████╗████████╗ ██████╗ ██████╗  █████╗  ██████╗ ███████╗
██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗██╔══██╗██╔════╝ ██╔════╝
███████╗   ██║   ██║   ██║██████╔╝███████║██║  ███╗█████╗
╚════██║   ██║   ██║   ██║██╔══██╗██╔══██║██║   ██║██╔══╝
███████║   ██║   ╚██████╔╝██║  ██║██║  ██║╚██████╔╝███████╗
╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝

*/

#include "esp_vfs.h"
#include "esp_vfs_fat.h"

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

void test_init_filesystem(void)
{
    // init file system
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = CURRENT_TEST_NUM_MAX + 2,
        .format_if_mount_failed = false,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE};
    esp_err_t err;

    err = esp_vfs_fat_spiflash_mount_rw_wl(FATFS_BASE_PATH, "storage", &mount_config, &s_wl_handle);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    // PRINT DISK INFO
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 0 */
    FRESULT res = f_getfree(FATFS_DISK_INDEX, &fre_clust, &fs);
    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    /* Print the free space (assuming 512 bytes/sector) */
    printf("%lu Bytes total drive space.\n%lu Bytes available.\n",
           tot_sect * CONFIG_WL_SECTOR_SIZE, fre_sect * CONFIG_WL_SECTOR_SIZE);
}

msc_host_device_handle_t msc_device;
msc_host_vfs_handle_t vfs_handle;
msc_host_device_info_t info;
BaseType_t task_created;
bool device_connected_ok = false;
static void msc_event_cb(const msc_host_event_t *event, void *arg)
{
    if (event->event == MSC_DEVICE_CONNECTED)
    {
        ESP_LOGI(TAG, "MSC device connected");
        SEND_I32_MSG(main_queue, USB_CONNECTED, event->device.address, portMAX_DELAY);
    }
    else if (event->event == MSC_DEVICE_DISCONNECTED)
    {
        ESP_LOGI(TAG, "MSC device disconnected");
        SEND_EMPTY_MSG(main_queue, USB_DISCONNECTED, portMAX_DELAY);
    }
}
static void print_device_info(msc_host_device_info_t *info)
{
    const size_t megabyte = 1024 * 1024;
    uint64_t capacity = ((uint64_t)info->sector_size * info->sector_count) / megabyte;

    printf("Device info:\n");
    printf("\t Capacity: %llu MB\n", capacity);
    printf("\t Sector size: %" PRIu32 "\n", info->sector_size);
    printf("\t Sector count: %" PRIu32 "\n", info->sector_count);
    printf("\t PID: 0x%4X \n", info->idProduct);
    printf("\t VID: 0x%4X \n", info->idVendor);
    wprintf(L"\t iProduct: %S \n", info->iProduct);
    wprintf(L"\t iManufacturer: %S \n", info->iManufacturer);
    wprintf(L"\t iSerialNumber: %S \n", info->iSerialNumber);
}
void usb_device_connected(int device_address)
{
    if (device_connected_ok == true)
    {
        ESP_LOGW(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 1024,
    };

    if (msc_host_install_device(device_address, &msc_device) != ESP_OK)
    {
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        device_connected_ok = false;
        return;
    }

    msc_host_print_descriptors(msc_device);

    if (msc_host_get_device_info(msc_device, &info) != ESP_OK)
    {
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        device_connected_ok = false;
        return;
    }

    print_device_info(&info);

    if (msc_host_vfs_register(msc_device, "/usb", &mount_config, &vfs_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        device_connected_ok = false;
        return;
    }

    ESP_LOGI(TAG, "%s, line:%d", __FILE__, __LINE__);
    device_connected_ok = true;
}
void usb_device_disconnected()
{
    if (device_connected_ok == false)
    {
        ESP_LOGW(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    msc_host_vfs_unregister(vfs_handle);
    msc_host_uninstall_device(msc_device);

    device_connected_ok = false;
}
// Handles common USB host library events
static void handle_usb_events(void *args)
{
    while (1)
    {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);

        // Release devices once all clients has deregistered
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)
        {
            usb_host_device_free_all();
        }
        // Give ready_to_uninstall_usb semaphore to indicate that USB Host library
        // can be deinitialized, and terminate this task.
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
        {
        }
    }

    vTaskDelete(NULL);
}
void test_init_usb(void)
{
    // install usb host
    const usb_host_config_t host_config = {.intr_flags = ESP_INTR_FLAG_LEVEL1};
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // create daemon task
    task_created = xTaskCreate(handle_usb_events, "usb_events", 2048, NULL, 2, NULL);
    assert(task_created);

    // install msc host
    const msc_host_driver_config_t msc_config = {
        .create_backround_task = true,
        .task_priority = 5,
        .stack_size = 2048,
        .callback = msc_event_cb,
    };
    ESP_ERROR_CHECK(msc_host_install(&msc_config));
}
void test_save_on_usb(int slot)
{
    assert(_test_status[slot] == TEST_DONE);

    if (device_connected_ok == false)
    {
        ESP_LOGW(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    //  global var
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_aditional_info_t *aditional_info = _test_aditional_info + slot;

    // get available name
    snprintf(buffer, sizeof(buffer), "%s/%s.%s", "/usb", test_setup->test_name, "csv");
    struct stat file_stat;
    bool name_available = (stat(buffer, &file_stat) != 0);
    if (!name_available)
    {
        // ESP_LOGI(TAG, "%s, line:%d", __FILE__, __LINE__);
        int i = 1;
        while (name_available == false)
        {
            // ESP_LOGI(TAG, "%s, line:%d", __FILE__, __LINE__);
            snprintf(buffer, sizeof(buffer), "%s/%s(%i).%s", "/usb", test_setup->test_name, i, "csv");
            struct stat file_stat;
            name_available = (stat(buffer, &file_stat) != 0);
            i++;
        }
    }
    ESP_LOGI(TAG, "filename(usb): %s", buffer);

    // open
    FILE *f;
    f = fopen(buffer, "ab");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "%s, line:%d, Err: %d", __FILE__, __LINE__, errno);
        return;
    }

    // save csv
    bool save_csv_success = (fwrite(csv, 1, csv_size, f) == csv_size);
    if (!save_csv_success)
    {
        fclose(f);
        remove(buffer);
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    ESP_LOGI(TAG, "test_save_on_usb : OK");
    fclose(f);
}
void test_save_on_filesystem(int slot)
{
    assert(_test_status[slot] == TEST_DONE);

    //  global var
    test_setup_t *test_setup = _test_setup + slot;
    test_table_t *test_table = _test_table + slot;
    test_aditional_info_t *aditional_info = _test_aditional_info + slot;

    // check available space in fatfs
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res = f_getfree(FATFS_DISK_INDEX, &fre_clust, &fs);
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;
    uint32_t bytes_free = fre_sect * CONFIG_WL_SECTOR_SIZE;
    if (bytes_free < TEST_FS_SLOT_SIZE_MIN)
    {
        ESP_LOGW(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    // get available name
    snprintf(buffer, sizeof(buffer), "%s/%s.%s", FATFS_BASE_PATH, test_setup->test_name, FATFS_FILETYPE_TEST);
    struct stat file_stat;
    bool name_available = (stat(buffer, &file_stat) != 0);
    if (!name_available)
    {
        // ESP_LOGI(TAG, "%s, line:%d", __FILE__, __LINE__);
        int i = 1;
        while (name_available == false)
        {
            // ESP_LOGI(TAG, "%s, line:%d", __FILE__, __LINE__);
            snprintf(buffer, sizeof(buffer), "%s/%s(%i).%s", FATFS_BASE_PATH, test_setup->test_name, i, FATFS_FILETYPE_TEST);
            struct stat file_stat;
            name_available = (stat(buffer, &file_stat) != 0);
            i++;
        }
    }

    ESP_LOGI(TAG, "filename(spiflash): %s", buffer);

    // save on internal filesystem
    // open
    FILE *f;
    f = fopen(buffer, "ab");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "%s, line:%d, Err: %d", __FILE__, __LINE__, errno);
        return;
    }

    // save setup
    bool save_setup_success = (fwrite(test_setup, 1, sizeof(test_setup_t), f) == sizeof(test_setup_t));
    if (!save_setup_success)
    {
        fclose(f);
        remove(buffer);
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    // save table
    bool save_table_success = (fwrite(test_table, 1, sizeof(test_table_t), f) == sizeof(test_table_t));
    if (!save_table_success)
    {
        fclose(f);
        remove(buffer);
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    // save aditional_info
    bool save_aditional_success = (fwrite(aditional_info, 1, sizeof(test_aditional_info_t), f) == sizeof(test_aditional_info_t));
    if (!save_aditional_success)
    {
        fclose(f);
        remove(buffer);
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    // save csv
    bool save_csv_success = (fwrite(csv, 1, csv_size, f) == csv_size);
    if (!save_csv_success)
    {
        fclose(f);
        remove(buffer);
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
        return;
    }

    ESP_LOGI(TAG, "test_save_on_filesystem : OK");
    fclose(f);

    // IMPRIMIR CONTENIDO
    /*
    struct dirent *d;
    DIR *dr;
    dr = opendir(FATFS_BASE_PATH);
    if (dr != NULL)
    {
        printf("List of Files & Folders: \n\r");
        for (d = readdir(dr); d != NULL; d = readdir(dr))
        {
            printf("-\"%s\"\n\r", d->d_name);
        }
        closedir(dr);
    }
    else
    {
        ESP_LOGE(TAG, "%s, line:%d", __FILE__, __LINE__);
    }
    */
}

/*
███████╗██╗      ██████╗ ████████╗
██╔════╝██║     ██╔═══██╗╚══██╔══╝
███████╗██║     ██║   ██║   ██║
╚════██║██║     ██║   ██║   ██║
███████║███████╗╚██████╔╝   ██║
╚══════╝╚══════╝ ╚═════╝    ╚═╝

*/
esp_err_t test_is_an_slot_in_execution()
{
    for (int i = 0; i < CURRENT_TEST_NUM_MAX; i++)
    {
        if ((_test_status[i] == TEST_TRIGGERING) || (_test_status[i] == TEST_RUNNING))
        {
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}
esp_err_t test_get_available_slot(int *slot)
{
    for (int i = 0; i < CURRENT_TEST_NUM_MAX; i++)
    {
        if (_test_status[i] == TEST_VOID)
        {
            *slot = i;
            return ESP_OK;
        }
    }

    ESP_LOGW(TAG, "%s, line: %d", __FILE__, __LINE__);
    return ESP_FAIL;
}
void test_deinit_all_slot_initialized()
{
    _initialized_slot = CURRENT_TEST_NUM_MAX;

    for (int i = 0; i < CURRENT_TEST_NUM_MAX; i++)
    {
        if (_test_status[i] == TEST_INITIALIZED)
        {
            _test_status[i] = TEST_VOID;
        }
    }
}
void test_deinit_slot_configured(int slot)
{
    // assert(_test_status[slot] >= TEST_CONFIGURED);
    assert(_test_status[slot] == TEST_DONE);

    // unlock sensors
    test_unlock_sensors(slot);

    _test_status[slot] = TEST_VOID;
    if (_foreground_slot == slot)
    {
        _foreground_slot = CURRENT_TEST_NUM_MAX;
        // current_test_reset_default_p1_p2_p3();
    }
}
esp_err_t test_init_available_slot()
{
    esp_err_t err;

    // DELETE INCOMPLETED SETUPS
    test_deinit_all_slot_initialized();

    // GET FREE SLOT
    int slot;
    bool no_slot_available = test_get_available_slot(&slot) != ESP_OK;
    if (no_slot_available)
    {
        ESP_LOGW(TAG, "%s, line: %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // INITIALIZE TEST SETUP
    _test_setup[slot].type = TEST_TYPE_MAX;
    _test_setup[slot].inputs.input_enable.byte = 0;
    _test_setup[slot].inputs.tara_enable.byte = 0;
    _test_setup[slot].logging.condition = LOGGING_CONDITION_MAX;
    _test_setup[slot].stop.condition = STOP_CONDITION_MAX;
    _test_setup[slot].graph.x_axis_option = AXIS_MAX;
    _test_setup[slot].graph.y_axis_option = AXIS_MAX;
    _test_setup[slot].start.condition = START_CONDITION_MAX;
    _test_setup[slot].start.parameters.greater_than.sensor_index = SENSOR_1_INDEX;
    _test_setup[slot].start.parameters.less_than.sensor_index = SENSOR_1_INDEX;
    memcpy(_test_setup[slot].test_name, "DEFAULT TEST NAME", 7);

    // INITIALIZE DEFAULT PAGES
    _loggging_page = PAGE_NEW_TEST_P3_1;
    _stop_page = PAGE_NEW_TEST_P4_1;
    _start_page = PAGE_NEW_TEST_P7_1;

    // INITIALIZE TEST TABLE
    _test_table[slot].row_num = 0;
    for (int i = 0; i < NUM_SENSORS; i++)
        _test_table[slot].peak_values[i] = -9999999;

    // INITIALIZE TIMESTAMPS
    _test_setup_timestamp[slot] = 0;
    _test_start_timestamp[slot] = 0;
    _test_stop_timestamp[slot] = 0;

    // SET SLOT INITIALIZED
    _initialized_slot = slot;

    // SET OPERATOR STOP
    _operator_stop[slot] = false;

    _initialized_slot = slot;
    _test_status[_initialized_slot] = TEST_INITIALIZED;

    return ESP_OK;
}
void test_complete_initialized_slot(void)
{
    assert(_initialized_slot != CURRENT_TEST_NUM_MAX);
    assert(_test_status[_initialized_slot] == TEST_INITIALIZED);

    // change status of initialized test to configured test
    // and set it as foreground
    _foreground_slot = _initialized_slot;
    _initialized_slot = CURRENT_TEST_NUM_MAX;

    _test_status[_foreground_slot] = TEST_CONFIGURED;
    _test_setup_timestamp[_foreground_slot] = time_get_ftimestamp();

    // define aditional strucute
    test_define_aditional_info(_foreground_slot);
    // lock test sensors
    test_lock_sensors(_foreground_slot);

    // SET status as triggering
    // ready to plot and work
    _test_status[_foreground_slot] = TEST_TRIGGERING;

    // plot new foreground slot
    current_test_load_foreground_slot();
}

void test_run_slot(int slot)
{
    // END EXECUTION IF TEST NOT CONFIGURED
    bool test_is_unconfigured = ((_test_status[slot] <= TEST_CONFIGURED));
    if (test_is_unconfigured)
    {
        // ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // LOOK FOR START CONDITION
    bool test_is_triggering = (_test_status[slot] == TEST_TRIGGERING);
    if (test_is_triggering)
    {
        switch (_test_setup[slot].start.condition)
        {
        case START_CONDITION_TRIGGER_IMMEDIATELY:
            test_start_condition_trigger_immediately(slot);
            break;
        case START_CONDITION_TIME_DELAY:
            test_start_condition_time_delay(slot);
            break;
        case START_CONDITION_GREATER_THAN:
            test_start_condition_greater_than(slot);
            break;
        default: // START_CONDITION_LESS_THAN
            test_start_condition_less_than(slot);
            break;
        }
        return;
    }

    // LOOK FOR STOP CONDITION and LOGGGING CONDITION
    bool test_is_running = (_test_status[slot] == TEST_RUNNING);
    if (test_is_running)
    {
        // LOGGING
        switch (_test_setup[slot].logging.condition)
        {
        case LOGGING_CONDITION_INTERVAL_LOGGING:
            test_logging_condition_interval_logging(slot);
            break;
        case LOGGING_CONDITION_LINEAR_TIME_INTERVAL:
            test_logging_condition_linear_time_interval(slot);
            break;
        case LOGGING_CONDITION_INTERVAL_LOGGING_TABLE:
            test_logging_condition_interval_logging_table(slot);
            break;
        default: // LOGGING_CONDITION_ELAPSED_TIME_TABLE
            test_logging_condition_elapsed_time_table(slot);
            break;
        }

        // STOP
        switch (_test_setup[slot].stop.condition)
        {
        case STOP_CONDITION_GREATER_THAN:
            test_stop_condition_greater_than(slot);
            break;
        case STOP_CONDITION_LESS_THAN:
            test_stop_condition_less_than(slot);
            break;
        case STOP_CONDITION_TIME_DELAY:
            test_stop_condition_time_delay(slot);
            break;
        case STOP_CONDITION_DISTANCE_UP:
            test_stop_condition_distance_up(slot);
            break;
        case STOP_CONDITION_DISTANCE_DOWN:
            test_stop_condition_distance_down(slot);
            break;
        case STOP_CONDITION_PERCENT_DROP:
            test_stop_condition_percent_drop(slot);
            break;
        case STOP_CONDITION_PERCENT_STRAIN:
            test_stop_condition_percent_strain(slot);
            break;
        case STOP_CONDITION_PERCENT_STRESS_DROP:
            test_stop_condition_percent_stress_drop(slot);
            break;
        default: // STOP_CONDITION_OPERATOR_STOP
            test_stop_condition_operator_stop(slot);
            break;
        }
        return;
    }

    // TEST STATUS : DONE
    test_generate_csv(slot);
    test_save_on_usb(slot);
    test_save_on_filesystem(slot);

    ESP_LOGW(TAG, "%s: line %d", __FILE__, __LINE__);
    test_deinit_slot_configured(slot);
}
void test_run_all(void)
{
    for (int i = 0; i < CURRENT_TEST_NUM_MAX; i++)
        test_run_slot(i);
}
