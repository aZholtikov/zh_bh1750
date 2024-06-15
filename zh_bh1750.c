#include "zh_bh1750.h"

#define LOW_RESOLUTION_MEASUREMENT_TIME 24   // L-Resolution mode measurement time max value.
#define HIGH_RESOLUTION_MEASUREMENT_TIME 180 // H-Resolution mode measurement time max value.

#define SENSITIVITY_DEFAULT 69

#define MEASUREMENT_TIME_HIGH_BIT 0x40 // Change measurement time (high bit).
#define MEASUREMENT_TIME_LOW_BIT 0x60  // Change measurement time (low bit).

#define CONTINUOUSLY_LOW_RESOLUTION 0x13    // Start measurement at 4 lx resolution.
#define CONTINUOUSLY_HIGH_RESOLUTION 0x10   // Start measurement at 1 lx resolution.
#define CONTINUOUSLY_HIGH_RESOLUTION_2 0x11 // Start measurement at 0.5 lx resolution.
#define ONE_TIME_LOW_RESOLUTION 0x23        // Start measurement at 4 lx resolution. It is automatically set to power down mode after measurement.
#define ONE_TIME_HIGH_RESOLUTION 0x20       // Start measurement at 1 lx resolution. It is automatically set to power down mode after measurement.
#define ONE_TIME_HIGH_RESOLUTION_2 0x21     // Start measurement at 0.5 lx resolution. It is automatically set to power down mode after measurement.

static zh_bh1750_init_config_t _init_config = {0};
static uint8_t _command = 0;
static bool _is_first_start = false;
static uint8_t _time = 0;
static uint8_t _sensivity = SENSITIVITY_DEFAULT;
static bool _is_initialized = false;

static const char *TAG = "zh_bh1750";

esp_err_t zh_bh1750_init(zh_bh1750_init_config_t *config)
{
    ESP_LOGI(TAG, "BH1750 initialization begin.");
    if (config == NULL)
    {
        ESP_LOGE(TAG, "BH1750 initialization fail. Invalid argument.");
        return ESP_ERR_INVALID_ARG;
    }
    _init_config = *config;
    switch (_init_config.operation_mode)
    {
    case LOW_RESOLUTION:;
        _command = (_init_config.work_mode == CONTINUOUSLY) ? CONTINUOUSLY_LOW_RESOLUTION : ONE_TIME_LOW_RESOLUTION;
        _time = LOW_RESOLUTION_MEASUREMENT_TIME;
        break;
    case HIGH_RESOLUTION:;
        _command = (_init_config.work_mode == CONTINUOUSLY) ? CONTINUOUSLY_HIGH_RESOLUTION : ONE_TIME_HIGH_RESOLUTION;
        _time = HIGH_RESOLUTION_MEASUREMENT_TIME;
        break;
    case HIGH_RESOLUTION_2:;
        _command = (_init_config.work_mode == CONTINUOUSLY) ? CONTINUOUSLY_HIGH_RESOLUTION_2 : ONE_TIME_HIGH_RESOLUTION_2;
        _time = HIGH_RESOLUTION_MEASUREMENT_TIME;
        break;
    default:
        break;
    }
    ESP_LOGI(TAG, "BH1750 initialization success.");
    _is_initialized = true;
    return ESP_OK;
}

esp_err_t zh_bh1750_read(float *data)
{
    ESP_LOGI(TAG, "BH1750 read begin.");
    if (data == NULL)
    {
        ESP_LOGE(TAG, "BH1750 read fail. Invalid argument.");
        return ESP_ERR_INVALID_ARG;
    }
    if (_is_initialized == false)
    {
        ESP_LOGE(TAG, "BH1750 read fail. BH1750 not initialized.");
        return ESP_ERR_NOT_FOUND;
    }
    esp_err_t esp_err = ESP_OK;
    uint8_t sensor_data_high = 0;
    uint8_t sensor_data_low = 0;
    if (_init_config.work_mode == CONTINUOUSLY)
    {
        if (_is_first_start == true)
        {
            goto READ;
        }
        else
        {
            _is_first_start = true;
        }
    }
    i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, _init_config.i2c_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(i2c_cmd_handle, _command, true);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "BH1750 read fail. I2C driver error.");
        return esp_err;
    }
    vTaskDelay(((_sensivity / SENSITIVITY_DEFAULT) * _time) / portTICK_PERIOD_MS);
READ:
    i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, _init_config.i2c_address << 1 | I2C_MASTER_READ, true);
    i2c_master_read_byte(i2c_cmd_handle, &sensor_data_high, I2C_MASTER_ACK);
    i2c_master_read_byte(i2c_cmd_handle, &sensor_data_low, I2C_MASTER_NACK);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "BH1750 read fail. I2C driver error.");
        return esp_err;
    }
    if (_init_config.operation_mode == HIGH_RESOLUTION_2)
    {
        *data = (sensor_data_high << 8 | sensor_data_low) * (1 / 1.2 * (69.0 / _sensivity) / 2);
    }
    else
    {
        *data = (sensor_data_high << 8 | sensor_data_low) * (1 / 1.2 * (69.0 / _sensivity));
    }
    ESP_LOGI(TAG, "BH1750 read success.");
    return ESP_OK;
}

esp_err_t zh_bh1750_adjust(uint8_t value)
{
    ESP_LOGI(TAG, "BH1750 adjust begin.");
    if ((value < 31 || value > 254))
    {
        ESP_LOGE(TAG, "BH1750 adjust fail. Invalid argument.");
        return ESP_ERR_INVALID_ARG;
    }
    if (_is_initialized == false)
    {
        ESP_LOGE(TAG, "BH1750 read fail. BH1750 not initialized.");
        return ESP_ERR_NOT_FOUND;
    }
    esp_err_t esp_err = ESP_OK;
    _sensivity = value;
    i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, _init_config.i2c_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(i2c_cmd_handle, value >> 5 | MEASUREMENT_TIME_HIGH_BIT, true);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "BH1750 adjust fail. I2C driver error.");
        return esp_err;
    }
    i2c_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_cmd_handle);
    i2c_master_write_byte(i2c_cmd_handle, _init_config.i2c_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(i2c_cmd_handle, (value & 0b00011111) | MEASUREMENT_TIME_LOW_BIT, true);
    i2c_master_stop(i2c_cmd_handle);
    esp_err = i2c_master_cmd_begin(_init_config.i2c_port, i2c_cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(i2c_cmd_handle);
    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "BH1750 adjust fail. I2C driver error.");
        return esp_err;
    }
    if (_init_config.work_mode == CONTINUOUSLY)
    {
        _is_first_start = false;
    }
    ESP_LOGI(TAG, "BH1750 adjust success.");
    return ESP_OK;
}