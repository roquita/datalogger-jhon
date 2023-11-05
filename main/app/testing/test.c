
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

#define TAG "TEST"

static test_setup_t *_test_setup[CURRENT_TEST_NUM_MAX] = {NULL};
static page_t _loggging_page;
static page_t _stop_page;
static page_t _start_page;

static test_table_t *_test_table[CURRENT_TEST_NUM_MAX] = {NULL};
static int64_t _test_setup_timestamp[CURRENT_TEST_NUM_MAX];
static int64_t _test_start_timestamp[CURRENT_TEST_NUM_MAX];
static int64_t _test_stop_timestamp[CURRENT_TEST_NUM_MAX];
static test_status_t _test_status[CURRENT_TEST_NUM_MAX] = {TEST_VOID};
static int _initialized_slot = CURRENT_TEST_NUM_MAX;
static int _foreground_slot = CURRENT_TEST_NUM_MAX;
static bool _operator_stop[CURRENT_TEST_NUM_MAX];

static char buffer[50];

static void test_force_stop(int slot)
{
    assert((slot >= 0) && (slot < CURRENT_TEST_NUM_MAX));

    _test_status[slot] = TEST_DONE;
    _test_stop_timestamp[slot] = time_get_timestamp();

    if (_foreground_slot == slot)
    {
        current_test_plot_status(slot);
    }
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
static int test_get_subindex(int input_enable, int sensor_index)
{
    int subindex = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            subindex++;
        }
        input_enable >>= 1;
    }
    subindex = (subindex < 0) ? 0 : subindex;
    return subindex;
}
static void test_lock_sensors(int slot)
{
    assert(_test_status[slot] == TEST_INITIALIZED);
    int input_enable = _test_setup[slot]->inputs.input_enable.byte;
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
    assert(_test_status[slot] >= TEST_CONFIGURED);
    int input_enable = _test_setup[slot]->inputs.input_enable.byte;
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

int test_get_tara_enable()
{
    return _test_setup[_initialized_slot]->inputs.tara_enable.byte;
}
void test_print_elapsed_time()
{
    if (_foreground_slot == CURRENT_TEST_NUM_MAX)
        return;

    char buffer[20];

    int64_t init = _test_setup_timestamp[_foreground_slot];
    int64_t actual = time_get_timestamp();
    int64_t elapsed = actual - init;

    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;

    snprintf(buffer, sizeof(buffer), "%02i:%02i:%02i", hours, minutes, seconds);

    nextion_1_current_test_p1_write_time(buffer);
    nextion_1_current_test_p2_write_time(buffer);
    nextion_1_current_test_p3_write_time(buffer);
}
/*
██████╗ ██╗   ██╗███╗   ██╗ █████╗ ███╗   ███╗██╗ ██████╗    ███╗   ███╗███████╗███╗   ███╗
██╔══██╗╚██╗ ██╔╝████╗  ██║██╔══██╗████╗ ████║██║██╔════╝    ████╗ ████║██╔════╝████╗ ████║
██║  ██║ ╚████╔╝ ██╔██╗ ██║███████║██╔████╔██║██║██║         ██╔████╔██║█████╗  ██╔████╔██║
██║  ██║  ╚██╔╝  ██║╚██╗██║██╔══██║██║╚██╔╝██║██║██║         ██║╚██╔╝██║██╔══╝  ██║╚██╔╝██║
██████╔╝   ██║   ██║ ╚████║██║  ██║██║ ╚═╝ ██║██║╚██████╗    ██║ ╚═╝ ██║███████╗██║ ╚═╝ ██║
╚═════╝    ╚═╝   ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝    ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝

*/
void free_test_setup(test_setup_t *test_setup)
{
    free(test_setup);
}
void free_test_table(test_table_t *test_table)
{
    double *peak_values = test_table->peak_values;
    if (peak_values != NULL)
        free(peak_values);

    test_row_t *row = test_table->first_row;
    while (row != NULL)
    {
        // printf("xxx \n");
        double *data = row->data;
        if (data != NULL)
            free(data);

        test_row_t *next_row = (test_row_t *)(row->next);
        free(row);
        row = next_row;
    }
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
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    _test_setup[_initialized_slot]->type = *type;
    test_print_type();

    return ESP_OK;
}
esp_err_t test_set_inputs(test_inputs_t *inputs)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup[_initialized_slot]->inputs), inputs, sizeof(test_inputs_t));
    test_print_inputs();

    return ESP_OK;
}
esp_err_t test_set_logging(test_logging_t *logging)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup[_initialized_slot]->logging), logging, sizeof(test_logging_t));
    test_print_logging();

    return ESP_OK;
}
esp_err_t test_set_stop(test_stop_t *stop)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup[_initialized_slot]->stop), stop, sizeof(test_stop_t));
    test_print_stop();

    return ESP_OK;
}
esp_err_t test_set_motor(test_motor_t *motor)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup[_initialized_slot]->motor), motor, sizeof(test_motor_t));
    test_print_motor();

    return ESP_OK;
}
esp_err_t test_set_graph(test_graph_t *graph)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup[_initialized_slot]->graph), graph, sizeof(test_graph_t));
    test_print_graph();

    return ESP_OK;
}
esp_err_t test_set_start(test_start_t *start)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup[_initialized_slot]->start), start, sizeof(test_start_t));
    test_print_start();

    return ESP_OK;
}
void test_print_type(void)
{
    switch (_test_setup[_initialized_slot]->type)
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
        ESP_LOGI(TAG, "  TYPE: %d", _test_setup[_initialized_slot]->type);
        break;
    }
}
void test_print_inputs(void)
{
    ESP_LOGI(TAG, "  INPUTS: [1 YES, 0 NO]");
    ESP_LOGI(TAG, "     0- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s0, _test_setup[_initialized_slot]->inputs.tara_enable.s0);
    ESP_LOGI(TAG, "     1- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s1, _test_setup[_initialized_slot]->inputs.tara_enable.s1);
    ESP_LOGI(TAG, "     2- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s2, _test_setup[_initialized_slot]->inputs.tara_enable.s2);
    ESP_LOGI(TAG, "     3- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s3, _test_setup[_initialized_slot]->inputs.tara_enable.s3);
    ESP_LOGI(TAG, "     4- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s4, _test_setup[_initialized_slot]->inputs.tara_enable.s4);
    ESP_LOGI(TAG, "     5- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s5, _test_setup[_initialized_slot]->inputs.tara_enable.s5);
    ESP_LOGI(TAG, "     6- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s6, _test_setup[_initialized_slot]->inputs.tara_enable.s6);
    ESP_LOGI(TAG, "     7- input_enabled: %d, tara_enabled: %d", _test_setup[_initialized_slot]->inputs.input_enable.s7, _test_setup[_initialized_slot]->inputs.tara_enable.s7);
}
void test_print_logging(void)
{
    ESP_LOGI(TAG, "  LOGGING:");
    switch (_test_setup[_initialized_slot]->logging.condition)
    {
    case LOGGING_CONDITION_INTERVAL_LOGGING:
        ESP_LOGI(TAG, "  condition: INTERVAL_LOGGING");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->logging.parameters.interval_logging.sensor_index);
        ESP_LOGI(TAG, "  direction: %d [0 UP, 1 DOWN]", _test_setup[_initialized_slot]->logging.parameters.interval_logging.direction);
        ESP_LOGI(TAG, "  value: %lf (unit)", _test_setup[_initialized_slot]->logging.parameters.interval_logging.value);
        break;
    case LOGGING_CONDITION_LINEAR_TIME_INTERVAL:
        ESP_LOGI(TAG, "  condition: LINEAR_TIME_INTERVAL");
        ESP_LOGI(TAG, "  value: %lu (s)", _test_setup[_initialized_slot]->logging.parameters.linear_time_interval.value);
        break;
    case LOGGING_CONDITION_INTERVAL_LOGGING_TABLE:
        ESP_LOGI(TAG, "  condition: INTERVAL_LOGGING_TABLE");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->logging.parameters.interval_logging_table.sensor_index);
        ESP_LOGI(TAG, "  direction: %d [0 UP, 1 DOWN]", _test_setup[_initialized_slot]->logging.parameters.interval_logging_table.direction);
        ESP_LOGI(TAG, "  value:");
        for (int i = 0; i < 30; i++)
        {
            ESP_LOGI(TAG, "     [%i]=%lf (unit)", i, _test_setup[_initialized_slot]->logging.parameters.interval_logging_table.value[i]);
        }
        break;
    case LOGGING_CONDITION_ELAPSED_TIME_TABLE:
        ESP_LOGI(TAG, "  condition: ELAPSED_TIME_TABLE");
        ESP_LOGI(TAG, "  value:");
        for (int i = 0; i < 30; i++)
        {
            test_time_t *value = &(_test_setup[_initialized_slot]->logging.parameters.elapsed_time_table.value[i]);
            ESP_LOGI(TAG, "     [%i]=> h=%d, m=%d, s=%.2f", i, value->h, value->m, value->s);
        }
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup[_initialized_slot]->logging.condition);
        break;
    }
}
void test_print_stop(void)
{
    ESP_LOGI(TAG, "  STOP:");
    switch (_test_setup[_initialized_slot]->stop.condition)
    {
    case STOP_CONDITION_GREATER_THAN:
        ESP_LOGI(TAG, "  condition: GREATER_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.greater_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.greater_than.value);
        break;
    case STOP_CONDITION_LESS_THAN:
        ESP_LOGI(TAG, "  condition: LESS_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.less_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.less_than.value);
        break;
    case STOP_CONDITION_TIME_DELAY:
        ESP_LOGI(TAG, "  condition: TIME_DELAY");
        ESP_LOGI(TAG, "  value: %lu", _test_setup[_initialized_slot]->stop.parameters.time_delay.value);
        break;
    case STOP_CONDITION_DISTANCE_UP:
        ESP_LOGI(TAG, "  condition: DISTANCE_UP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.distance_up.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.distance_up.value);
        break;
    case STOP_CONDITION_DISTANCE_DOWN:
        ESP_LOGI(TAG, "  condition: DISTANCE_DOWN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.distance_down.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.distance_down.value);
        break;
    case STOP_CONDITION_PERCENT_DROP:
        ESP_LOGI(TAG, "  condition: PERCENT_DROP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.percent_drop.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.percent_drop.value);
        break;
    case STOP_CONDITION_PERCENT_STRAIN:
        ESP_LOGI(TAG, "  condition: PERCENT_STRAIN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.percent_strain.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.percent_strain.value);
        break;
    case STOP_CONDITION_PERCENT_STRESS_DROP:
        ESP_LOGI(TAG, "  condition: PERCENT_STRESS_DROP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->stop.parameters.percent_stress_drop.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->stop.parameters.percent_stress_drop.value);
        break;
    case STOP_CONDITION_OPERATOR_STOP:
        ESP_LOGI(TAG, "  condition: OPERATOR_STOP");
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup[_initialized_slot]->stop.condition);
        break;
    }
}
void test_print_motor(void)
{
}
void test_print_graph(void)
{
    switch (_test_setup[_initialized_slot]->graph.x_axis_option)
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
        ESP_LOGI(TAG, "  GRAPH: X_AXIS %d", _test_setup[_initialized_slot]->graph.x_axis_option);
        break;
    }

    switch (_test_setup[_initialized_slot]->graph.y_axis_option)
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
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS %d", _test_setup[_initialized_slot]->graph.y_axis_option);
        break;
    }
}
void test_print_start(void)
{
    ESP_LOGI(TAG, "  START:");
    switch (_test_setup[_initialized_slot]->start.condition)
    {
    case START_CONDITION_TRIGGER_IMMEDIATELY:
        ESP_LOGI(TAG, "  condition: TRIGGER_IMMEDIATELY");
        break;
    case START_CONDITION_TIME_DELAY:
        ESP_LOGI(TAG, "  condition: TIME_DELAY");
        ESP_LOGI(TAG, "  value: %lu", _test_setup[_initialized_slot]->start.parameters.time_delay.value);
        break;
    case START_CONDITION_GREATER_THAN:
        ESP_LOGI(TAG, "  condition: GREATER_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->start.parameters.greater_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->start.parameters.greater_than.value);
        ESP_LOGI(TAG, "  current_value: %lf", _test_setup[_initialized_slot]->start.parameters.greater_than.current_value);
        ESP_LOGI(TAG, "  first_point: %d [0 AT TRIGGER, 1 AT INITIAL VALUE]", _test_setup[_initialized_slot]->start.parameters.greater_than.first_point);
        break;
    case START_CONDITION_LESS_THAN:
        ESP_LOGI(TAG, "  condition: LESS_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup[_initialized_slot]->start.parameters.less_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup[_initialized_slot]->start.parameters.less_than.value);
        ESP_LOGI(TAG, "  current_value: %lf", _test_setup[_initialized_slot]->start.parameters.less_than.current_value);
        ESP_LOGI(TAG, "  first_point: %d [0 AT TRIGGER, 1 AT INITIAL VALUE]", _test_setup[_initialized_slot]->start.parameters.less_than.first_point);
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup[_initialized_slot]->logging.condition);
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
    if (_test_setup[_initialized_slot] == NULL)
        return ESP_ERR_NO_MEM;

    uint16_t input_enable = _test_setup[_initialized_slot]->inputs.input_enable.byte;
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
    if (_test_setup[_initialized_slot] == NULL)
        return ESP_ERR_NO_MEM;

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
    if (_test_setup[_initialized_slot] == NULL)
        return ESP_ERR_NO_MEM;

    if ((combobox_val >= 0) && (combobox_val <= 3))
    {
        *axis_option = (axis_option_t)(combobox_val - 4);
        return ESP_OK;
    }
    else if ((combobox_val >= 4) && (combobox_val <= 11))
    {
        combobox_val -= 4;
        uint16_t input_enable = _test_setup[_initialized_slot]->inputs.input_enable.byte;
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
    ESP_LOGI(TAG, "Greater than, set sensor_index: %u", sensor_index);
    _test_setup[_initialized_slot]->start.parameters.greater_than.sensor_index = sensor_index;
    return ESP_OK;
}
esp_err_t test_start_GreaterThan_get_sensor_index(int *sensor_index)
{
    *sensor_index = _test_setup[_initialized_slot]->start.parameters.greater_than.sensor_index;
    return ESP_OK;
}
esp_err_t test_start_LessThan_set_sensor_index(int sensor_index)
{
    ESP_LOGI(TAG, "Less than, set sensor_index: %u", sensor_index);
    _test_setup[_initialized_slot]->start.parameters.less_than.sensor_index = sensor_index;
    return ESP_OK;
}
esp_err_t test_start_LessThan_get_sensor_index(int *sensor_index)
{
    // if (_test_setup == NULL)
    //    return ESP_ERR_NO_MEM;

    *sensor_index = _test_setup[_initialized_slot]->start.parameters.less_than.sensor_index;
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
    // GLOBAL VARS
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    int row_num = test_table->row_num;
    int sensor_num = test_setup->inputs.sensor_num;
    double *peak_values = test_table->peak_values;
    uint8_t input_enable = test_setup->inputs.input_enable.byte;
    uint8_t tara_enable = test_setup->inputs.tara_enable.byte;

    // ALLOC PEAK VALUES IF NOT
    if (peak_values == NULL)
    {
        peak_values = (double *)malloc(sensor_num * sizeof(double));
        if (peak_values == NULL)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
        test_table->peak_values = peak_values;
        for (int i = 0; i < sensor_num; i++)
        {
            *(peak_values + i) = -999999.0;
            // peak_values++;
        }
    }

    // ALLOC NEW ROW
    test_row_t *row = (test_row_t *)malloc(sizeof(test_row_t));
    if (row == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // SET NEW-ROW-INDEX
    row->index = row_num;

    // SET NEW-ROW-TIME
    bool is_test_running = _test_status[slot] == TEST_RUNNING;
    int64_t seconds_num = (is_test_running) ? (time_get_timestamp() - _test_start_timestamp[slot]) : 0;
    row->time.h = seconds_num / 3600;
    row->time.m = (seconds_num % 3600) / 60;
    row->time.s = seconds_num % 60;

    // SET NEW-ROW-NEXT
    row->next = NULL;

    // SET NEW-ROW-DATA AND TABLE-PEAK-VALUES
    // alloc data
    double *data = (double *)malloc(sensor_num * sizeof(double));
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        free(row);
        return ESP_FAIL;
    }
    // attach data to new-row
    row->data = data;
    // set data and update peak values if sensor was choosen for testing
    for (int sensor_index = 0; sensor_index < NUM_SENSORS; sensor_index++)
    {
        bool sensor_enabled = (input_enable & 1) == 1;
        bool tara_enabled = (tara_enable & 1) == 1;
        if (sensor_enabled)
        {
            double real = sensor_get_real(sensor_index, tara_enabled);
            *data = real;
            double max_value = *peak_values;
            if (real > max_value)
                *peak_values = real;

            data++;
            peak_values++;
        }
        input_enable >>= 1;
        tara_enable >>= 1;
    }

    // SET TABLE-ROW-NUM
    test_table->row_num = row_num + 1;

    // ATTACH NEW-ROW TO TABLE
    if (test_table->first_row == NULL)
    {
        // printf("aaa\n");
        test_table->first_row = row;
        test_table->last_row = row;
    }
    else
    {
        // printf("bbb\n");
        test_table->last_row->next = row;
        test_table->last_row = row;
    }

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

    test_table_t *test_table = _test_table[slot];
    nextion_1_current_test_p1_write_points(test_table->row_num);
    nextion_1_current_test_p2_write_points(test_table->row_num);
    nextion_1_current_test_p3_write_points(test_table->row_num);
}
void current_test_plot_last_data_p1(void)
{
    if (_foreground_slot == CURRENT_TEST_NUM_MAX)
        return;

    // test vars
    test_inputs_t *test_inputs = &(_test_setup[_foreground_slot]->inputs);
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    // ESP_LOGI(TAG, "input_enable:%d || tara_enable:%d", input_enable, tara_enable);

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
    test_start_t *test_start = &(test_setup->start);
    test_graph_t *test_graph = &(test_setup->graph);

    // test vars
    uint8_t input_enable = test_setup->inputs.input_enable.byte;
    uint8_t tara_enable = test_setup->inputs.tara_enable.byte;
    axis_option_t x_axis_option = test_graph->x_axis_option;
    axis_option_t y_axis_option = test_graph->y_axis_option;

    // PRINT INDEX AND TIME
    {
        test_row_t *row = test_table->last_row;
        int row_index = row->index;
        test_time_t time = row->time;

        snprintf(buffer, sizeof(buffer), "%i", row_index);
        nextion_1_current_test_p2_append_index(buffer);

        snprintf(buffer, sizeof(buffer), "%02i:%02i:%02.0f", time.h, time.m, time.s);
        nextion_1_current_test_p2_append_time(buffer);
    }

    // PRINT ON TABLE AND GRAPH
    for (int sensor_index = 0; sensor_index < NUM_SENSORS; sensor_index++)
    {
        // TABLE
        bool sensor_enabled = (input_enable & 1) == 1;
        bool tara_enabled = (tara_enable & 1) == 1;

        char *content = sensor_enabled ? sensor_get_real_str(sensor_index, tara_enabled) : "";
        nextion_1_current_test_p2_append_sensor_data(sensor_index, content);

        // GRAPH
        bool sensor_selected_for_graph = (y_axis_option == (uint32_t)sensor_index);
        if (sensor_selected_for_graph)
        {
            double real = sensor_get_real(sensor_index, tara_enabled);
            double capacity = sensor_get_capacity(sensor_index);
            int value = 100.0 * real / capacity;
            value = (value > 100) ? 100 : value;
            //nextion_1_current_test_p3_append_data(50);
            nextion_1_current_test_p3_append_data(value);
        }

        input_enable >>= 1;
        tara_enable >>= 1;
    }
}
void current_test_load_foreground_slot(void)
{
    assert(_foreground_slot != CURRENT_TEST_NUM_MAX);
    assert(_test_status[_foreground_slot] >= TEST_CONFIGURED);

    // GLOBAL VARS
    test_table_t *test_table = _test_table[_foreground_slot];
    test_setup_t *test_setup = _test_setup[_foreground_slot];

    // test vars
    int input_enable = test_setup->inputs.input_enable.byte;
    axis_option_t x_axis_option = test_setup->graph.x_axis_option;
    axis_option_t y_axis_option = test_setup->graph.y_axis_option;

    // set title on p1 p2 p3
    char *test_name = _test_setup[_foreground_slot]->test_name;
    char *datetime = time_get_datetime_formated(true, _test_setup_timestamp[_foreground_slot]);

    nextion_1_current_test_p1_write_title(test_name);
    nextion_1_current_test_p1_append_title(" - ");
    nextion_1_current_test_p1_append_title(datetime);

    nextion_1_current_test_p2_write_title(test_name);
    nextion_1_current_test_p2_append_title(" - ");
    nextion_1_current_test_p2_append_title(datetime);

    nextion_1_current_test_p3_write_title(test_name);
    nextion_1_current_test_p3_append_title(" - ");
    nextion_1_current_test_p3_append_title(datetime);

    // set status on p1 p2 p3
    nextion_1_current_test_p1_write_status(test_get_status_str(_foreground_slot));
    nextion_1_current_test_p2_write_status(test_get_status_str(_foreground_slot));
    nextion_1_current_test_p3_write_status(test_get_status_str(_foreground_slot));

    // set points on p1 p2 p3
    nextion_1_current_test_p1_write_points(test_table->row_num);
    nextion_1_current_test_p2_write_points(test_table->row_num);
    nextion_1_current_test_p3_write_points(test_table->row_num);

    // set time on p1 p2 p3
    {
        char buffer[20];
        int64_t elapsed_time = time_get_timestamp() - _test_setup_timestamp[_foreground_slot];
        int hours = elapsed_time / 3600;
        int minutes = (elapsed_time % 3600) / 60;
        int seconds = elapsed_time % 60;
        snprintf(buffer, sizeof(buffer), "%02i:%02i:%02i", hours, minutes, seconds);

        nextion_1_current_test_p1_write_time(buffer);
        nextion_1_current_test_p2_write_time(buffer);
        nextion_1_current_test_p3_write_time(buffer);
    }

    // set Y axis info on p3
    {
        bool is_sensor = ((y_axis_option >= AXIS_SENSOR_INDEX_0) && (y_axis_option < AXIS_MAX));
        if (is_sensor)
        {
            int index = y_axis_option;
            nextion_1_current_test_p3_write_sensor_name(sensor_get_name(index));
            nextion_1_current_test_p3_write_sensor_unit(sensor_get_unit_str(index));

            double capacity = sensor_get_capacity(index);
            int num_decimals = sensor_get_num_decimals(index);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 1.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y1(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 2.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y2(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 3.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y3(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 4.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y4(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 5.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y5(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 6.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y6(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 7.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y7(buffer);

            snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, 8.0 * capacity / 8.0);
            nextion_1_current_test_p3_write_Y8(buffer);
        }
    }

    // set sensors info on p1 p2
    {
        int enablers = input_enable;
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            bool input_enabled = (enablers & 1) == 1;

            // p1
            nextion_1_current_test_p1_write_sensor_name(i, input_enabled ? sensor_get_name(i) : CURRENT_TEST_SENSOR_NAME_DEFAULT);
            nextion_1_current_test_p1_write_data(i, CURRENT_TEST_DATA_DEFAULT);
            nextion_1_current_test_p1_write_unit(i, input_enabled ? sensor_get_unit_str(i) : CURRENT_TEST_UNIT_DEFAULT);
            nextion_1_current_test_p1_write_dataps(i, CURRENT_TEST_DATAPS_DEFAULT);
            nextion_1_current_test_p1_write_unitps(i, input_enabled ? sensor_get_unitps_str(i) : CURRENT_TEST_UNITPS_DEFAULT);

            // p2
            nextion_1_current_test_p2_write_sensor_name(i, input_enabled ? sensor_get_name(i) : CURRENT_TEST_SENSOR_NAME_DEFAULT);
            nextion_1_current_test_p2_write_unit(i, input_enabled ? sensor_get_unit_str(i) : CURRENT_TEST_UNIT_DEFAULT);
            nextion_1_current_test_p2_clean_sensor_data(i);

            enablers >>= 1;
        }
    }

    // load sensor data on p2
    {
        nextion_1_current_test_p2_clean_index();
        nextion_1_current_test_p2_clean_time();

        char buffer[15];
        test_row_t *row = test_table->first_row;
        while (row != NULL)
        {
            // index
            snprintf(buffer, sizeof(buffer), "%i", row->index);
            nextion_1_current_test_p2_append_index(buffer);

            // time
            snprintf(buffer, sizeof(buffer), "%2i:%2i:%2.2f", row->time.h, row->time.m, row->time.s);
            nextion_1_current_test_p2_append_time(buffer);

            // sensor
            int enablers = input_enable;
            double *data = row->data;
            for (int i = 0; i < NUM_SENSORS; i++)
            {
                bool input_enabled = (enablers & 1) == 1;
                if (input_enabled)
                {
                    snprintf(buffer, sizeof(buffer), "%.*lf", sensor_get_num_decimals(i), *data);
                    nextion_1_current_test_p2_append_sensor_data(i, buffer);
                    data++;
                }
                else
                {
                    nextion_1_current_test_p2_append_sensor_data(i, "");
                }

                enablers >>= 1;
            }

            // next row
            row = (test_row_t *)(row->next);
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

        int subindex = test_get_subindex(input_enable, y_axis_option);
        int row_num = test_table->row_num;
        int excedent_row_num = (row_num > 60) ? row_num - 60 : 0;
        test_row_t *row = test_table->first_row;

        while (excedent_row_num > 0)
        {
            row = (test_row_t *)(row->next);
            excedent_row_num--;
        }

        for (int i = 0; i < (row_num - excedent_row_num); i++)
        {
            double real = *((row->data) + subindex);
            double capacity = sensor_get_capacity(y_axis_option);
            int value = 100.0 * real / capacity;
            value = (value > 100) ? 100 : value;
            nextion_1_current_test_p3_write_slope(i, value);

            // next row
            row = (test_row_t *)(row->next);
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
    bool add_row_success = test_table_add_row(slot) == ESP_OK;
    if (!add_row_success)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
    test_start_t *test_start = &(test_setup->start);

    // EXECUTE CONDITION
    uint32_t value = test_start->parameters.time_delay.value;
    if (time_get_timestamp() - _test_setup_timestamp[slot] > value)
    {
        _test_status[slot] = TEST_RUNNING;
        _test_start_timestamp[slot] = time_get_timestamp();

        // add first row
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
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
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
        _test_start_timestamp[slot] = time_get_timestamp();

        // check for first point at trigger
        bool need_first_point = (first_point == TAKEN_AT_TRIGGER) && (test_table->row_num == 0);
        if (need_first_point == true)
        {
            bool add_row_success = test_table_add_row(slot) == ESP_OK;
            if (!add_row_success)
            {
                ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
                return ESP_FAIL;
            }
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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
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
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
        _test_start_timestamp[slot] = time_get_timestamp();

        // check for first point at trigger
        bool need_first_point = (first_point == TAKEN_AT_TRIGGER) && (test_table->row_num == 0);
        if (need_first_point == true)
        {
            bool add_row_success = test_table_add_row(slot) == ESP_OK;
            if (!add_row_success)
            {
                ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
                return ESP_FAIL;
            }
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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
    test_stop_t *test_stop = &(test_setup->stop);
    test_inputs_t *test_input = &(test_setup->inputs);

    // test vars
    int sensor_index = test_stop->parameters.greater_than.sensor_index;
    double value = test_stop->parameters.greater_than.value;
    int tara_enable = test_input->tara_enable.byte;

    // execute condition
    bool condition_done = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1) > value;
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
    test_stop_t *test_stop = &(test_setup->stop);
    test_inputs_t *test_input = &(test_setup->inputs);

    // test vars
    int sensor_index = test_stop->parameters.less_than.sensor_index;
    double value = test_stop->parameters.less_than.value;
    int tara_enable = test_input->tara_enable.byte;

    // execute condition
    bool condition_done = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1) < value;
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];
    test_stop_t *test_stop = &(test_setup->stop);

    // test vars
    uint32_t value = test_stop->parameters.time_delay.value;

    // execute condition
    bool condition_done = (time_get_timestamp() - _test_start_timestamp[slot] > value);
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.distance_up.sensor_index;
    double value = test_stop->parameters.distance_up.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = (latest_value - first_value > value);
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.distance_down.sensor_index;
    double value = test_stop->parameters.distance_down.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = (first_value - latest_value > value);
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.percent_drop.sensor_index;
    double value = test_stop->parameters.percent_drop.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    double percent_drop = ((first_value - latest_value) / first_value) * 100.0;
    bool condition_done = (percent_drop > value);
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.percent_strain.sensor_index;
    double value = test_stop->parameters.percent_strain.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    double percent_strain = (fabs(latest_value - first_value) / first_value) * 100.0;
    bool condition_done = (percent_strain > value);
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_stop_t *test_stop = &(test_setup->stop);
    int sensor_index = test_stop->parameters.percent_stress_drop.sensor_index;
    double value = test_stop->parameters.percent_stress_drop.value;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    double percent_stress_drop = ((first_value - latest_value) / first_value) * 100.0;
    bool condition_done = (percent_stress_drop > value);
    if (condition_done)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

        if (_foreground_slot == slot)
        {
            current_test_plot_status(slot);
        }
    }
    return ESP_OK;
}

esp_err_t test_stop_condition_operator_stop(int slot)
{
    if (_operator_stop[slot] == true)
    {
        _test_status[slot] = TEST_DONE;
        _test_stop_timestamp[slot] = time_get_timestamp();

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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_logging_t *test_logging = &(test_setup->logging);
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    int row_num = test_table->row_num;

    int sensor_index = test_logging->parameters.interval_logging.sensor_index;
    logging_direction_t direction = test_logging->parameters.interval_logging.direction;
    double value = test_logging->parameters.interval_logging.value;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    double next_step = first_value + (row_num * value) * ((direction == LOGGING_DIRECTION_UP) ? 1.0 : -1.0);
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = ((direction == LOGGING_DIRECTION_UP) ? (latest_value > next_step) : (latest_value < next_step));
    if (condition_done)
    {
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_logging_t *test_logging = &(test_setup->logging);
    int row_num = test_table->row_num;

    uint32_t value = test_logging->parameters.linear_time_interval.value;

    // execute condition
    int64_t next_step = _test_start_timestamp[slot] + (row_num * value);
    bool condition_done = (time_get_timestamp() > next_step);
    if (condition_done)
    {
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_inputs_t *test_inputs = &(test_setup->inputs);
    test_logging_t *test_logging = &(test_setup->logging);
    test_row_t *first_row = test_table->first_row;
    double *data = (first_row == NULL) ? NULL : first_row->data;
    uint8_t input_enable = test_inputs->input_enable.byte;
    uint8_t tara_enable = test_inputs->tara_enable.byte;
    int row_num = test_table->row_num;

    int sensor_index = test_logging->parameters.interval_logging_table.sensor_index;
    logging_direction_t direction = test_logging->parameters.interval_logging_table.direction;
    double *value = test_logging->parameters.interval_logging_table.value;
    int size = test_logging->parameters.interval_logging_table.size;

    // execute condition
    // at least one row
    if (data == NULL)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // get sub index
    int data_index = -1;
    for (int i = 0; i <= sensor_index; i++)
    {
        bool input_enabled = (input_enable & 1) == 1;
        if (input_enabled)
        {
            data_index++;
        }
        input_enable >>= 1;
    }

    // verify sub index
    if (data_index < 0)
    {
        ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // verify condition
    double first_value = data[data_index];
    int value_index = (row_num - 1) % size;
    int cycles_num = (row_num - 1) / size;
    double next_step = first_value + (cycles_num * value[size - 1] + value[value_index]) * ((direction == LOGGING_DIRECTION_UP) ? 1.0 : -1.0);
    double latest_value = sensor_get_real(sensor_index, (tara_enable >> sensor_index) & 1);
    bool condition_done = ((direction == LOGGING_DIRECTION_UP) ? (latest_value > next_step) : (latest_value < next_step));
    if (condition_done)
    {
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
    test_setup_t *test_setup = _test_setup[slot];
    test_table_t *test_table = _test_table[slot];

    // test vars
    test_logging_t *test_logging = &(test_setup->logging);
    int row_num = test_table->row_num;

    test_time_t *value = test_logging->parameters.elapsed_time_table.value;
    int size = test_logging->parameters.elapsed_time_table.size;

    // execute condition
    int value_index = (row_num - 1) % size;
    int cycles_num = (row_num - 1) / size;
    double t = (value[size - 1].h * 3600.0) + (value[size - 1].m * 60.0) + (value[size - 1].s);
    double t1 = (value[value_index].h * 3600.0) + (value[value_index].m * 60.0) + (value[value_index].s);
    double next_step = (_test_start_timestamp[slot] * 1.0) + (cycles_num * t + t1);
    bool condition_done = (time_get_ftimestamp() > next_step);
    if (condition_done)
    {
        bool add_row_success = test_table_add_row(slot) == ESP_OK;
        if (!add_row_success)
        {
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            return ESP_FAIL;
        }
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
void test_init_storage(void)
{
    // init file system

    // init usb
}
void test_save_on_external_usb(int slot)
{
    assert(_test_status[slot] >= TEST_CONFIGURED);

    // save on usb
}
void test_save_on_internal_filesystem(int slot)
{
    assert(_test_status[slot] >= TEST_CONFIGURED);

    // save on internal filesystem
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
        if ((_test_status[i] == TEST_CONFIGURED) || (_test_status[i] == TEST_RUNNING))
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
            free_test_setup(_test_setup[i]);
            free_test_table(_test_table[i]);
            _test_status[i] = TEST_VOID;
        }
    }
}
void test_deinit_slot_configured(int slot)
{
    assert(_test_status[slot] >= TEST_CONFIGURED);

    // unlock sensors
    test_unlock_sensors(slot);

    // free slot and dynamic resources
    free_test_setup(_test_setup[slot]);
    free_test_table(_test_table[slot]);

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
        ESP_LOGE(TAG, "%s, line: %d", __FILE__, __LINE__);
        return ESP_FAIL;
    }

    // INITIALIZE TEST SETUP
    _test_setup[slot] = (test_setup_t *)malloc(sizeof(test_setup_t));
    if (_test_setup[slot] == NULL)
    {
        ESP_LOGE(TAG, "%s, line: %d", __FILE__, __LINE__);
        return ESP_ERR_NO_MEM;
    }

    _test_setup[slot]->type = TEST_TYPE_MAX;
    _test_setup[slot]->inputs.input_enable.byte = 0;
    _test_setup[slot]->inputs.tara_enable.byte = 0;
    _test_setup[slot]->logging.condition = LOGGING_CONDITION_MAX;
    _test_setup[slot]->stop.condition = STOP_CONDITION_MAX;
    _test_setup[slot]->graph.x_axis_option = AXIS_MAX;
    _test_setup[slot]->graph.y_axis_option = AXIS_MAX;
    _test_setup[slot]->start.condition = START_CONDITION_MAX;
    _test_setup[slot]->start.parameters.greater_than.sensor_index = SENSOR_1_INDEX;
    _test_setup[slot]->start.parameters.less_than.sensor_index = SENSOR_1_INDEX;
    memcpy(_test_setup[slot]->test_name, "NONAME", 7);

    // INITIALIZE DEFAULT PAGES
    _loggging_page = PAGE_NEW_TEST_P3_1;
    _stop_page = PAGE_NEW_TEST_P4_1;
    _start_page = PAGE_NEW_TEST_P7_1;

    // INITIALIZE TEST TABLE
    _test_table[slot] = (test_table_t *)malloc(sizeof(test_table_t));
    if (_test_table[slot] == NULL)
    {
        ESP_LOGE(TAG, "%s, line: %d", __FILE__, __LINE__);
        free_test_setup(_test_setup[slot]);
        return ESP_ERR_NO_MEM;
    }

    _test_table[slot]->first_row = NULL;
    _test_table[slot]->last_row = NULL;
    _test_table[slot]->row_num = 0;
    _test_table[slot]->peak_values = NULL;

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

    _test_setup_timestamp[_initialized_slot] = time_get_timestamp();
    // lock test sensors
    test_lock_sensors(_initialized_slot);

    // change status of initialized test to configured test
    // and set it as foreground
    _test_status[_initialized_slot] = TEST_CONFIGURED;
    _foreground_slot = _initialized_slot;
    _initialized_slot = CURRENT_TEST_NUM_MAX;

    // plot new foreground slot
    current_test_load_foreground_slot();
}

void test_run_slot(int slot)
{
    // END EXECUTION IF TEST NOT CONFIGURED
    bool test_is_unconfigured = ((_test_status[slot] <= TEST_INITIALIZED));
    if (test_is_unconfigured)
    {
        // ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
        return;
    }

    // LOOK FOR START CONDITION
    bool test_is_configured = (_test_status[slot] == TEST_CONFIGURED);
    if (test_is_configured)
    {
        switch (_test_setup[slot]->start.condition)
        {
        case START_CONDITION_TRIGGER_IMMEDIATELY:
            if (test_start_condition_trigger_immediately(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case START_CONDITION_TIME_DELAY:
            if (test_start_condition_time_delay(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case START_CONDITION_GREATER_THAN:
            if (test_start_condition_greater_than(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case START_CONDITION_LESS_THAN:
            if (test_start_condition_less_than(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        default:
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            test_force_stop(slot);
            goto end;
        }
        return;
    }

    // LOOK FOR STOP CONDITION and LOGGGING CONDITION
    bool test_is_running = (_test_status[slot] == TEST_RUNNING);
    if (test_is_running)
    {
        // LOGGING
        switch (_test_setup[slot]->logging.condition)
        {
        case LOGGING_CONDITION_INTERVAL_LOGGING:
            if (test_logging_condition_interval_logging(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case LOGGING_CONDITION_LINEAR_TIME_INTERVAL:
            if (test_logging_condition_linear_time_interval(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case LOGGING_CONDITION_INTERVAL_LOGGING_TABLE:
            if (test_logging_condition_interval_logging_table(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case LOGGING_CONDITION_ELAPSED_TIME_TABLE:
            if (test_logging_condition_elapsed_time_table(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        default:
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            test_force_stop(slot);
            goto end;
        }

        // STOP
        switch (_test_setup[slot]->stop.condition)
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
            if (test_stop_condition_distance_up(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case STOP_CONDITION_DISTANCE_DOWN:
            if (test_stop_condition_distance_down(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case STOP_CONDITION_PERCENT_DROP:
            if (test_stop_condition_percent_drop(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case STOP_CONDITION_PERCENT_STRAIN:
            if (test_stop_condition_percent_strain(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case STOP_CONDITION_PERCENT_STRESS_DROP:
            if (test_stop_condition_percent_stress_drop(slot) != ESP_OK)
            {
                test_force_stop(slot);
                goto end;
            }
            break;
        case STOP_CONDITION_OPERATOR_STOP:
            test_stop_condition_operator_stop(slot);
            break;
        default:
            ESP_LOGE(TAG, "%s: line %d", __FILE__, __LINE__);
            test_force_stop(slot);
            goto end;
        }
        return;
    }

end:
    // TEST STATUS : DONE
    test_save_on_external_usb(slot);
    test_save_on_internal_filesystem(slot);

    ESP_LOGW(TAG, "%s: line %d", __FILE__, __LINE__);
    test_deinit_slot_configured(slot);
}
void test_run_all(void)
{
    for (int i = 0; i < CURRENT_TEST_NUM_MAX; i++)
        test_run_slot(i);
}
