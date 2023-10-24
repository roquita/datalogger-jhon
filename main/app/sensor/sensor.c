#include "sensor.h"
#include "project_defines.h"
#include "project_tasks.h"
#include "project_typedefs.h"
#include "string.h"

sensor_dev_t sensor[NUM_SENSORS];
char buffer[100] = {0};

void sensor_init(int index)
{
    sensor[index].calib = SENSOR_CALIB_DEFAULT;
    sensor[index].config = SENSOR_CONFIG_DEFAULT;
    sensor[index].data = SENSOR_DATA_DEFAULT;
    // sensor[index].class = CLASS_MAX;
    sensor[index].is_free = true;
    sensor[index].is_calibrated = false;
}
void sensor_set_class(int index, sensor_class_t class)
{
    sensor[index].class = class;
}
sensor_class_t sensor_get_class(int index)
{
    return sensor[index].class;
}
void sensor_enable(int index)
{
    sensor[index].config.is_sensor_enabled = true;
}
void sensor_disable(int index)
{
    sensor[index].config.is_sensor_enabled = false;
}
bool sensor_is_enabled(int index)
{
    return sensor[index].config.is_sensor_enabled;
}
void sensor_set_tara(int index)
{
    sensor[index].config.tara_val = sensor[index].data.real;
}
double sensor_get_tara(int index)
{
    return sensor[index].config.tara_val;
}
void sensor_clear_tara(int index)
{
    sensor[index].config.tara_val = 0.0;
}
double sensor_get_real(int index, bool use_tara)
{
    sensor_class_t class = sensor[index].class;
    sensor_dev_t *s = sensor + index;
    double tara = use_tara ? s->config.tara_val : 0.0;
    if (class == CLASS_D)
        return (s->data.real - tara) * s->config.unit_fc;
    else
        return s->data.real;
}
char *sensor_get_real_str(int index, bool use_tara)
{
    int num_decimals = sensor_get_num_decimals(index);
    double real = sensor_get_real(index, use_tara);
    snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, real);
    return buffer;
}
int32_t sensor_get_raw(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].data.raw;
    else
        return 0;
}
char *sensor_get_raw_str(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
    {
        int32_t raw = sensor[index].data.raw;
        snprintf(buffer, sizeof(buffer), "%li", raw);
        return buffer;
    }
    else
    {
        return "- - -";
    }
}
double sensor_get_rawps(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].data.rawps;
    else
        return 0.0;
}
double sensor_get_realps(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].data.realps * sensor[index].config.unit_fc;
    else
        return sensor[index].data.realps;
}
char *sensor_get_realps_str(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
    {
        int num_decimals = sensor[index].config.num_decimals;
        double realps = sensor[index].data.realps;
        snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, realps);
        return buffer;
    }
    else
    {
        int num_decimals = sensor[index].data.num_decimals;
        double realps = sensor[index].data.realps;
        snprintf(buffer, sizeof(buffer), "%.*lf", num_decimals, realps);
        return buffer;
    }
}
bool sensor_is_calibrated(int index)
{
    return sensor[index].is_calibrated;
}
bool sensor_is_ready(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
    {
        bool is_enabled = sensor_is_enabled(index);
        bool is_calibrated = sensor_is_calibrated(index);
        return is_enabled && is_calibrated;
    }
    else
    {
        bool is_enabled = sensor_is_enabled(index);
        return is_enabled;
    }
}
bool sensor_is_free(int index)
{
    return sensor[index].is_free;
}
void sensor_set_calibration(int index, sensor_calib_t *calib)
{
    // sensor[index].calib = *calib;
    memcpy(&(sensor[index].calib), calib, sizeof(sensor_calib_t));
    sensor[index].is_calibrated = true;
}
void sensor_set_configuration(int index, sensor_config_t *config)
{
    memcpy(&sensor[index].config, config, sizeof(sensor_config_t));
    // sensor[index].config = *config;
}
sensor_config_t *sensor_get_configuration(int index)
{
    return &(sensor[index].config);
}
double sensor_get_capacity(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
    {
        double capacity = sensor[index].calib.capacity;
        double unit_fc = sensor[index].config.unit_fc;
        return capacity * unit_fc;
    }
    else
    {
        double capacity = sensor[index].config.capacity;
        double unit_fc = sensor[index].config.unit_fc;
        return capacity * unit_fc;
    }
}
int sensor_get_num_decimals(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].config.num_decimals;
    else
        return sensor[index].data.num_decimals;
}
sensor_unit_t sensor_get_unit(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].config.unit;
    else
        return sensor[index].data.unit_src;
}
sensor_unit_t sensor_get_src_unit(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].calib.unit_src;
    else
        return sensor[index].config.capacity_unit;
}
char *sensor_get_unit_str(int index)
{
    sensor_unit_t unit = sensor_get_unit(index);
    switch (unit)
    {
    case DISPLACEMENT_UNIT_CM:
        return "cm";
    case DISPLACEMENT_UNIT_IN:
        return "in";
    case DISPLACEMENT_UNIT_MM:
        return "mm";
    case LOAD_UNIT_KN:
        return "kN";
    case LOAD_UNIT_LBF:
        return "lbf";
    case LOAD_UNIT_N:
        return "N";
    case LOAD_UNIT_KGF:
        return "kgf";
    case PRESSURE_UNIT_KPA:
        return "kPa";
    case PRESSURE_UNIT_PSI:
        return "psi";
    case PRESSURE_UNIT_KSF:
        return "ksf";
    case PRESSURE_UNIT_MPA:
        return "MPa";
    case PRESSURE_UNIT_KGF_CM2:
        return "kgf/cm2";
    case VOLUME_UNIT_CM3:
        return "cm3";
    case VOLUME_UNIT_IN3:
        return "in3";
    default:
        return "N.A.";
    }
}
char *sensor_get_unitps_str(int index)
{
    sensor_unit_t unit = sensor_get_unit(index);
    switch (unit)
    {
    case DISPLACEMENT_UNIT_CM:
        return "cm/s";
    case DISPLACEMENT_UNIT_IN:
        return "in/s";
    case DISPLACEMENT_UNIT_MM:
        return "mm/s";
    case LOAD_UNIT_KN:
        return "kN/s";
    case LOAD_UNIT_LBF:
        return "lbf/s";
    case LOAD_UNIT_N:
        return "N/s";
    case LOAD_UNIT_KGF:
        return "kgf/s";
    case PRESSURE_UNIT_KPA:
        return "kPa/s";
    case PRESSURE_UNIT_PSI:
        return "psi/s";
    case PRESSURE_UNIT_KSF:
        return "ksf/s";
    case PRESSURE_UNIT_MPA:
        return "MPa/s";
    case PRESSURE_UNIT_KGF_CM2:
        return "(kgf/cm2)/s";
    case VOLUME_UNIT_CM3:
        return "cm3/s";
    case VOLUME_UNIT_IN3:
        return "in3/s";
    default:
        return "N.A.";
    }
}
static double calculate_real(int index, int32_t raw)
{
    // variable
    double y = 0.0;
    double x = (double)raw;
    calib_points_num_t num_points = sensor[index].calib.num_points;
    double(*table)[2] = sensor[index].calib.table;

    // get total points
    int total_points;
    if (num_points == NUM_POINTS_1)
        total_points = 1;
    else if (num_points == NUM_POINTS_5)
        total_points = 5;
    else
        total_points = 10;

    // if x got lower than x_min
    double x_min = table[0][1];
    bool out_of_range_lower = (x < x_min);
    if (out_of_range_lower)
    {
        double y1 = table[1][0];
        double y0 = table[0][0];
        double x1 = table[1][1];
        double x0 = table[0][1];
        double m = (y1 - y0) / (x1 - x0);
        double delta_x = x - x0;
        y = y0 + m * delta_x;
        y = (y < 0.0) ? 0.0 : y;
        goto end;
    }

    // if x got higher than x_max
    double x_max = table[total_points][1];
    bool out_of_range_upper = (x > x_max);
    if (out_of_range_upper)
    {
        double y1 = table[total_points][0];
        double y0 = table[total_points - 1][0];
        double x1 = table[total_points][1];
        double x0 = table[total_points - 1][1];
        double m = (y1 - y0) / (x1 - x0);
        double delta_x = x - x0;
        y = y0 + m * delta_x;
        y = (y < 0.0) ? 0.0 : y;
        goto end;
    }

    // if x got in range
    for (int i = 0; i < total_points; i++)
    {
        double x1 = table[i + 1][1];
        double x0 = table[i][1];
        if ((x > x0) && (x < x1))
        {
            double y1 = table[i + 1][0];
            double y0 = table[i][0];
            double m = (y1 - y0) / (x1 - x0);
            double delta_x = x - x0;
            y = y0 + m * delta_x;
            y = (y < 0.0) ? 0.0 : y;
            goto end;
        }
    }

end:
    return y;
}

