#include "new_test.h"
#include <string.h>
#include "project_defines.h"

#define TAG "NEW_TEST"
#define str(x) #x

static test_setup_t *_test_setup = NULL;

esp_err_t new_test_init()
{
    if (_test_setup != NULL)
        free(_test_setup);

    _test_setup = malloc(sizeof(test_setup_t));

    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memset(_test_setup, 0, sizeof(test_setup_t));

    return ESP_OK;
}
esp_err_t new_test_deinit()
{
    if (_test_setup != NULL)
        free(_test_setup);

    return ESP_OK;
}
esp_err_t new_test_set_type(test_type_t *type)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    _test_setup->type = *type;
    new_test_print_type();

    return ESP_OK;
}
esp_err_t new_test_set_inputs(test_inputs_t *inputs)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup->inputs), inputs, sizeof(test_inputs_t));
    new_test_print_inputs();

    return ESP_OK;
}
esp_err_t new_test_set_logging(test_logging_t *logging)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup->logging), logging, sizeof(test_logging_t));
    new_test_print_logging();

    return ESP_OK;
}
esp_err_t new_test_set_stop(test_stop_t *stop)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup->stop), stop, sizeof(test_stop_t));
    new_test_print_stop();

    return ESP_OK;
}
esp_err_t new_test_set_motor(test_motor_t *motor)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup->motor), motor, sizeof(test_motor_t));
    new_test_print_motor();

    return ESP_OK;
}
esp_err_t new_test_set_graph(test_graph_t *graph)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup->graph), graph, sizeof(test_graph_t));
    new_test_print_graph();

    return ESP_OK;
}
esp_err_t new_test_set_start(test_start_t *start)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    memcpy(&(_test_setup->start), start, sizeof(test_start_t));
    new_test_print_start();

    return ESP_OK;
}
esp_err_t new_test_move_setup(test_setup_t **test_setup)
{
    *test_setup = _test_setup;
    _test_setup = NULL;
    ESP_LOGI(TAG, "%s", __FUNCTION__);
    return ESP_OK;
}

