/*
 * Copyright (C) 2020 FishSemi Limited
 */

/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     14. May 2020
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <drv/gpio.h>
#include "pinmux.h"
#include "pin_name.h"
#include <csi_core.h>

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

struct pinmux_mapping_s
{
    uint32_t pin;
    uint32_t offset;
};

static const struct pinmux_mapping_s mapping[] =
{
    { 0, 0xA0}, { 1, 0xA4}, { 2, 0xA8}, { 3, 0xAC},
    { 4, 0x00}, { 5, 0x04}, { 6, 0x08}, { 7, 0x0C},
    { 8, 0x10}, { 9, 0x14}, {10, 0x18}, {11, 0x1C},
    {12, 0x20}, {13, 0x24}, {14, 0x28}, {15, 0x2C},
    {16, 0x30}, {17, 0x34}, {18, 0x38}, {19, 0x3C},
    {20, 0x40}, {21, 0x44}, {22, 0x48}, {23, 0x4C},
    {24, 0x50}, {25, 0x54}, {26, 0x58}, {27, 0x5C},
    {28, 0x60}, {29, 0x64}, {30, 0x68}, {31, 0x6C},
    {32, 0x70}, {33, 0x74}, {34, 0x78}, {35, 0x7C},
    {36, 0x80}, {37, 0x84}, {38, 0x88}, {39, 0x8C},
    {40, 0x90}, {41, 0x94}, {42, 0x98}, {43, 0xB0},
    {44, 0xB8}, {.pin = UINT32_MAX},
};

static int pinmux_find_offset(uint32_t pin)
{
    uint32_t i = 0;

    while (UINT32_MAX != mapping[i].pin) {
        if (mapping[i].pin == pin) {
            return mapping[i].offset;
        }
        i++;
    }

    return -EINVAL;
}

/**
  \brief       initial hobbit pinmux.
  \return      none
*/
void ioreuse_initial(void)
{
}

/**
  \brief       config the pin function.
  \param[in]   pin       refs to pin_name_e.
  \param[in]   pin_func  refs to pin_func_e.
  \return      0-success or -1-failure
*/
int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func)
{
    uint32_t val = 0;
    int offset = pinmux_find_offset((uint32_t)pin);

    if (offset < 0)
        return -1;

    val = readl(U1_PINMUX_BASE_ADDR + offset);

    if (pin > GP04) {
        val &= ~0x3;
        val |= (uint8_t)pin_func & 0x3;
    } else {
        val &= ~0x2;
        val |= ((uint8_t)pin_func & 0x1) << 1;
    }

    writel(val, U1_PINMUX_BASE_ADDR + offset);

    return 0;
}

/**
  \brief       config the pin mode.
  \param[in]   port      refs to port_name_e.
  \param[in]   offset    the offset of the pin in corresponding port.
  \param[in]   pin_mode  refs to gpio_mode_e.
  \return      0-success or -1-failure
*/
int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode)
{
    uint32_t val = 0;
    int reg_offset = 0;

    if (port != PORTA)
        return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED);

    if (offset > (uint8_t)GP41)
        return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED);

    reg_offset = pinmux_find_offset(offset);
    if (reg_offset < 0)
        return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED);

    val = readl(U1_PINMUX_BASE_ADDR + reg_offset);
    val &= ~(3 << 2);

    switch (pin_mode) {
        case GPIO_MODE_PULLUP:
            val |= (1 << 2);
            break;

        case GPIO_MODE_PULLDOWN:
            val |= (2 << 2);
            break;

        case GPIO_MODE_OPEN_DRAIN:
            return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED);

        case GPIO_MODE_PUSH_PULL:
        case GPIO_MODE_PULLNONE:
            val |= (0 << 2);
            break;

        default:
            return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER);
    }

    writel(val, U1_PINMUX_BASE_ADDR + reg_offset);

    return 0;
}