static void sensor_set_data_for_class_D(int index, raw_t *r)
{
    sensor_dev_t *s = sensor + index;

    // GET RAW PER SECOND
    int32_t ms = r->tickcount;
    int32_t raw = r->value;
    int32_t last_raw = s->data.raw;
    int32_t last_ms = s->data.tickcount;
    int32_t delta_raw = raw - last_raw;
    int32_t delta_ms = ms - last_ms;
    double rawps = (delta_raw * 1000.0) / delta_ms;

    // SAVE VALUES
    s->data.raw = raw;
    s->data.tickcount = ms;
    s->data.rawps = rawps;

    if (s->is_calibrated)
    {
        // GET REAL and REALPS
        double last_real = s->data.real;
        double real = calculate_real(index, raw);
        double delta_real = real - last_real;
        double realps = (delta_real * 1000.0) / delta_ms;

        // SAVE VALUES
        s->data.real = real;
        s->data.realps = realps;
    }
}
static void sensor_set_data_for_class_I(int index, real_t *r)
{
    sensor_dev_t *s = sensor + index;

    // PRIOR DATA
    int32_t last_ms = s->data.tickcount;
    double last_real = s->data.real;
    sensor_unit_t last_unit = s->data.unit_src;

    // CURRENT DATA
    int32_t ms = r->tickcount;
    double real = r->value;
    sensor_unit_t unit = r->unit;

    // GET EVENT UNIT CHANGED
    bool unit_changed = (last_unit != unit);

    // GET REAL PER SECOND
    double delta_real = real - last_real;
    int32_t deltat_ms = ms - last_ms;
    double realps = (delta_real * 1000.0) / deltat_ms;

    // SAVE VALUES
    s->data.real = real;
    s->data.realps = realps;
    s->data.tickcount = ms;
    s->data.unit_src = unit;
    s->data.num_decimals = r->num_decimals;
    s->data.num_ints = r->num_ints;

    // TRANSMIT EVENT
    if (unit_changed)
    {
        uint8_t *str = (uint8_t *)malloc(1);
        *str = unit;
        msg_t msg;
        msg.type = SENSOR_UNIT_CHANGED;
        msg.content.cs.index = index;
        msg.content.cs.str = (char *)str;
        xQueueSend(main_queue, &msg, portMAX_DELAY);
    }
}

