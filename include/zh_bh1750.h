#pragma once

#include "esp_err.h"
#include "esp_log.h"
#ifdef CONFIG_IDF_TARGET_ESP8266
#include "driver/i2c.h"
#else
#include "driver/i2c_master.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ZH_BH1750_INIT_CONFIG_DEFAULT()    \
    {                                      \
        .i2c_address = I2C_ADDRESS_LOW,    \
        .operation_mode = HIGH_RESOLUTION, \
        .work_mode = ONE_TIME,             \
        .i2c_port = 0,                     \
        .auto_adjust = false               \
    }

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct // Structure for initial initialization of BH1750 sensor.
    {
        enum // Sensor I2C address.
        {
            I2C_ADDRESS_HIGH = 0x5C, // Data pin is connected to VCC.
            I2C_ADDRESS_LOW = 0x23   // Data pin is not connected / connected to GND.
        } i2c_address;
        enum // Operation mode.
        {
            LOW_RESOLUTION,    // Measurement with 4 lx resolution.
            HIGH_RESOLUTION,   // Measurement with 1 lx resolution.
            HIGH_RESOLUTION_2, // Measurement with 0,5 lx resolution.
        } operation_mode;
        enum // Work mode.
        {
            CONTINUOUSLY, // Continuously measurement.
            ONE_TIME      // One time measurement. Sensor is power down after measurement.
        } work_mode;
        bool i2c_port;    // I2C port.
        bool auto_adjust; // Flag of automatic sensitivity adjustment.
#ifndef CONFIG_IDF_TARGET_ESP8266
        i2c_master_bus_handle_t i2c_handle; // Unique I2C bus handle.
#endif

    } zh_bh1750_init_config_t;

    /**
     * @brief Initialize BH1750 sensor.
     *
     * @param[in] config Pointer to BH1750 initialized configuration structure. Can point to a temporary variable.
     *
     * @attention I2C driver must be initialized first.
     *
     * @note Before initialize the sensor recommend initialize zh_bh1750_init_config_t structure with default values.
     *
     * @code zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT() @endcode
     *
     * @return
     *              - ESP_OK if initialization was success
     *              - ESP_ERR_INVALID_ARG if parameter error
     *              - ESP_ERR_NOT_FOUND if sensor not connected or not responded
     */
    esp_err_t zh_bh1750_init(const zh_bh1750_init_config_t *config);

    /**
     * @brief Read BH1750 sensor.
     *
     * @param[out] data Pointer for BH1750 sensor reading data.
     *
     * @return
     *              - ESP_OK if read was success
     *              - ESP_ERR_INVALID_ARG if parameter error
     *              - ESP_ERR_NOT_FOUND if sensor is not initialized
     *              - ESP_ERR_INVALID_RESPONSE if I2C driver error
     */
    esp_err_t zh_bh1750_read(float *data);

    /**
     * @brief Adjust BH1750 sensor sensitivity.
     * 
     * @attention Can only be used if the automatic adjustment is not switched on.
     *
     * @param[in] value Value of sensitivity.
     *
     * @note Range value from 31 to 254. 69 by default. Can be changed continuously during program work.
     *
     * @return
     *              - ESP_OK if adjust was success
     *              - ESP_ERR_INVALID_ARG if parameter error
     *              - ESP_ERR_NOT_FOUND if sensor is not initialized or auto adjust is enabled
     *              - ESP_ERR_INVALID_RESPONSE if I2C driver error
     */
    esp_err_t zh_bh1750_adjust(const uint8_t value);

#ifdef __cplusplus
}
#endif