void new_test_print_type(void)
{
    switch (_test_setup->type)
    {
    case TEST_TYPE_CBR:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_CBR));
        break;
    case TEST_TYPE_LBR:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_LBR));
        break;
    case TEST_TYPE_MARSHALL:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_MARSHALL));
        break;
    case TEST_TYPE_TSR:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_TSR));
        break;
    case TEST_TYPE_SCB_ASHTO:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_SCB_ASHTO));
        break;
    case TEST_TYPE_SCB_ASTM:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_SCB_ASTM));
        break;
    case TEST_TYPE_USER_DEFINED:
        ESP_LOGI(TAG, "  TYPE: %s", str(TEST_TYPE_USER_DEFINED));
        break;
    default:
        ESP_LOGI(TAG, "  TYPE: %d", _test_setup->type);
        break;
    }
}
void new_test_print_inputs(void)
{
    ESP_LOGI(TAG, "  INPUTS: [1 YES, 0 NO]");
    ESP_LOGI(TAG, "     0- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s0, _test_setup->inputs.tara_enable.s0);
    ESP_LOGI(TAG, "     1- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s1, _test_setup->inputs.tara_enable.s1);
    ESP_LOGI(TAG, "     2- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s2, _test_setup->inputs.tara_enable.s2);
    ESP_LOGI(TAG, "     3- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s3, _test_setup->inputs.tara_enable.s3);
    ESP_LOGI(TAG, "     4- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s4, _test_setup->inputs.tara_enable.s4);
    ESP_LOGI(TAG, "     5- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s5, _test_setup->inputs.tara_enable.s5);
    ESP_LOGI(TAG, "     6- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s6, _test_setup->inputs.tara_enable.s6);
    ESP_LOGI(TAG, "     7- input_enabled: %d, tara_enabled: %d", _test_setup->inputs.input_enable.s7, _test_setup->inputs.tara_enable.s7);
}
void new_test_print_logging(void)
{
    ESP_LOGI(TAG, "  LOGGING:");
    switch (_test_setup->logging.condition)
    {
    case LOGGING_CONDITION_INTERVAL_LOGGING:
        ESP_LOGI(TAG, "  condition: INTERVAL_LOGGING");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->logging.parameters.interval_logging.sensor_index);
        ESP_LOGI(TAG, "  direction: %d [0 UP, 1 DOWN]", _test_setup->logging.parameters.interval_logging.direction);
        ESP_LOGI(TAG, "  value: %lf (unit)", _test_setup->logging.parameters.interval_logging.value);
        break;
    case LOGGING_CONDITION_LINEAR_TIME_INTERVAL:
        ESP_LOGI(TAG, "  condition: LINEAR_TIME_INTERVAL");
        ESP_LOGI(TAG, "  value: %lu (s)", _test_setup->logging.parameters.linear_time_interval.value);
        break;
    case LOGGING_CONDITION_INTERVAL_LOGGING_TABLE:
        ESP_LOGI(TAG, "  condition: INTERVAL_LOGGING_TABLE");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->logging.parameters.interval_logging_table.sensor_index);
        ESP_LOGI(TAG, "  direction: %d [0 UP, 1 DOWN]", _test_setup->logging.parameters.interval_logging_table.direction);
        ESP_LOGI(TAG, "  value:");
        for (int i = 0; i < 30; i++)
        {
            ESP_LOGI(TAG, "     [%i]=%lf (unit)", i, _test_setup->logging.parameters.interval_logging_table.value[i]);
        }
        break;
    case LOGGING_CONDITION_ELAPSED_TIME_TABLE:
        ESP_LOGI(TAG, "  condition: ELAPSED_TIME_TABLE");
        ESP_LOGI(TAG, "  value:");
        for (int i = 0; i < 30; i++)
        {
            logging_time_row_t *value = &(_test_setup->logging.parameters.elapsed_time_table.value[i]);
            ESP_LOGI(TAG, "     [%i]=> h=%d, m=%d, s=%.2f", i, value->h, value->m, value->s);
        }
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup->logging.condition);
        break;
    }
}
void new_test_print_stop(void)
{
    ESP_LOGI(TAG, "  STOP:");
    switch (_test_setup->stop.condition)
    {
    case STOP_CONDITION_GREATER_THAN:
        ESP_LOGI(TAG, "  condition: GREATER_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.greater_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.greater_than.value);
        break;
    case STOP_CONDITION_LESS_THAN:
        ESP_LOGI(TAG, "  condition: LESS_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.less_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.less_than.value);
        break;
    case STOP_CONDITION_TIME_DELAY:
        ESP_LOGI(TAG, "  condition: TIME_DELAY");
        ESP_LOGI(TAG, "  value: %lu", _test_setup->stop.parameters.time_delay.value);
        break;
    case STOP_CONDITION_DISTANCE_UP:
        ESP_LOGI(TAG, "  condition: DISTANCE_UP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.distance_up.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.distance_up.value);
        break;
    case STOP_CONDITION_DISTANCE_DOWN:
        ESP_LOGI(TAG, "  condition: DISTANCE_DOWN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.distance_down.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.distance_down.value);
        break;
    case STOP_CONDITION_PERCENT_DROP:
        ESP_LOGI(TAG, "  condition: PERCENT_DROP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.percent_drop.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.percent_drop.value);
        break;
    case STOP_CONDITION_PERCENT_STRAIN:
        ESP_LOGI(TAG, "  condition: PERCENT_STRAIN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.percent_strain.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.percent_strain.value);
        break;
    case STOP_CONDITION_PERCENT_STRESS_DROP:
        ESP_LOGI(TAG, "  condition: PERCENT_STRESS_DROP");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->stop.parameters.percent_stress_drop.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->stop.parameters.percent_stress_drop.value);
        break;
    case STOP_CONDITION_OPERATOR_STOP:
        ESP_LOGI(TAG, "  condition: OPERATOR_STOP");
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup->stop.condition);
        break;
    }
}
void new_test_print_motor(void)
{
}
void new_test_print_graph(void)
{
    switch (_test_setup->graph.x_axis_option)
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
        ESP_LOGI(TAG, "  GRAPH: X_AXIS %d", _test_setup->graph.x_axis_option);
        break;
    }

    switch (_test_setup->graph.y_axis_option)
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
        ESP_LOGI(TAG, "  GRAPH: Y_AXIS %d", _test_setup->graph.y_axis_option);
        break;
    }
}
void new_test_print_start(void)
{
    ESP_LOGI(TAG, "  START:");
    switch (_test_setup->start.condition)
    {
    case START_CONDITION_TRIGGER_IMMEDIATELY:
        ESP_LOGI(TAG, "  condition: TRIGGER_IMMEDIATELY");
        break;
    case START_CONDITION_TIME_DELAY:
        ESP_LOGI(TAG, "  condition: TIME_DELAY");
        ESP_LOGI(TAG, "  value: %lu", _test_setup->start.parameters.time_delay.value);
        break;
    case START_CONDITION_GREATER_THAN:
        ESP_LOGI(TAG, "  condition: GREATER_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->start.parameters.greater_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->start.parameters.greater_than.value);
        ESP_LOGI(TAG, "  current_value: %lf", _test_setup->start.parameters.greater_than.current_value);
        ESP_LOGI(TAG, "  first_point: %d [0 AT TRIGGER, 1 AT INITIAL VALUE]", _test_setup->start.parameters.greater_than.first_point_taken_at);
        break;
    case START_CONDITION_LESS_THAN:
        ESP_LOGI(TAG, "  condition: LESS_THAN");
        ESP_LOGI(TAG, "  sensor_index: %d", _test_setup->start.parameters.less_than.sensor_index);
        ESP_LOGI(TAG, "  value: %lf", _test_setup->start.parameters.less_than.value);
        ESP_LOGI(TAG, "  current_value: %lf", _test_setup->start.parameters.less_than.current_value);
        ESP_LOGI(TAG, "  first_point: %d [0 AT TRIGGER, 1 AT INITIAL VALUE]", _test_setup->start.parameters.less_than.first_point_taken_at);
        break;
    default:
        ESP_LOGI(TAG, "  condition: %d", _test_setup->logging.condition);
        break;
    }
}
esp_err_t new_test_get_sensor_index_from_combobox(int combobox_val, uint8_t *sensor_index)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    uint16_t input_enable = _test_setup->inputs.input_enable.byte;
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
    return ESP_FAIL;
}

esp_err_t new_test_get_logging_direction_from_combobox(int combobox_val, logging_direction_t *direction)
{
    if (_test_setup == NULL)
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
        return ESP_FAIL;
    }
}
esp_err_t new_test_get_graph_option_from_combobox(int combobox_val, axis_option_t *axis_option)
{
    if (_test_setup == NULL)
        return ESP_ERR_NO_MEM;

    if ((combobox_val >= 0) && (combobox_val <= 3))
    {
        *axis_option = (axis_option_t)(combobox_val - 4);
        return ESP_OK;
    }
    else if ((combobox_val >= 4) && (combobox_val <= 11))
    {
        combobox_val -= 4;
        uint16_t input_enable = _test_setup->inputs.input_enable.byte;
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
        return ESP_FAIL;
    }
    else
    {
        return ESP_FAIL;
    }
}
esp_err_t new_test_get_start_first_point_from_multiradio(int multiradio_val, first_point_taken_at_t *first_point)
{
    if (multiradio_val == 0)
    {
        *first_point = FIRST_POINT_TAKEN_AT_TRIGGER;
        return ESP_OK;
    }
    else if (multiradio_val == 1)
    {
        *first_point = FIRST_POINT_TAKEN_AT_INITIAL_VALUE;
        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
}
esp_err_t new_test_start_GreaterThan_set_sensor_index(int sensor_index)
{
    ESP_LOGI(TAG, "Greater than, set sensor_index: %u", sensor_index);
    _test_setup->start.parameters.greater_than.sensor_index = sensor_index;
    return ESP_OK;
}
esp_err_t new_test_start_GreaterThan_get_sensor_index(int *sensor_index)
{
    *sensor_index = _test_setup->start.parameters.greater_than.sensor_index;
    return ESP_OK;
}
esp_err_t new_test_start_LessThan_set_sensor_index(int sensor_index)
{
    ESP_LOGI(TAG, "Less than, set sensor_index: %u", sensor_index);
    _test_setup->start.parameters.less_than.sensor_index = sensor_index;
    return ESP_OK;
}
esp_err_t new_test_start_LessThan_get_sensor_index(int *sensor_index)
{
    *sensor_index = _test_setup->start.parameters.less_than.sensor_index;
    return ESP_OK;
}
esp_err_t new_test_get_type_from_multiradio(int multiradio_val, test_type_t *test_type)
{
    if ((multiradio_val >= TEST_TYPE_CBR) && (multiradio_val <= TEST_TYPE_SCB_ASTM))
    {
        *test_type = (test_type_t)multiradio_val;
        return ESP_OK;
    }
    return ESP_FAIL;
}
