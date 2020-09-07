/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
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

/* saving the pulldown register status because the register write only */
static uint32_t pulldown_flag0;     /* saving the PortA and PortB pulldown register status */
static uint8_t pulldown_flag1;      /* saving the PortC pulldown register status */

/**
  \brief       initial hobbit pinmux.
  \return      none
*/
void ioreuse_initial(void)
{
    unsigned int value;

    value = readl(CH2201_GIPO0_PORTCTL_REG);
    value &= ~(GPIO0_REUSE_DIS);
    writel(value, CH2201_GIPO0_PORTCTL_REG);

}

/**
  \brief       config the pin function.
  \param[in]   pin       refs to pin_name_e.
  \param[in]   pin_func  refs to pin_func_e.
  \return      0-success or -1-failure
*/
int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func)
{
    unsigned int val = 0;
    unsigned int reg_val = 0;

    uint8_t offset;

    if (pin_func > 3) {
        /* GPIO FUNC */
        if (pin <= PB3) {
            if (pin <= PA5) {
                offset = pin;
                /* gpio data source select */
                val = readl(CH2201_GIPO0_PORTCTL_REG);
                val &= ~(1 << offset);
                writel(val, CH2201_GIPO0_PORTCTL_REG);
                return 0;
            } else if (pin >= PB0) {
                offset = pin - 6;
                /* gpio data source select */
                val = readl(CH2201_GIPO1_PORTCTL_REG);
                val &= ~(1 << offset);
                writel(val, CH2201_GIPO1_PORTCTL_REG);
                return 0;
            }
        }

        if ((pin >= PA6) && (pin <= PA27)) {
            offset = pin - 4;
            /* gpio data source select */
            val = readl(CH2201_GIPO0_PORTCTL_REG);
            val &= ~(1 << offset);
            writel(val, CH2201_GIPO0_PORTCTL_REG);
            return 0;
        }

        return -1;
    } else if (pin_func == 10) {
        /* PA10 PA12 PA14 SYS_WKUP */
        if (pin == PA10 || pin == PA12) {
            csi_vic_set_wakeup_irq(14);
            csi_vic_set_wakeup_irq(31);
        } else if (pin == PA14){
            csi_vic_set_wakeup_irq(14);
            csi_vic_set_wakeup_irq(30);
        }
        return 0;
    }

    if ((pin >= PA6) && (pin <= PA27)) {
        offset = pin - 4;

        /* gpio data source select */
        val = readl(CH2201_GIPO0_PORTCTL_REG);
        val |= (1 << offset);
        writel(val, CH2201_GIPO0_PORTCTL_REG);

        if (pin <= PA11) {
            offset = pin;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0L_REG);
            val &= ~(reg_val);
            val |= (pin_func << (2 * offset));
            writel(val, CH2201_IOMUX0L_REG);
            return 0;
        } else {
            offset = pin - 16;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0H_REG);
            val &= ~(reg_val);
            val |= (pin_func << (2 * offset));
            writel(val, CH2201_IOMUX0H_REG);
            return 0;
        }
    }

    if ((pin >= PA0) && (pin <= PB3)) {
        if (pin >= PB0) {
            offset = pin - 6;
            val = readl(CH2201_GIPO1_PORTCTL_REG);
            val |= (1 << offset);
            writel(val, CH2201_GIPO1_PORTCTL_REG);

            offset = pin;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0L_REG);
            val &= ~(reg_val);
            val |= (pin_func << (2 * offset));
            writel(val, CH2201_IOMUX0L_REG);
            return 0;
        }

        if (pin <= PA5) {
            offset = pin;
            /* gpio data source select */
            val = readl(CH2201_GIPO0_PORTCTL_REG);
            val |= (1 << offset);
            writel(val, CH2201_GIPO0_PORTCTL_REG);

            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0L_REG);
            val &= ~(reg_val);
            val |= (pin_func << (2 * offset));
            writel(val, CH2201_IOMUX0L_REG);
            return 0;
        }
    }

    if (pin > PA27) {
        offset = pin - PC0;
        reg_val = (0x3 << (offset * 2));
        val = readl(CH2201_IOMUX1L_REG);
        val &= ~(reg_val);
        val |= (pin_func << (2 * offset));
        writel(val, CH2201_IOMUX1L_REG);
        return 0;
    }

    return -1;
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
    unsigned int val = 0;
    uint32_t reg = 0;

    if (port == PORTA) {
        if (offset > PA5) {
            offset += 4;
        }
    } else if (port == PORTB) {
        offset += 6;
    }

    switch (pin_mode) {
        case GPIO_MODE_PULLUP:
            if (port == PORTC) {
                reg = CH2201_IOPU1_REG;

                /* disable Portc corresponding pin pulldown status */
                if (pulldown_flag1 & (1 << offset)) {
                    pulldown_flag1 &= ~(1 << offset);
                    writel(pulldown_flag1, CH2201_IOPD1_REG);
                }
            } else {
                /* disable PortA&PortB corresponding pin pulldown status */
                reg = CH2201_IOPU0_REG;

                if (pulldown_flag0 & (1 << offset)) {
                    pulldown_flag0 &= ~(1 << offset);
                    writel(pulldown_flag0, CH2201_IOPD0_REG);
                }
            }

            break;

        case GPIO_MODE_PULLDOWN:
            if (port == PORTC) {
                reg = CH2201_IOPD1_REG;
                val = readl(CH2201_IOPU1_REG);
                pulldown_flag1 |= (1 << offset);

                /* disable Portc corresponding pin pullup status */
                if (val & (1 << offset)) {
                    val &= ~(1 << offset);
                    writel(val, CH2201_IOPU1_REG);
                }
            } else {
                reg = CH2201_IOPD0_REG;
                val = readl(CH2201_IOPU0_REG);
                pulldown_flag0 |= (1 << offset);

                /* disable PortA&PortB corresponding pin pullup status */
                if (val & (1 << offset)) {
                    val &= ~(1 << offset);
                    writel(val, CH2201_IOPU0_REG);
                }
            }

            break;

        case GPIO_MODE_OPEN_DRAIN:
            if (port == PORTC) {
                reg = CH2201_IOOD1_REG;
            } else {
                reg = CH2201_IOOD0_REG;
            }

            break;

        case GPIO_MODE_PUSH_PULL:
            return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED);

        case GPIO_MODE_PULLNONE:
            if (port == PORTC) {
                /* disable Portc corresponding pin pulldown status */
                if (pulldown_flag1 & (1 << offset)) {
                    pulldown_flag1 &= ~(1 << offset);
                    writel(pulldown_flag1, CH2201_IOPD1_REG);
                }

                val = readl(CH2201_IOPU1_REG);

                /* disable Portc corresponding pin pullup status */
                if (val & (1 << offset)) {
                    val &= ~(1 << offset);
                    writel(val, CH2201_IOPU1_REG);
                }
            } else {
                /* disable PortA&PortB corresponding pin pulldown status */
                if (pulldown_flag0 & (1 << offset)) {
                    pulldown_flag0 &= ~(1 << offset);
                    writel(pulldown_flag0, CH2201_IOPD0_REG);
                }

                val = readl(CH2201_IOPU0_REG);

                /* disable PortA&PortB corresponding pin pullup status */
                if (val & (1 << offset)) {
                    val &= ~(1 << offset);
                    writel(val, CH2201_IOPU0_REG);
                }
            }

            return 0;

        default:
            return (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER);
    }

    val = readl(reg);
    val |= (1 << offset);
    writel(val, reg);
    return 0;
}

