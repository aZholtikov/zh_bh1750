#pragma once

#include "esp_err.h"
#include "driver/i2c.h"
#ifdef CONFIG_IDF_TARGET_ESP8266
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#define ZH_BH1750_INIT_CONFIG_DEFAULT()            \
    {                                              \
        .i2c_address = ZH_BH1750_I2C_ADDRESS_LOW,  \
        .operation_mode = ZH_HIGH_RESOLUTION_MODE, \
        .work_mode = ZH_ONE_TIME_WORK_MODE,        \
        .i2c_port = 0                              \
    }

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum zh_bh1750_operation_mode_t
    {
        ZH_LOW_RESOLUTION_MODE = 0, // Measurement with 4 lx resolution.
        ZH_HIGH_RESOLUTION_MODE,    // Measurement with 1 lx resolution.
        ZH_HIGH_RESOLUTION_MODE_2,  // Measurement with 0,5 lx resolution.
    } __attribute__((packed)) zh_bh1750_operation_mode_t;

    typedef enum zh_bh1750_work_mode_t
    {
        ZH_CONTINUOUSLY_WORK_MODE = 0, // Continuously measurement.
        ZH_ONE_TIME_WORK_MODE          // One time measurement. Sensor is power down after measurement.
    } __attribute__((packed)) zh_bh1750_work_mode_t;

    typedef enum zh_bh1750_i2c_address_t
    {
        ZH_BH1750_I2C_ADDRESS_HIGH = 0x5C, // If data pin is connected to VCC.
        ZH_BH1750_I2C_ADDRESS_LOW = 0x23   // If data pin is not connected / connected to GND.
    } __attribute__((packed)) zh_bh1750_i2c_address_t;

    typedef struct zh_bh1750_init_config_t
    {
        zh_bh1750_i2c_address_t i2c_address;       // One of two possible sensor addresses.
        zh_bh1750_operation_mode_t operation_mode; // Operation mode of the sensor.
        zh_bh1750_work_mode_t work_mode;           // Continuously or one time work mode.
        bool i2c_port;                             // Used I2C port. 0 or 1 (in accordance with used chip).
    } __attribute__((packed)) zh_bh1750_init_config_t;

    /**
     * @brief      Initialize BH1750 sensor.
     *
     * @param[in]  config  Pointer to BH1750 initialized configuration structure. Can point to a temporary variable.
     *
     * @return
     *              - ESP_OK always
     */
    esp_err_t zh_bh1750_init(zh_bh1750_init_config_t *config);

    /**
     * @brief      Read BH1750 sensor.
     *
     * @param[out]  data  Pointer for BH1750 sensor reading data.
     *
     * @return
     *              - ESP_OK if read was success
     *              - ESP_ERR_INVALID_ARG if parameter error
     *              - ESP_FAIL if sending command error or slave has not ACK the transfer
     *              - ESP_ERR_INVALID_STATE if I2C driver not installed or not in master mode
     *              - ESP_ERR_TIMEOUT if operation timeout because the bus is busy
     */
    esp_err_t zh_bh1750_read(float *data);

    /**
     * @brief      Adjust BH1750 sensor sensitivity.
     *
     * @param[in]  value  Value of sensitivity. Range value from 31 to 254.
     *
     * @return
     *              - ESP_OK if adjust was success
     *              - ESP_ERR_INVALID_ARG if parameter error
     *              - ESP_FAIL if sending command error or slave has not ACK the transfer
     *              - ESP_ERR_INVALID_STATE if I2C driver not installed or not in master mode
     *              - ESP_ERR_TIMEOUT if operation timeout because the bus is busy
     */
    esp_err_t zh_bh1750_adjust(uint8_t value);

#ifdef __cplusplus
}
#endif