/**
  \brief       get the pin function.
  \param[in]   pin       refs to pin_name_e.
  \return      pin function count
*/
static pin_func_e drv_pinmux_get_config(pin_name_e pin)
{
    uint32_t val = 0;
    int offset = 0;

    offset = pinmux_find_offset((uint32_t)pin);
    if (offset < 0)
        return PIN_FUNC_GPIO;

    val = readl(U1_PINMUX_BASE_ADDR + offset);

    if (pin > GP04) {
        val &= 0x3;
    } else {
        val &= 0x2;
        val >>= 1;
    }

    return (pin_func_e)val;
}

struct uart_tx_gpio_map {
    int32_t idx;
    pin_name_e  uart_tx;
    pin_func_e  uart_tx_fun;
}
const s_uart_tx_gpio_map[] = {
    {1, GP07, GP07_FUNC_U1TXD},
    {2, GP11, GP11_FUNC_U2TXD},
    {3, GP13, GP13_FUNC_U3TXD},
};

struct uart_rx_gpio_map {
    int32_t idx;
    pin_name_e  uart_rx;
    pin_func_e  uart_rx_fun;
}
const s_uart_rx_gpio_map[] = {
    {1, GP08, GP08_FUNC_U1RXD},
    {2, GP12, GP12_FUNC_U2RXD},
    {3, GP14, GP14_FUNC_U3RXD},
};

void drv_pinmux_dwuart_save(int32_t idx, pin_name_e *tx_pin, pin_name_e *rx_pin)
{
    if (tx_pin == NULL || rx_pin == NULL) {
        return;
    }

    int32_t cnt;
    *tx_pin = DSIND + 1;
    *rx_pin = DSIND + 1;

    for (cnt = 0; cnt < sizeof(s_uart_tx_gpio_map) / sizeof(struct uart_tx_gpio_map); cnt++) {
        if (idx == s_uart_tx_gpio_map[cnt].idx && drv_pinmux_get_config(s_uart_tx_gpio_map[cnt].uart_tx) == s_uart_tx_gpio_map[cnt].uart_tx_fun) {
            drv_pinmux_config(s_uart_tx_gpio_map[cnt].uart_tx, PIN_FUNC_GPIO);
            drv_pin_config_mode(PORTA, s_uart_tx_gpio_map[cnt].uart_tx,1);
            *tx_pin = s_uart_tx_gpio_map[cnt].uart_tx;
            break;
        }
    }

    for (cnt = 0; cnt < sizeof(s_uart_rx_gpio_map) / sizeof(struct uart_rx_gpio_map); cnt++) {
        if (idx == s_uart_rx_gpio_map[cnt].idx && drv_pinmux_get_config(s_uart_rx_gpio_map[cnt].uart_rx) == s_uart_rx_gpio_map[cnt].uart_rx_fun) {
            drv_pinmux_config(s_uart_rx_gpio_map[cnt].uart_rx, PIN_FUNC_GPIO);
            drv_pin_config_mode(PORTA, s_uart_rx_gpio_map[cnt].uart_rx,1);
            *rx_pin = s_uart_rx_gpio_map[cnt].uart_rx;
            break;
        }
    }
}

void drv_pinmux_dwuart_restore(pin_name_e tx_pin, pin_name_e rx_pin)
{
    int32_t cnt;

    if (tx_pin < DSIND) {
        for (cnt = 0; cnt < sizeof(s_uart_tx_gpio_map) / sizeof(struct uart_tx_gpio_map); cnt++) {
            if (tx_pin == s_uart_tx_gpio_map[cnt].uart_tx) {
                drv_pinmux_config(s_uart_tx_gpio_map[cnt].uart_tx, s_uart_tx_gpio_map[cnt].uart_tx_fun);
                break;
            }
        }
    }

    if (rx_pin < DSIND) {
        for (cnt = 0; cnt < sizeof(s_uart_rx_gpio_map) / sizeof(struct uart_rx_gpio_map); cnt++) {
            if (rx_pin == s_uart_rx_gpio_map[cnt].uart_rx) {
                drv_pinmux_config(s_uart_rx_gpio_map[cnt].uart_rx, s_uart_rx_gpio_map[cnt].uart_rx_fun);
                break;
            }
        }
    }
}

void drv_pinmux_reset(void)
{
}

