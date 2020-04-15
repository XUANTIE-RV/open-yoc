/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drv/gpio.h>
#include <pinmux.h>
#include <devices/hal/led_impl.h>

#define LED_GPIO_SET(h, v, f) do { if (h) csi_gpio_pin_write((h), ((v) ^ (f)));}while(0)

typedef struct led_dev {
    aos_dev_t             device;
    gpio_pin_handle_t led_r_pin_handle;
    gpio_pin_handle_t led_g_pin_handle;
    gpio_pin_handle_t led_b_pin_handle;
    aos_timer_t       timer;
    int               color;
    int               on_time;
    int               off_time;
    int               blink;
    int               ctrl_req;
} led_dev_t;

static void yoc_led_delay_action(void *timer, void *args);

static aos_dev_t *yoc_led_rgb_init(driver_t *drv, void *config, int id)
{
    led_dev_t *dev     = (led_dev_t *)device_new(drv, sizeof(led_dev_t), id);

    if (dev)
        dev->device.config = config;

    return (aos_dev_t *)dev;
}

#define yoc_led_rgb_uninit device_free

static int yoc_led_rgb_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static void yoc_led_timer_trigger(aos_dev_t *dev)
{
    led_dev_t *led = (led_dev_t *)dev;
    int        delay_time;

    delay_time = (led->blink == BLINK_ON) ? led->on_time : led->off_time;

    if (delay_time <= 0 && led->ctrl_req) {
        delay_time = 100;
    }
    aos_timer_stop(&led->timer);
    if (delay_time > 0){
        aos_timer_change(&led->timer, delay_time);
        aos_timer_start(&led->timer);
    }
}

static void yoc_led_delay_action(void *timer, void *args)
{
    led_dev_t *       led        = (led_dev_t *)args;
    led_pin_config_t *led_config = (led_pin_config_t *)led->device.config;

    if ( led->on_time > 0 && led->off_time > 0 ) {
        /* 初始化BLINK_ON，所以闪烁的请求从 BLINK_OFF */
        led->blink = (led->blink == BLINK_ON) ? BLINK_OFF : BLINK_ON;
    }

    if (led->blink == BLINK_ON) {
        LED_GPIO_SET(led->led_r_pin_handle, COLOR_RED_GET(led->color) ? 1 : 0, led_config->flip);
        LED_GPIO_SET(led->led_g_pin_handle, COLOR_GREEN_GET(led->color) ? 1 : 0, led_config->flip);
        LED_GPIO_SET(led->led_b_pin_handle, COLOR_BLUE_GET(led->color) ? 1 : 0, led_config->flip);
    } else {
        LED_GPIO_SET(led->led_r_pin_handle, 0, led_config->flip);
        LED_GPIO_SET(led->led_g_pin_handle, 0, led_config->flip);
        LED_GPIO_SET(led->led_b_pin_handle, 0, led_config->flip);
    }

    led->ctrl_req = 0;
    yoc_led_timer_trigger((aos_dev_t *)led);
}

