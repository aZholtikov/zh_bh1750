#include "zh_bh1750.h"

#define ZH_LOW_RESOLUTION_MEASUREMENT_TIME 24
#define ZH_HIGH_RESOLUTION_MEASUREMENT_TIME 180

#define ZH_SENSITIVITY_DEFAULT 69

#define ZH_MEASUREMENT_TIME_HIGH_BIT 0x40
#define ZH_MEASUREMENT_TIME_LOW_BIT 0x60

#define ZH_CONTINUOUSLY_LOW_RESOLUTION_MODE 0x13
#define ZH_CONTINUOUSLY_HIGH_RESOLUTION_MODE 0x10
#define ZH_CONTINUOUSLY_HIGH_RESOLUTION_MODE_2 0x11
#define ZH_ONE_TIME_LOW_RESOLUTION_MODE 0x23
#define ZH_ONE_TIME_HIGH_RESOLUTION_MODE 0x20
#define ZH_ONE_TIME_HIGH_RESOLUTION_MODE_2 0x21

static zh_bh1750_init_config_t s_zh_bh1750_init_config = {0};
static uint8_t s_i2c_command = 0;
static bool s_is_continuously_mode_first_start = false;
static uint8_t s_measurement_time = 0;
static uint8_t s_sensivity = ZH_SENSITIVITY_DEFAULT;

esp_err_t zh_bh1750_init(zh_bh1750_init_config_t *config)
{
    if (config == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    s_zh_bh1750_init_config = *config;
    switch (s_zh_bh1750_init_config.operation_mode)
    {
    case ZH_LOW_RESOLUTION_MODE:;
        s_i2c_command = (s_zh_bh1750_init_config.work_mode == ZH_CONTINUOUSLY_WORK_MODE) ? ZH_CONTINUOUSLY_LOW_RESOLUTION_MODE : ZH_ONE_TIME_LOW_RESOLUTION_MODE;
        s_measurement_time = ZH_LOW_RESOLUTION_MEASUREMENT_TIME;
        break;
    case ZH_HIGH_RESOLUTION_MODE:;
        s_i2c_command = (s_zh_bh1750_init_config.work_mode == ZH_CONTINUOUSLY_WORK_MODE) ? ZH_CONTINUOUSLY_HIGH_RESOLUTION_MODE : ZH_ONE_TIME_HIGH_RESOLUTION_MODE;
        s_measurement_time = ZH_HIGH_RESOLUTION_MEASUREMENT_TIME;
        break;
    case ZH_HIGH_RESOLUTION_MODE_2:;
        s_i2c_command = (s_zh_bh1750_init_config.work_mode == ZH_CONTINUOUSLY_WORK_MODE) ? ZH_CONTINUOUSLY_HIGH_RESOLUTION_MODE_2 : ZH_ONE_TIME_HIGH_RESOLUTION_MODE_2;
        s_measurement_time = ZH_HIGH_RESOLUTION_MEASUREMENT_TIME;
        break;
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t zh_bh1750_read(float *data)
{
    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t esp_err = ESP_OK;
    uint8_t sensor_data_high = 0;
    uint8_t sensor_data_low = 0;
    if (s_zh_bh1750_init_config.work_mode == ZH_CONTINUOUSLY_WORK_MODE)
    {
        if (s_is_continuously_mode_first_start == true)
        {
            goto ZH_BH1750_READ;
        }
        else
        {
            s_is_continuously_mode_first_start = true;
        }
    }
    i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, s_zh_bh1750_init_config.i2c_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(i2c_cmd_handle, s_i2c_command, true);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(s_zh_bh1750_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (esp_err != ESP_OK)
    {
        goto ZH_BH1750_READ_EXIT;
    }
    vTaskDelay(s_measurement_time / portTICK_PERIOD_MS);
ZH_BH1750_READ:
    i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, s_zh_bh1750_init_config.i2c_address << 1 | I2C_MASTER_READ, true);
    i2c_master_read_byte(i2c_cmd_handle, &sensor_data_high, I2C_MASTER_ACK);
    i2c_master_read_byte(i2c_cmd_handle, &sensor_data_low, I2C_MASTER_NACK);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(s_zh_bh1750_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (s_zh_bh1750_init_config.operation_mode == ZH_HIGH_RESOLUTION_MODE_2)
    {
        *data = (sensor_data_high << 8 | sensor_data_low) * (1 / 1.2 * (69.0 / s_sensivity) / 2);
    }
    else
    {
        *data = (sensor_data_high << 8 | sensor_data_low) * (1 / 1.2 * (69.0 / s_sensivity));
    }
ZH_BH1750_READ_EXIT:
    return esp_err;
}

esp_err_t zh_bh1750_adjust(uint8_t value)
{
    if ((value < 31 || value > 254))
    {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t esp_err = ESP_OK;
    s_sensivity = value;
    i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, s_zh_bh1750_init_config.i2c_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(i2c_cmd_handle, value >> 5 | ZH_MEASUREMENT_TIME_HIGH_BIT, true);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(s_zh_bh1750_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (esp_err != ESP_OK)
    {
        goto ZH_BH1750_ADJUST_EXIT;
    }
    i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, s_zh_bh1750_init_config.i2c_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(i2c_cmd_handle, (value & 0b00011111) | ZH_MEASUREMENT_TIME_LOW_BIT, true);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(s_zh_bh1750_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (s_zh_bh1750_init_config.work_mode == ZH_CONTINUOUSLY_WORK_MODE)
    {
        s_is_continuously_mode_first_start = false;
    }
ZH_BH1750_ADJUST_EXIT:
    return esp_err;
}