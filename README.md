# ESP32 ESP-IDF and ESP8266 RTOS SDK component for BH1750 Ambient Light Sensor

## Tested on

1. ESP8266 RTOS_SDK v3.4
2. ESP32 ESP-IDF v5.1.0

## Initial settings

1. i2c_address - One of two possible sensor addresses.
      1. ZH_BH1750_I2C_ADDRESS_HIGH - If data pin is connected to VCC.
      2. ZH_BH1750_I2C_ADDRESS_LOW - If data pin is not connected / connected to GND.
2. operation_mode - Operation mode of the sensor.
      1. ZH_LOW_RESOLUTION_MODE - Measurement with 4 lx resolution. Range of lux detected values see below.
      2. ZH_HIGH_RESOLUTION_MODE - Measurement with 1 lx resolution. Range of lux detected values see below.
      3. ZH_HIGH_RESOLUTION_MODE_2 - Measurement with 0,5 lx resolution. Range of lux detected values see below.
3. work_mode - Continuously or one time work mode.
      1. ZH_CONTINUOUSLY_WORK_MODE - Continuously measurement.
      2. ZH_ONE_TIME_WORK_MODE - One time measurement. Sensor is power down after measurement.
4. i2c_port - Used I2C port. 0 or 1 (in accordance with used chip).

## Changing sensitivity

Sensor sensitivity and accordingly range of lux detected values can be changed continuously during program work. Range of sensitivity values from 31 to 254 with step 1. 69 by default.

1. ZH_LOW_RESOLUTION_MODE
      1. Sensitivity is 31 - Range of lux detected values from 1,85 to 121239,75.
      2. Sensitivity is 69 - Range of lux detected values from 0,83 to 54394,05.
      3. Sensitivity is 254 - Range of lux detected values from 0,23 to 15073,05.
2. ZH_HIGH_RESOLUTION_MODE
      1. Sensitivity is 31 - Range of lux detected values from 1,85 to 121239,75.
      2. Sensitivity is 69 - Range of lux detected values from 0,83 to 54394,05.
      3. Sensitivity is 254 - Range of lux detected values from 0,23 to 15073,05.
3. ZH_HIGH_RESOLUTION_MODE_2
      1. Sensitivity is 31 - Range of lux detected values from 0,93 to 60947,55.
      2. Sensitivity is 69 - Range of lux detected values from 0,42 to 27524,7.
      3. Sensitivity is 254 - Range of lux detected values from 0,11 to 7208,85.

## Using

In an existing project, run the following command to install the component:

```text
cd ../your_project/components
git clone http://git.zh.com.ru/alexey.zholtikov/zh_bh1750.git
```

In the application, add the component:

```c
#include "zh_bh1750.h"
```

## Example

Reading the sensor:

```c
#include "zh_bh1750.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

void app_main(void)
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21, // In accordance with used chip.
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = GPIO_NUM_22, // In accordance with used chip.
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
#ifndef CONFIG_IDF_TARGET_ESP8266
        .master.clk_speed = 100000,
#endif
    };
#ifdef CONFIG_IDF_TARGET_ESP8266
    i2c_param_config(I2C_PORT, &i2c_config);
    i2c_driver_install(I2C_PORT, i2c_config.mode);
#else
    i2c_param_config(I2C_PORT, &i2c_config);
    i2c_driver_install(I2C_PORT, i2c_config.mode, 0, 0, 0);
#endif
    zh_bh1750_init_config_t zh_bh1750_init_config = ZH_BH1750_INIT_CONFIG_DEFAULT();
    zh_bh1750_init_config.i2c_port = I2C_PORT; // Just for an example of how to change the default values.
    zh_bh1750_init(&zh_bh1750_init_config);
    zh_bh1750_adjust(69); // Just for an example of how to change the sensor sensitivity.
    float lux = 0;
    for (;;)
    {
        zh_bh1750_read(&lux);
        printf("Lux %f\n", lux);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

Any [feedback](mailto:github@azholtikov.ru) will be gladly accepted.
