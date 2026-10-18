# ESP32 ESP-IDF component for BH1750 ambient light sensor

## Features

1. Measurement of ambient light intensity in lux.
2. I2C interface with frequency up to 400 kHz (Fast Mode).
3. Support for two I2C addresses: 0x23 (ADDR low) and 0x5C (ADDR high).
4. One-time H-resolution measurement with 1 lux step.
5. Automatic lux conversion from raw sensor data.
6. Sensor presence verification during initialization via I2C probe.
7. Built-in I2C error statistics tracking and reset for diagnostics.

## Attention

1. The I2C bus must be initialized before the component.
2. Only one-time H-resolution measurement mode is supported.
3. Each read is a blocking call of approximately 180 ms.
4. Error statistics are global for all sensor instances on the device.

## Using

In an existing project, run the following command to install the component:

```bash
cd ../your_project/components
git clone https://github.com/aZholtikov/zh_bh1750
```

In the application, add the component:

```c
#include "zh_bh1750.h"
```

## Examples

### Basic Read Example

```c
#include "zh_bh1750.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_bh1750_handle_t *bh1750_handle = NULL;

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
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        printf("I2C bus initialization error\n");
        return;
    }
    zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    ret = zh_bh1750_init(&config, &bh1750_handle);
    if (ret != ESP_OK)
    {
        printf("BH1750 initialization error: %s\n", esp_err_to_name(ret));
        return;
    }
    float lux = 0.0;
    for (;;)
    {
        ret = zh_bh1750_read(&bh1750_handle, &lux);
        if (ret == ESP_OK)
        {
            printf("Lux: %0.2f\n", lux);
        }
        else
        {
            printf("Read error: %s\n", esp_err_to_name(ret));
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

---

### Alternative Address Example

```c
#include "zh_bh1750.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_bh1750_handle_t *bh1750_handle = NULL;

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
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        printf("I2C bus initialization error\n");
        return;
    }
    zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    config.i2c_address = 0x5C;
    ret = zh_bh1750_init(&config, &bh1750_handle);
    if (ret != ESP_OK)
    {
        printf("BH1750 initialization error: %s\n", esp_err_to_name(ret));
        return;
    }
    float lux = 0.0;
    ret = zh_bh1750_read(&bh1750_handle, &lux);
    if (ret == ESP_OK)
    {
        printf("Lux: %0.2f\n", lux);
    }
    else
    {
        printf("Read error: %s\n", esp_err_to_name(ret));
    }
    ret = zh_bh1750_deinit(&bh1750_handle);
    if (ret != ESP_OK)
    {
        printf("Deinitialization error: %s\n", esp_err_to_name(ret));
    }
}
```

---

### Error Statistics Example

```c
#include "zh_bh1750.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_bh1750_handle_t *bh1750_handle = NULL;

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
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        printf("I2C bus initialization error\n");
        return;
    }
    zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    ret = zh_bh1750_init(&config, &bh1750_handle);
    if (ret != ESP_OK)
    {
        printf("BH1750 initialization error: %s\n", esp_err_to_name(ret));
        return;
    }
    float lux = 0.0;
    for (int i = 0; i < 10; i++)
    {
        ret = zh_bh1750_read(&bh1750_handle, &lux);
        if (ret == ESP_OK)
        {
            printf("Lux: %0.2f\n", lux);
        }
        else
        {
            printf("Read error: %s\n", esp_err_to_name(ret));
        }
        const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
        printf("Number of I2C driver errors: %lu\n", (unsigned long)stats->i2c_driver_error);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    zh_bh1750_reset_stats();
    const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
    printf("Number of I2C driver errors after reset: %lu\n", (unsigned long)stats->i2c_driver_error);
    zh_bh1750_deinit(&bh1750_handle);
}
```