void sensor_set_data(int index, reading_t *reading)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        sensor_set_data_for_class_D(index, &(reading->raw));
    else
        sensor_set_data_for_class_I(index, &(reading->real));
}
char *sensor_get_name(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].calib.name;
    else
        return sensor[index].config.name;
}
char *sensor_get_range(int index)
{
    int num_decimals = sensor_get_num_decimals(index);
    snprintf(buffer, sizeof(buffer), "0.0-%.*lf %s", num_decimals, sensor_get_capacity(index), sensor_get_unit_str(index));
    return buffer;
}
char *sensor_get_units_path(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
    {
        sensor_type_t type = sensor[index].calib.sensor_type;
        switch (type)
        {
        case DISPLACEMENT:
            return "cm\r\nin\r\nmm";
        case LOAD:
            return "kN\r\nlbf\r\nN\r\nkgf";
        case PRESSURE:
            return "kPa\r\npsi\r\nksf\r\nMPa\r\nkgf/cm2";
        case VOLUME:
            return "cm3\r\nin3";
        default:
            return "";
        }
    }
    else
    {
        return "";
    }
}
int32_t sensor_get_units_val(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
    {
        sensor_unit_t unit = sensor_get_unit(index);
        switch (unit)
        {
        case DISPLACEMENT_UNIT_CM:
            return 0;
        case DISPLACEMENT_UNIT_IN:
            return 1;
        case DISPLACEMENT_UNIT_MM:
            return 2;
        case LOAD_UNIT_KN:
            return 0;
        case LOAD_UNIT_LBF:
            return 1;
        case LOAD_UNIT_N:
            return 2;
        case LOAD_UNIT_KGF:
            return 3;
        case PRESSURE_UNIT_KPA:
            return 0;
        case PRESSURE_UNIT_PSI:
            return 1;
        case PRESSURE_UNIT_KSF:
            return 2;
        case PRESSURE_UNIT_MPA:
            return 3;
        case PRESSURE_UNIT_KGF_CM2:
            return 4;
        case VOLUME_UNIT_CM3:
            return 0;
        case VOLUME_UNIT_IN3:
            return 1;
        default:
            return 255;
        }
    }
    else
    {
        return 0;
    }
}
bool sensor_is_limit_enabled(int index)
{
    return sensor[index].config.is_limit_enabled;
}

sensor_type_t sensor_get_type(int index)
{
    sensor_class_t class = sensor[index].class;
    if (class == CLASS_D)
        return sensor[index].calib.sensor_type;
    else
        return sensor[index].config.sensor_type;
}
void sensor_set_limits(int index, bool limits)
{
    sensor[index].config.is_limit_enabled = limits;
}
