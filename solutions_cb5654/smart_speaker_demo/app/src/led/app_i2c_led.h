#ifndef _APP_I2C_LED_H_
#include <stdio.h>

typedef enum {
    LED_ARRAY_1 = 0,
    LED_ARRAY_2,
    LED_ARRAY_3,
    LED_ARRAY_4,
    LED_ARRAY_5,
    LED_ARRAY_6,
    LED_ARRAY_7,
    LED_ARRAY_8,
    LED_ARRAY_9,
    LED_ARRAY_10,
    LED_ARRAY_11,
    LED_ARRAY_12
} led_array_id_t;

typedef enum {
    LED_RED,
    LED_GREEN,
    LED_BLUE,
} led_rgb_t;

typedef enum {
    LED_OFF,
    LED_LIGHT,          // 常亮
    LED_FLASH,          // 闪烁
    LED_BREATH,         // 呼吸
} led_mode_t;

int aw9523_init(aos_dev_t *dev);
int aw9523_shutdown(void);
int aw9523_led_control(aos_dev_t *dev, led_array_id_t index, uint8_t dime);
int aw9523_read_input(aos_dev_t *dev, uint16_t *val, uint16_t input_mask);

int aw2013_init(aos_dev_t *dev);
int aw2013_low_power(aos_dev_t *dev);
int aw2013_led_control(aos_dev_t *dev, led_rgb_t led, led_mode_t mode, uint8_t dime);

#endif
