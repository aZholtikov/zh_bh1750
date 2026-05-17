# ESP32 ESP-IDF component for BH1750 ambient light sensor

## Tested on

1. [ESP32 ESP-IDF v6.0.1](https://docs.espressif.com/projects/esp-idf/en/v6.0.1/esp32/index.html)

## SAST Tools

[PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.

## Features

1. Recorded illuminance values from 1 to 65535 lux.
2. Support some sensors on one device with [zh_pca9548a](https://github.com/aZholtikov/zh_pca9548a).

## Attention

For correct operation, please enable the following settings in the menuconfig:

```text
I2C_ISR_IRAM_SAFE
I2C_MASTER_ISR_HANDLER_IN_IRAM
```

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

zh_bh1750_handle_t bh1750_handle = {0};

void app_main(void)
{
    esp_log_level_set("zh_bh1750", ESP_LOG_ERROR);
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    zh_bh1750_init(&config, &bh1750_handle);
    float lux = 0.0;
    for (;;)
    {
        zh_bh1750_read(&bh1750_handle, &lux);
        printf("Lux %0.2f\n", lux);
        const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
        printf("Number of i2c driver error: %ld.\n", stats->i2c_driver_error);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

3 sensor on one device:

```c
#include "zh_pca9548a.h"
#include "zh_bh1750.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pca9548a_handle_t pca9548a_handle = {0};
zh_bh1750_handle_t bh1750_handle_chan_0 = {0};
zh_bh1750_handle_t bh1750_handle_chan_1 = {0};
zh_bh1750_handle_t bh1750_handle_chan_2 = {0};

void app_main(void)
{
    esp_log_level_set("zh_pca9548a", ESP_LOG_ERROR);
    esp_log_level_set("zh_bh1750", ESP_LOG_ERROR);
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    zh_pca9548a_init_config_t config = ZH_PCA9548A_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    config.i2c_address = 0x70;
    zh_pca9548a_init(&config, &pca9548a_handle);
    zh_bh1750_init_config_t bh1750_config = ZH_BH1750_INIT_CONFIG_DEFAULT();
    bh1750_config.i2c_handle = i2c_bus_handle;
    zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_0);
    zh_bh1750_init(&bh1750_config, &bh1750_handle_chan_0);
    zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_1);
    zh_bh1750_init(&bh1750_config, &bh1750_handle_chan_1);
    zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_2);
    zh_bh1750_init(&bh1750_config, &bh1750_handle_chan_2);
    float lux = 0.0;
    for (;;)
    {
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_0);
        zh_bh1750_read(&bh1750_handle_chan_0, &lux);
        printf("Sensor 1. Lux %0.2f\n", lux);
        lux = 0.0;
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_1);
        zh_bh1750_read(&bh1750_handle_chan_1, &lux);
        printf("Sensor 2. Lux %0.2f\n", lux);
        lux = 0.0;
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_2);
        zh_bh1750_read(&bh1750_handle_chan_2, &lux);
        printf("Sensor 3. Lux %0.2f\n", lux);
        lux = 0.0;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```
