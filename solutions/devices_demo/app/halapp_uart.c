/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/uart.h>
#include <devices/devicelist.h>

#define UART_BUF_SIZE   20
#define UART_TX_TIMEOUT 1000
#define UART_RX_TIMEOUT 1000
#define UART_TEST_CNT   3

/* data buffer */
static char uart_data_buf_send[] = "I am uart!";
static char uart_data_buf_recv[UART_BUF_SIZE];
const uint8_t str_recv[] = "uart demo!";
static char uart_data_buf_send1[] = "I am uart!";
static char uart_data_buf_recv1[UART_BUF_SIZE];

extern const csi_pinmap_t uart_pinmap[];

static uint32_t get_uart_pin_func(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFF;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin) {
            ret = (uint32_t)map->pin_func;
            break;
        }

        map++;
    }

    return ret;
}

static uint8_t get_uart_idx(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin) {
            ret = (uint32_t)map->idx;
            break;
        }

        map++;
    }

    return ret;
}

int hal_uart_demo(uint32_t *gpio_pins)
{
    int ret     = -1;

    printf("task: uart test start...\n");

    uint8_t uart_idx = get_uart_idx(gpio_pins[0], uart_pinmap);

#if !defined(CONFIG_CHIP_BL606P_E907) // 博流的板子UART的复用在board里面做掉了
    uint32_t pin0_func = get_uart_pin_func(gpio_pins[0], uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pins[1], uart_pinmap);

    csi_pin_set_mux(gpio_pins[0], pin0_func);
    csi_pin_set_mux(gpio_pins[1], pin1_func);
#endif

    rvm_uart_drv_register(uart_idx);

    char file[8];
    sprintf(file, "uart%d", uart_idx);

    rvm_dev_t *uart_dev = NULL;

    uart_dev = rvm_hal_uart_open(file);
    if (uart_dev == NULL) {
        return -1;
    }

    rvm_hal_uart_config_t config;
    config.baud_rate    = 115200;
    config.data_width   = DATA_WIDTH_8BIT;
    config.parity       = PARITY_NONE;
    config.stop_bits    = STOP_BITS_1;
    config.flow_control = FLOW_CONTROL_DISABLED;
    config.mode         = MODE_TX_RX;
    

    rvm_hal_uart_config(uart_dev, &config);

    /* send data by task_uart */
    printf("# task: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);
    rvm_hal_uart_send(uart_dev, uart_data_buf_send, sizeof(uart_data_buf_send), AOS_WAIT_FOREVER);

    printf("\n\n\n# task: Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv, 0, sizeof(str_recv));
    /* receive data */
    rvm_hal_uart_recv(uart_dev, uart_data_buf_recv, sizeof(str_recv) + 1, 0xffffffff);
    uart_data_buf_recv[10] = '\0';

    if (0 == memcmp(str_recv, uart_data_buf_recv, sizeof(str_recv))) {
        printf("task: uart data received succeed !\n");
    } else {
        printf("task: uart test failed !\n");
        return -1;
    }

    ret = rvm_hal_uart_close(uart_dev);

    if (ret < 0) {
        printf("task: uart_close error !\n");
    }
    
    printf("task: uart test successfully\n");

    return 0;
}

int hal_uart_demo1(uint32_t *gpio_pins)
{
    int ret     = -1;

    printf("task1: uart test start...\n");

    uint8_t uart_idx = get_uart_idx(gpio_pins[0], uart_pinmap);

#if !defined(CONFIG_CHIP_BL606P_E907) // 博流的板子UART的复用在board里面做掉了
    uint32_t pin0_func = get_uart_pin_func(gpio_pins[0], uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pins[1], uart_pinmap);

    csi_pin_set_mux(gpio_pins[0], pin0_func);
    csi_pin_set_mux(gpio_pins[1], pin1_func);
#endif

    rvm_uart_drv_register(uart_idx);

    char file[8];
    sprintf(file, "uart%d", uart_idx);

    rvm_dev_t *uart_dev = NULL;

    uart_dev = rvm_hal_uart_open(file);
    if (uart_dev == NULL) {
        return -1;
    }

    /* send data by task_uart */
    printf("# task1: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send1);
    rvm_hal_uart_send(uart_dev, uart_data_buf_send1, sizeof(uart_data_buf_send1), AOS_WAIT_FOREVER);

    printf("\n\n\n# task1: Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv1, 0, sizeof(str_recv));
    /* receive data */
    rvm_hal_uart_recv(uart_dev, uart_data_buf_recv1, sizeof(str_recv) + 1, 0xffffffff);
    uart_data_buf_recv1[10] = '\0';

    aos_msleep(10);

    if (0 == memcmp(str_recv, uart_data_buf_recv1, sizeof(str_recv))) {
        printf("task1: uart data received succeed !\n");
    } else {
        printf("task1: uart test failed !\n");
        return -1;
    }

    ret = rvm_hal_uart_close(uart_dev);

    if (ret < 0) {
        printf("task1: uart_close error !\n");
    }

    ret = rvm_driver_unregister(file);

    if (ret < 0) {
        printf("task1: rvm_driver_unregister error !\n");
    }
    
    printf("task1: uart test successfully\n");

    return 0;
}

void uart_task(void *priv)
{
    uint32_t *port = (uint32_t *)priv;

    hal_uart_demo(port);
    aos_msleep(500);
}


void uart_task1(void *priv)
{
    uint32_t *port = (uint32_t *)priv;

    hal_uart_demo1(port);
    aos_msleep(500);
}

int hal_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins)
{
    aos_task_t task;

    aos_task_new_ext(&task, "uart_task", uart_task, (void *)task1_pins, 4096, 32);
    aos_task_new_ext(&task, "uart_task1", uart_task1, (void *)task2_pins, 4096, 32);

    return 0;
}
