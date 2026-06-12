# zh_bh1750 - Компонент датчика освещенности BH1750 для ESP-IDF

## Содержание

- [Обзор](#обзор)
- [Возможности](#возможности)
- [Установка](#установка)
- [Справочник API](#справочник-api)
- [Примеры использования](#примеры-использования)
- [Технические характеристики](#технические-характеристики)
- [Коды ошибок](#коды-ошибок)
- [Вклад в проект](#вклад-в-проект)
- [Лицензия](#лицензия)

---

## Обзор

`zh_bh1750` - это легковесный компонент ESP-IDF для датчика освещенности BH1750. Он предоставляет простой API для считывания значений освещенности от 1 до 65535 люкс. Компонент поддерживает несколько датчиков на одной шине I2C с использованием I2C мультиплексатора [zh_pca9548a](https://github.com/aZholtikov/zh_pca9548a).

Компонент разработан специально для микроконтроллеров ESP32 и использует API драйвера I2C ESP-IDF v5.0+.

---

## Возможности

1. **Измерение освещенности**: Считывание значений освещенности от 1 до 65535 люкс
2. **Интерфейс I2C**: Использует стандартный протокол I2C (макс. частота 400 кГц)
3. **Настраиваемый адрес I2C**: Поддерживает адреса 0x23 и 0x5C
4. **Статистика ошибок**: Встроенный счетчик ошибок драйвера I2C
5. **Низкое энергопотребление**: Работает с системой управления питанием ESP-IDF
6. **Потокобезопасность**: Использует драйвер I2C ESP-IDF (потокобезопасен)
7. **Минимальные накладные расходы**: Низкие накладные расходы на память и процессор
8. **Поддержка нескольких датчиков**: Совместим с I2C мультиплексатором (zh_pca9548a)

---

## Установка

1. Перейдите в каталог компонентов вашего проекта:

```bash
cd ../ваш_проект/components
```

2. Клонируйте репозиторий:

```bash
git clone https://github.com/aZholtikov/zh_bh1750.git
```

3. В вашем приложении подключите заголовочный файл:

```c
#include "zh_bh1750.h"
```

4. Компонент будет автоматически собран вместе с вашим проектом.

### Опционально: Использование с I2C мультиплексатором (zh_pca9548a)

Для использования нескольких датчиков BH1750 на одной шине I2C, также установите компонент [zh_pca9548a](https://github.com/aZholtikov/zh_pca9548a).

---

## Справочник API

### Структура zh_bh1750_init_config_t

```c
typedef struct
{
    i2c_master_bus_handle_t i2c_handle; // Уникальный дескриптор шины I2C
    uint8_t i2c_address;                // Адрес устройства датчика I2C (0x23 или 0x5C)
    uint32_t i2c_frequency;             // Частота I2C датчика (макс. 400000 Гц)
} zh_bh1750_init_config_t;
```

Используйте макрос `ZH_BH1750_INIT_CONFIG_DEFAULT()` для инициализации значениями по умолчанию:

- `i2c_frequency`: 400000 Гц
- `i2c_address`: 0x23

---

### Структура zh_bh1750_handle_t

```c
typedef struct
{
    bool is_initialized;                // Флаг инициализации датчика
    i2c_master_dev_handle_t dev_handle; // Уникальный дескриптор устройства I2C
} zh_bh1750_handle_t;
```

---

### Структура zh_bh1750_stats_t

```c
typedef struct
{
    uint32_t i2c_driver_error; // Количество ошибок драйвера I2C
} zh_bh1750_stats_t;
```

---

### zh_bh1750_init()

Инициализирует датчик BH1750.

**Параметры:**

- `config` - Указатель на структуру конфигурации инициализации BH1750
- `handle` - Указатель на уникальный дескриптор BH1750

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL config или handle)
- `ESP_FAIL` - Ошибка инициализации (проверка конфигурации или добавление устройства I2C)

**Пример:**

```c
zh_bh1750_handle_t bh1750_handle = {0};
zh_bh1750_init_config_t config = ZH_BH1750_INIT_CONFIG_DEFAULT();
config.i2c_handle = i2c_bus_handle;
zh_bh1750_init(&config, &bh1750_handle);
```

---

### zh_bh1750_deinit()

Деинициализирует датчик BH1750 и удаляет его из шины I2C.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор BH1750

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle)
- `ESP_ERR_INVALID_STATE` - Датчик не инициализирован
- `ESP_FAIL` - Ошибка удаления устройства I2C

---

### zh_bh1750_read()

Считывает значение освещенности с датчика.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор BH1750
- `data` - Указатель для хранения значения освещенности (в люксах)

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle или data)
- `ESP_ERR_NOT_FOUND` - Датчик не инициализирован
- `ESP_FAIL` - Ошибка связи I2C

**Примечание:** Функция выполняет измерение и ожидает ~180 мс готовности данных.

---

### zh_bh1750_get_stats()

Получает статистику ошибок с момента последнего сброса.

**Возвращает:**

- Указатель на структуру статистики

**Пример:**

```c
const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
printf("Ошибки I2C: %ld\n", stats->i2c_driver_error);
```

---

### zh_bh1750_reset_stats()

Сбрасывает счетчик статистики ошибок.

**Пример:**

```c
zh_bh1750_reset_stats();
```

---

## Примеры использования

### Базовый пример: Один датчик

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
        printf("Освещенность: %.2f люкс\n", lux);
        
        const zh_bh1750_stats_t *stats = zh_bh1750_get_stats();
        printf("Ошибки I2C: %ld\n", stats->i2c_driver_error);
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

---

### Несколько датчиков: Использование I2C мультиплексатора (zh_pca9548a)

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
    
    // Инициализация I2C мультиплексатора
    zh_pca9548a_init_config_t pca_config = ZH_PCA9548A_INIT_CONFIG_DEFAULT();
    pca_config.i2c_handle = i2c_bus_handle;
    pca_config.i2c_address = 0x70;
    zh_pca9548a_init(&pca_config, &pca9548a_handle);
    
    // Инициализация датчиков BH1750 на разных каналах
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
        // Считывание с датчика на канале 0
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_0);
        zh_bh1750_read(&bh1750_handle_chan_0, &lux);
        printf("Датчик 1. Освещенность: %.2f люкс\n", lux);
        
        // Считывание с датчика на канале 1
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_1);
        zh_bh1750_read(&bh1750_handle_chan_1, &lux);
        printf("Датчик 2. Освещенность: %.2f люкс\n", lux);
        
        // Считывание с датчика на канале 2
        zh_pca9548a_set(&pca9548a_handle, ZH_PCA9548A_CHAN_NUM_2);
        zh_bh1750_read(&bh1750_handle_chan_2, &lux);
        printf("Датчик 3. Освещенность: %.2f люкс\n", lux);
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

---

## Технические характеристики

| Параметр | Значение |
|----------|----------|
| **Диапазон освещенности** | 1 - 65535 люкс |
| **Разрешение измерения** | 1 люкс |
| **Адрес I2C** | 0x23, 0x5C |
| **Частота I2C** | До 400 кГц |
| **Время измерения** | ~180 мс |
| **Версия ESP-IDF** | >= 5.0 |
| **Платформа** | Семейство ESP32 |
| **Язык** | C (C99) |

---

## Коды ошибок

| Код ошибки | Описание |
|------------|----------|
| `ESP_OK` | Операция выполнена успешно |
| `ESP_ERR_INVALID_ARG` | Неверный аргумент (NULL указатель или неверная конфигурация) |
| `ESP_ERR_INVALID_STATE` | Датчик не инициализирован |
| `ESP_ERR_NOT_FOUND` | Датчик не инициализирован или не отвечает |
| `ESP_FAIL` | Общая ошибка (ошибка связи I2C) |

---

## Вклад в проект

Вклад приветствуется! Чтобы внести свой вклад:

1. Сделайте форк репозитория
2. Создайте ветку функции (`git checkout -b feature/AmazingFeature`)
3. Закоммитьте ваши изменения (`git commit -m 'Add some AmazingFeature'`)
4. Отправьте в ветку (`git push origin feature/AmazingFeature`)
5. Откройте Pull Request

Пожалуйста, убедитесь, что ваш код следует существующему стилю и включает соответствующую документацию.

---

## Лицензия

Этот проект лицензирован по лицензии Apache, версия 2.0 - см. файл [LICENSE](LICENSE) для подробной информации.

### Apache License, Version 2.0

Авторское право (c) 2026 Алексей Жолтиков

Лицензировано по лицензии Apache License, Version 2.0 (далее — "Лицензия");
вы не можете использовать этот файл, кроме случаев, предусмотренных Лицензией.
Копию Лицензии можно получить по адресу:

    http://www.apache.org/licenses/LICENSE-2.0

Если иное не требуется действующим законодательством или не согласовано в письменном виде,
программное обеспечение, распространяемое по Лицензии, распространяется на условиях "КАК ЕСТЬ",
БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, явных или подразумеваемых, включая, но не ограничиваясь, гарантии
ТОВАРНОГО СОСТОЯНИЯ, ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ ЦЕЛИ И НЕНАРУШЕНИЯ ПРАВ.
Смотрите Лицензию для получения конкретных прав и ограничений.

---

## Дополнительные заметки

- **Подтягивающие резисторы I2C**: Убедитесь, что к линиям SDA и SCL подключены соответствующие подтягивающие резисторы
- **Время измерения**: Датчик требует ~180 мс для каждого измерения
- **I2C_ISR_IRAM_SAFE**: Для правильной работы включите `I2C_ISR_IRAM_SAFE` и `I2C_MASTER_ISR_HANDLER_IN_IRAM` в menuconfig
- **Потокобезопасность**: Компонент использует драйвер I2C ESP-IDF, который является потокобезопасным

---

*Сгенерировано для zh_bh1750 v1.0.0*