/**
  \brief       get the pin function.
  \param[in]   pin       refs to pin_name_e.
  \return      pin function count
*/
static pin_func_e drv_pinmux_get_config(pin_name_e pin)
{
    unsigned int val = 0;
    unsigned int reg_val = 0;

    uint8_t offset;
    if (pin <= PB3) {
        if (pin <= PA5) {
            offset = pin;
            /* gpio data source select */
            val = readl(CH2201_GIPO0_PORTCTL_REG);
            val &= (1 << offset);
            if (val != 1 << offset )
                return PIN_FUNC_GPIO;
        } else if (pin >= PB0) {
            offset = pin - 6;
            /* gpio data source select */
            val = readl(CH2201_GIPO1_PORTCTL_REG);
            val &= (1 << offset);
            if (val != 1 << offset )
                return PIN_FUNC_GPIO;
        }
    }

    if ((pin >= PA6) && (pin <= PA27)) {
        offset = pin - 4;
        /* gpio data source select */
        val = readl(CH2201_GIPO0_PORTCTL_REG);
        val &= (1 << offset);
        if (val != 1 << offset )
            return PIN_FUNC_GPIO;
    }


    if ((pin >= PA6) && (pin <= PA27)) {
        offset = pin - 4;

        if (pin <= PA11) {
            offset = pin;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0L_REG);
            val &= (reg_val);
            return (val >> (2 * offset));
        } else {
            offset = pin - 16;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0H_REG);
            val &= (reg_val);
            return (val >> (2 * offset));
        }
    }

    if ((pin >= PA0) && (pin <= PB3)) {
        if (pin >= PB0) {
            offset = pin;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0L_REG);
            val &= (reg_val);
            return (val >> (2 * offset));
        }

        if (pin <= PA5) {
            offset = pin;

            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(CH2201_IOMUX0L_REG);
            val &= (reg_val);
            return (val >> (2 * offset));
        }
    }

    if (pin > PA27) {
        offset = pin - PC0;
        reg_val = (0x3 << (offset * 2));
        val = readl(CH2201_IOMUX1L_REG);
        val &= (reg_val);
        return (val >> (2 * offset));
    }

    return PIN_FUNC_GPIO;
}
struct uart_tx_gpio_map {
        int32_t idx;
        pin_name_e  uart_tx;
        pin_func_e  uart_tx_fun;
}
const s_uart_tx_gpio_map[] = {
    {0, 2,  PA2_UART0_TX},
    {0, 10, PA10_UART0_TX},
    {1, 17, PA17_UART1_TX},
    {1, 21, PA21_UART1_TX},
    {2, 24, PA24_UART2_TX},
};

