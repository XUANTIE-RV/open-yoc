/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include "aos/hal/uart.h"

#define UART_BUF_SIZE   20
#define UART_TX_TIMEOUT 1000
#define UART_RX_TIMEOUT 1000
#define UART_TEST_CNT   3

/* define dev */
static uart_dev_t task_uart;
static uart_dev_t task1_uart;

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

    /* uart port set */
    task_uart.port = uart_idx;

    /* uart attr config */
    task_uart.config.baud_rate    = 115200;
    task_uart.config.data_width   = DATA_WIDTH_8BIT;
    task_uart.config.parity       = NO_PARITY;
    task_uart.config.stop_bits    = STOP_BITS_1;
    task_uart.config.flow_control = FLOW_CONTROL_DISABLED;
    task_uart.config.mode         = MODE_TX_RX;

    /* init task_uart with the given settings */
    ret = hal_uart_init(&task_uart);
    if (ret != 0) {
        printf("task: uart init error !\n");
        return -1;
    }

    /* send data by task_uart */
    printf("# Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);

    ret = hal_uart_send(&task_uart, uart_data_buf_send, sizeof(uart_data_buf_send), UART_TX_TIMEOUT);
    if (ret == 0) {
        printf("\n\n\ntask: uart data send succeed !\n");
    } else {
        printf("\n\n\ntask: uart data send failed !\n");
        return -1;
    }

    printf("\n\n\n# Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv, 0, sizeof(str_recv));
    /* receive data */
    hal_uart_recv(&task_uart, uart_data_buf_recv, sizeof(str_recv) + 1, 0xffffffff);
    uart_data_buf_recv[10] = '\0';

    if (0 == memcmp(str_recv, uart_data_buf_recv, sizeof(str_recv))) {
        printf("task: uart data received succeed !\n");
    } else {
        printf("task: uart test failed !\n");
        return -1;
    }

    hal_uart_finalize(&task_uart);
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

    /* uart port set */
    task1_uart.port = uart_idx;

    /* uart attr config */
    task1_uart.config.baud_rate    = 115200;
    task1_uart.config.data_width   = DATA_WIDTH_8BIT;
    task1_uart.config.parity       = NO_PARITY;
    task1_uart.config.stop_bits    = STOP_BITS_1;
    task1_uart.config.flow_control = FLOW_CONTROL_DISABLED;
    task1_uart.config.mode         = MODE_TX_RX;

    /* init task_uart with the given settings */
    ret = hal_uart_init(&task1_uart);
    if (ret != 0) {
        printf("task1: uart init error !\n");
        return -1;
    }

    /* send data by task_uart */
    printf("# Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send1);

    ret = hal_uart_send(&task1_uart, uart_data_buf_send1, sizeof(uart_data_buf_send1) + 1, UART_TX_TIMEOUT);
    if (ret == 0) {
        printf("\n\n\ntask1: uart data send succeed !\n");
    } else {
        printf("\n\n\ntask1: uart data send failed !\n");
        return -1;
    }

    printf("\n\n\n# Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv1, 0, sizeof(str_recv));
    /* receive data */
    hal_uart_recv(&task1_uart, uart_data_buf_recv1, sizeof(str_recv) + 1, 0xffffffff);
    uart_data_buf_recv1[10] = '\0';

    if (0 == memcmp(str_recv, uart_data_buf_recv1, sizeof(str_recv))) {
        printf("task1: uart data received succeed !\n");
    } else {
        printf("task1: uart test failed !\n");
        return -1;
    }

    hal_uart_finalize(&task1_uart);
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