static int yoc_led_rgb_config(aos_dev_t *dev, uint32_t r_pin, uint32_t g_pin, uint32_t b_pin)
{
    led_dev_t *       led        = (led_dev_t *)dev;
    led_pin_config_t *led_config = (led_pin_config_t *)led->device.config;
    int               ret        = 0;

    led->led_r_pin_handle = NULL;
    led->led_g_pin_handle = NULL;
    led->led_b_pin_handle = NULL;

    if (r_pin != LED_PIN_NOT_SET) {
        drv_pinmux_config(r_pin, PIN_FUNC_GPIO);
        led->led_r_pin_handle = csi_gpio_pin_initialize(r_pin, NULL);
        csi_gpio_pin_config(led->led_r_pin_handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
        LED_GPIO_SET(led->led_r_pin_handle, COLOR_RED_GET(led->color) ? 1 : 0, led_config->flip);
    }

    if (g_pin != LED_PIN_NOT_SET) {
        drv_pinmux_config(g_pin, PIN_FUNC_GPIO);
        led->led_g_pin_handle = csi_gpio_pin_initialize(g_pin, NULL);
        csi_gpio_pin_config(led->led_g_pin_handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
        LED_GPIO_SET(led->led_g_pin_handle, COLOR_GREEN_GET(led->color) ? 1 : 0, led_config->flip);
    }

    if (b_pin != LED_PIN_NOT_SET) {
        drv_pinmux_config(b_pin, PIN_FUNC_GPIO);
        led->led_b_pin_handle = csi_gpio_pin_initialize(b_pin, NULL);
        csi_gpio_pin_config(led->led_b_pin_handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
        LED_GPIO_SET(led->led_b_pin_handle, COLOR_BLUE_GET(led->color) ? 1 : 0, led_config->flip);
    }

    ret = aos_timer_new_ext(&led->timer, yoc_led_delay_action, (void *)dev, 100, 1, 0);
    if (ret) {
        return -1;
    }

    return 0;
}

static int yoc_led_rgb_open(aos_dev_t *dev)
{
    led_dev_t *       led        = (led_dev_t *)dev;
    led_pin_config_t *led_config = (led_pin_config_t *)led->device.config;

    yoc_led_rgb_config(dev, led_config->pin_r, led_config->pin_g, led_config->pin_b);

    return 0;
}

static int yoc_led_rgb_close(aos_dev_t *dev)
{
    led_dev_t *       led        = (led_dev_t *)dev;
    led_pin_config_t *led_config = (led_pin_config_t *)led->device.config;

    aos_timer_stop(&led->timer);
    aos_timer_free(&led->timer);

    if (led->led_r_pin_handle) {
        LED_GPIO_SET(led->led_r_pin_handle, 0, led_config->flip);
        csi_gpio_pin_uninitialize(led->led_r_pin_handle);
    }
    if (led->led_g_pin_handle) {
        LED_GPIO_SET(led->led_g_pin_handle, 0, led_config->flip);
        csi_gpio_pin_uninitialize(led->led_g_pin_handle);
    }
    if (led->led_b_pin_handle) {
        LED_GPIO_SET(led->led_b_pin_handle, 0, led_config->flip);
        csi_gpio_pin_uninitialize(led->led_b_pin_handle);
    }

    return 0;
}

static int yoc_led_rgb_control(aos_dev_t *dev, int color, int on_time, int off_time)
{
    led_dev_t *       led        = (led_dev_t *)dev;
    led_pin_config_t *led_config = (led_pin_config_t *)led->device.config;

    led->color    = color;
    led->on_time  = on_time;
    led->off_time = off_time;

    if (led->led_r_pin_handle) {
        drv_pinmux_config(led_config->pin_r, PIN_FUNC_GPIO);
        csi_gpio_pin_config(led->led_r_pin_handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
        LED_GPIO_SET(led->led_r_pin_handle, 0, led_config->flip);
    }

    if (led->led_g_pin_handle) {
        drv_pinmux_config(led_config->pin_g, PIN_FUNC_GPIO);
        csi_gpio_pin_config(led->led_g_pin_handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
        LED_GPIO_SET(led->led_g_pin_handle, 0, led_config->flip);
    }

    if (led->led_b_pin_handle) {
        drv_pinmux_config(led_config->pin_b, PIN_FUNC_GPIO);
        csi_gpio_pin_config(led->led_b_pin_handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
        LED_GPIO_SET(led->led_b_pin_handle, 0, led_config->flip);
    }

    led->blink = BLINK_ON;

    led->ctrl_req = 1;
    //if (on_time > 0 && off_time > 0 && color != COLOR_BLACK) {
    yoc_led_timer_trigger(dev);
    //}

    return 0;
}

static led_driver_t led_driver = {
    .drv =
        {
            .name   = "ledrgb",
            .type   = "led",
            .init   = yoc_led_rgb_init,
            .uninit = yoc_led_rgb_uninit,
            .lpm    = yoc_led_rgb_lpm,
            .open   = yoc_led_rgb_open,
            .close  = yoc_led_rgb_close,
        },
    .control = yoc_led_rgb_control,
};

void led_rgb_register(led_pin_config_t *config, int idx)
{
    driver_register(&led_driver.drv, (void *)config, idx);
}
