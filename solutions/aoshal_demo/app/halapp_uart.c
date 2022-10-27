/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include "aos/hal/uart.h"

#define UART_BUF_SIZE   20
#define UART_TX_TIMEOUT 10
#define UART_RX_TIMEOUT 10
#define UART_TEST_CNT   3

/* define dev */
static uart_dev_t uart1;

/* data buffer */
static char uart_data_buf_send[] = "I am uart!";
static char uart_data_buf_recv[UART_BUF_SIZE];
const uint8_t str_recv[] = "uart demo!";

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

int hal_uart_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    int ret     = -1;

    printf("uart test start...\n");

    uint32_t pin0_func = get_uart_pin_func(gpio_pin0, uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pin1, uart_pinmap);
    uint8_t uart_idx = get_uart_idx(gpio_pin0, uart_pinmap);

    csi_pin_set_mux(gpio_pin0, pin0_func);
    csi_pin_set_mux(gpio_pin1, pin1_func);

    /* uart port set */
    uart1.port = uart_idx;

    /* uart attr config */
    uart1.config.baud_rate    = 115200;
    uart1.config.data_width   = DATA_WIDTH_8BIT;
    uart1.config.parity       = NO_PARITY;
    uart1.config.stop_bits    = STOP_BITS_1;
    uart1.config.flow_control = FLOW_CONTROL_DISABLED;
    uart1.config.mode         = MODE_TX_RX;

    /* init uart1 with the given settings */
    ret = hal_uart_init(&uart1);
    if (ret != 0) {
        printf("uart1 init error !\n");
        return -1;
    }

    /* send data by uart1 */
    printf("# Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);

    ret = hal_uart_send(&uart1, uart_data_buf_send, sizeof(uart_data_buf_send), UART_RX_TIMEOUT);
    if (ret == 0) {
        printf("\n\n\nuart1 data send succeed !\n");
    } else {
        printf("\n\n\nuart1 data send failed !\n");
        return -1;
    }

    printf("\n\n\n# Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv, 0, sizeof(str_recv));
    /* receive and send data 3 times in a loop */
    hal_uart_recv(&uart1, uart_data_buf_recv, sizeof(str_recv), 0xffffffff);
    uart_data_buf_recv[10] = '\0';

    if (0 == memcmp(str_recv, uart_data_buf_recv, sizeof(str_recv))) {
        printf("uart1 data received succeed !\n");
    } else {
        printf("uart test failed !\n");
        return -1;
    }

    hal_uart_finalize(&uart1);
    printf("uart test successfully\n");

    return 0;
}
