# ESP32 ESP-IDF and ESP8266 RTOS SDK component for BH1750 ambient light sensor

## Tested on

1. ESP8266 RTOS_SDK v3.4
2. ESP32 ESP-IDF v5.2

## Using

In an existing project, run the following command to install the component:

```text
cd ../your_project/components
git clone https://github.com/aZholtikov/zh_bh1750.git
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
    esp_log_level_set("zh_bh1750", ESP_LOG_NONE);
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
    zh_bh1750_init_config.i2c_port = I2C_PORT;
    zh_bh1750_init(&zh_bh1750_init_config);
    zh_bh1750_adjust(69); // Just for an example of how to change the sensor sensitivity.
    float lux = 0.0;
    for (;;)
    {
        zh_bh1750_read(&lux);
        printf("Lux %0.2f\n", lux);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

Any [feedback](mailto:github@azholtikov.ru) will be gladly accepted.