struct uart_rx_gpio_map {
        int32_t idx;
        pin_name_e  uart_rx;
        pin_func_e  uart_rx_fun;
}
const s_uart_rx_gpio_map[] = {
    {0, 3,  PA3_UART0_RX},
    {0, 11, PA11_UART0_RX},
    {1, 16, PA16_UART1_RX},
    {1, 20, PA20_UART1_RX},
    {2, 25, PA25_UART2_RX},
};

void drv_pinmux_dwuart_save(int32_t idx, pin_name_e *tx_pin, pin_name_e *rx_pin)
{
    if (tx_pin == NULL || rx_pin == NULL) {
        return;
    }

    int32_t cnt;
    *tx_pin = PC1 + 1;
    *rx_pin = PC1 + 1;

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
    if (tx_pin < PC1) {
        for (cnt = 0; cnt < sizeof(s_uart_tx_gpio_map) / sizeof(struct uart_tx_gpio_map); cnt++) {
            if (tx_pin == s_uart_tx_gpio_map[cnt].uart_tx) {
                drv_pinmux_config(s_uart_tx_gpio_map[cnt].uart_tx, s_uart_tx_gpio_map[cnt].uart_tx_fun);
                break;
            }
        }
    }

    if (rx_pin < PC1) {
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
    writel(0x3, CH2201_GIPO0_PORTCTL_REG);
    writel(0, CH2201_GIPO1_PORTCTL_REG);
}

