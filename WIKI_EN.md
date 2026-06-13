# zh_bh1750 - BH1750 Ambient Light Sensor Component for ESP-IDF

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Technical Specifications](#technical-specifications)
- [Error Codes](#error-codes)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

`zh_bh1750` is a lightweight ESP-IDF component for the BH1750 ambient light sensor. It provides a simple API to read illuminance values from 1 to 65535 lux. The component supports multiple sensors on a single I2C bus using the [zh_pca9548a](https://github.com/aZholtikov/zh_pca9548a) I2C multiplexer.

The component is designed specifically for ESP32 microcontrollers and uses ESP-IDF v5.0+ I2C driver API.

---

## Features

1. **Illuminance Measurement**: Reads illuminance values from 1 to 65535 lux
2. **I2C Interface**: Uses standard I2C protocol (400 kHz max frequency)
3. **Configurable I2C Address**: Supports both 0x23 and 0x5C addresses
4. **Error Statistics**: Built-in error counter for I2C driver errors
5. **Low Power**: Works with ESP-IDF power management
6. **Thread-Safe**: Uses ESP-IDF I2C driver (thread-safe)
7. **Minimal Overhead**: Low memory and CPU overhead
8. **Multiple Sensor Support**: Compatible with I2C multiplexer (zh_pca9548a)

---

## Installation

1. Navigate to your project's components directory:

```bash
cd ../your_project/components
```

2. Clone the repository:

```bash
git clone https://github.com/aZholtikov/zh_bh1750.git
```

3. In your application, include the header:

```c
#include "zh_bh1750.h"
```

4. The component will be automatically built with your project.

### Optional: Using with I2C Multiplexer (zh_pca9548a)

To use multiple BH1750 sensors on the same I2C bus, also install the [zh_pca9548a](https://github.com/aZholtikov/zh_pca9548a) component.

---

## API Reference

### zh_bh1750_init_config_t Structure

```c
typedef struct
{
    i2c_master_bus_handle_t i2c_handle; // Unique I2C bus handle
    uint8_t i2c_address;                // Sensor I2C address (0x23 or 0x5C)
    uint32_t i2c_frequency;             // Sensor I2C frequency (max 400000 Hz)
} zh_bh1750_init_config_t;
```

Use `ZH_BH1750_INIT_CONFIG_DEFAULT()` macro to initialize with default values:

- `i2c_frequency`: 400000 Hz
- `i2c_address`: 0x23

---

### zh_bh1750_handle_t Structure

```c
typedef struct
{
    bool is_initialized;                // Sensor initialization flag
    i2c_master_dev_handle_t dev_handle; // Unique I2C device handle
} zh_bh1750_handle_t;
```

---

### zh_bh1750_stats_t Structure

```c
typedef struct
{
    uint32_t i2c_driver_error; // Number of I2C driver errors
} zh_bh1750_stats_t;
```

---

### zh_bh1750_init()

Initializes the BH1750 sensor.

**Parameters:**

- `config` - Pointer to BH1750 initialization configuration structure
- `handle` - Pointer to unique BH1750 handle

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL config or handle)
- `ESP_FAIL` - Initialization failed (configuration check or I2C device addition)

**Example:**

```c
zh_bh1750_handle_t bh1750_handle = {0};
zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT();
config.i2c_handle = i2c_bus_handle;
zh_bh1750_init(&config, &bh1750_handle);
```

---

### zh_bh1750_deinit()

Deinitializes the BH1750 sensor and removes it from the I2C bus.

**Parameters:**

- `handle` - Pointer to unique BH1750 handle

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_ERR_INVALID_STATE` - Sensor not initialized
- `ESP_FAIL` - I2C device removal failed

---

### zh_bh1750_read()

Reads illuminance value from the sensor.

**Parameters:**

- `handle` - Pointer to unique BH1750 handle
- `data` - Pointer to store the illuminance value (in lux)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle or data)
- `ESP_ERR_NOT_FOUND` - Sensor not initialized
- `ESP_FAIL` - I2C communication error

**Note:** The function performs measurement and waits ~180ms for data ready.

---

### zh_bh1750_get_stats()

Gets error statistics since last reset.

**Returns:**

- Pointer to the statistics structure

**Example:**

```c
const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
printf("I2C errors: %ld\n", stats->i2c_driver_error);
```

---

### zh_bh1750_reset_stats()

Resets error statistics counter.

**Example:**

```c
zh_bh1750_reset_stats();
```

---

## Usage Examples

### Basic Example: Single Sensor

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
        printf("Lux: %.2f\n", lux);
        const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
        printf("I2C errors: %ld\n", stats->i2c_driver_error);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

---

### Multiple Sensors: Using I2C Multiplexer (zh_pca9548a)

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
    // Initialize I2C multiplexer
    zh_pca9548a_init_config_t pca_config = ZH_PCA9548A_INIT_CONFIG_DEFAULT();
    pca_config.i2c_handle = i2c_bus_handle;
    pca_config.i2c_address = 0x70;
    zh_pca9548a_init(&pca_config, &pca9548a_handle);
    // Initialize BH1750 sensors on different channels
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
        // Read sensor on channel 0
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_0);
        zh_bh1750_read(&bh1750_handle_chan_0, &lux);
        printf("Sensor 1. Lux: %.2f\n", lux);
        // Read sensor on channel 1
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_1);
        zh_bh1750_read(&bh1750_handle_chan_1, &lux);
        printf("Sensor 2. Lux: %.2f\n", lux);
        // Read sensor on channel 2
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_2);
        zh_bh1750_read(&bh1750_handle_chan_2, &lux);
        printf("Sensor 3. Lux: %.2f\n", lux);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

---

## Technical Specifications

| Parameter | Value |
|-----------|-------|
| **Illuminance Range** | 1 - 65535 lux |
| **Measurement Resolution** | 1 lux |
| **I2C Address** | 0x23, 0x5C |
| **I2C Frequency** | Up to 400 kHz |
| **Measurement Time** | ~180 ms |
| **ESP-IDF Version** | >= 5.0 |
| **Platform** | ESP32 series |
| **Language** | C (C99) |

---

## Error Codes

| Error Code | Description |
|------------|-------------|
| `ESP_OK` | Operation successful |
| `ESP_ERR_INVALID_ARG` | Invalid argument (NULL pointer or invalid configuration) |
| `ESP_ERR_INVALID_STATE` | Sensor not initialized |
| `ESP_ERR_NOT_FOUND` | Sensor not initialized or not responding |
| `ESP_FAIL` | General failure (I2C communication error) |

---

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please ensure your code follows the existing style and includes appropriate documentation.

---

## License

This project is licensed under the Apache License, Version 2.0 - see the [LICENSE](LICENSE) file for details.

### Apache License, Version 2.0

Copyright (c) 2026 Alexey Zholtikov

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

---

## Additional Notes

- **I2C Pull-up Resistors**: Ensure proper pull-up resistors are connected to SDA and SCL lines
- **Measurement Time**: The sensor requires ~180ms for each measurement
- **I2C_ISR_IRAM_SAFE**: For correct operation, enable `I2C_ISR_IRAM_SAFE` and `I2C_MASTER_ISR_HANDLER_IN_IRAM` in menuconfig
- **Thread Safety**: The component uses ESP-IDF I2C driver which is thread-safe

---

*Generated for zh_bh1750 v1.0.0*
