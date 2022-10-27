/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     dw_gpio.c
 * @brief    CSI Source File for GPIO Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pmu.h>
#include <dw_gpio.h>
#include <pin_name.h>
#include <clock.h>

extern int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode);
extern void registers_save(uint32_t *mem, uint32_t *addr, int size);
extern void registers_restore(uint32_t *addr, uint32_t *mem, int size);

#define ERR_GPIO(errno) (CSI_DRV_ERRNO_GPIO_BASE | errno)
#define GPIO_NULL_PARAM_CHK(para)    HANDLE_PARAM_CHK(para, ERR_GPIO(DRV_ERROR_PARAMETER))
#define RAM_CODE_SECTION(func)  __attribute__((section(".__sram.code."#func)))  func
static void RAM_CODE_SECTION(dw_gpio_irq_clear)(gpio_pin_handle_t pin, uint32_t idx);
void RAM_CODE_SECTION(dw_gpio_irqhandler)(int idx);
void RAM_CODE_SECTION(csi_gpio_prepare_sleep_action)();
void RAM_CODE_SECTION(csi_gpio_wakeup_sleep_action)();
void RAM_CODE_SECTION(csi_pinmux_prepare_sleep_action)();
void RAM_CODE_SECTION(csi_pinmux_wakeup_sleep_action)(void);

typedef void *gpio_port_handle_t;

typedef struct {
#ifdef CONFIG_LPM
    uint8_t gpio_power_status;
    uint32_t gpio_regs_saved[7];
#endif
    uint32_t base;              ///< handle register base
    uint32_t irq;               ///< irq of this handle
    uint32_t pin_num;           ///< pin number of this handle
    gpio_mode_e mode;           ///< gpio mode
    gpio_direction_e dir;       ///< gpio direction
    uint32_t mask;              ///< gpio mask bit
    uint32_t value;             ///< gpio value
} dw_gpio_priv_t;

typedef struct {
    uint8_t     portidx;
    uint8_t     idx;
    uint8_t     offset;
    gpio_event_cb_t cb;
} dw_gpio_pin_priv_t;

extern int32_t target_gpio_port_init(port_name_e port, uint32_t *base, uint32_t *irq, void **handler, uint32_t *pin_num);
extern int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx);

static dw_gpio_priv_t gpio_handle[CONFIG_GPIO_NUM];
static dw_gpio_pin_priv_t gpio_pin_handle[CONFIG_GPIO_PIN_NUM];

static uint32_t gpio_regs_saved[16];
static uint32_t pinmux_regs_saved[21];

//
// Functions
//
static int32_t dw_gpio_set_direction(
    void *port,
    gpio_direction_e direction
)
{
    dw_gpio_priv_t *gpio_priv = port;
    dw_gpio_reg_t *gpio_reg = (dw_gpio_reg_t *)(gpio_priv->base);

    if (direction == GPIO_DIRECTION_INPUT) {
        gpio_reg->SWPORT_DDR &= (~gpio_priv->mask);
    } else if (direction == GPIO_DIRECTION_OUTPUT) {
        gpio_reg->SWPORT_DDR |= gpio_priv->mask;
    } else {
        return ERR_GPIO(GPIO_ERROR_DIRECTION);
    }

    return 0;
}

/*
 * Read the statu of the Port choosed.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 * return: the value of the corresponding Port.
 */

static int32_t dw_gpio_read(void *port, int32_t idx, uint32_t *value)
{
    dw_gpio_priv_t *gpio_priv = port;
    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(gpio_priv->base + 0x30);

    if (idx == 0) {
        *value = gpio_control_reg->EXT_PORTA;
    }

    return 0;
}


/*
 * Write an output value to corresponding Port.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 *   output: value that will be written to the corresponding Port.
 * return: SUCCESS
 */

static int32_t dw_gpio_write(void *port, uint32_t mask)
{
    dw_gpio_priv_t *gpio_priv = port;
    dw_gpio_reg_t *gpio_reg = (dw_gpio_reg_t *)(gpio_priv->base);

    uint32_t value = gpio_reg->SWPORT_DR;

    value &= ~(mask);
    value |= gpio_priv->value;
    gpio_reg->SWPORT_DR = value;
    return 0;
}

/**
 * Configure a GPIO gpio_set_irq_mode.
 * @param[in]   pin         the addr store the pin num.
 * @param[in]   _irqmode    the irqmode of gpio
 * @return      zero on success. -1 on falure.
 */
static int32_t dw_gpio_set_irq_mode(gpio_pin_handle_t pin, gpio_irq_mode_e irq_mode)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);
    uint32_t offset = gpio_pin_priv->idx;
    uint32_t mask = 1 << offset;

    switch (irq_mode) {
        /* rising edge interrupt mode */
        case GPIO_IRQ_MODE_RISING_EDGE:
            gpio_control_reg->INTTYPE_LEVEL |= mask;
            gpio_control_reg->INT_POLARITY |= mask;
            break;

        /* falling edge interrupt mode */
        case GPIO_IRQ_MODE_FALLING_EDGE:
            gpio_control_reg->INTTYPE_LEVEL |= mask;
            gpio_control_reg->INT_POLARITY &= (~mask);
            break;

        /* low level interrupt mode */
        case GPIO_IRQ_MODE_LOW_LEVEL:
            gpio_control_reg->INTTYPE_LEVEL &= (~mask);
            gpio_control_reg->INT_POLARITY &= (~mask);
            break;

        /* high level interrupt mode */
        case GPIO_IRQ_MODE_HIGH_LEVEL:
            gpio_control_reg->INTTYPE_LEVEL &= (~mask);
            gpio_control_reg->INT_POLARITY |= mask;
            break;

        /* double edge interrupt mode */
        case GPIO_IRQ_MODE_DOUBLE_EDGE:
            return ERR_GPIO(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_GPIO(GPIO_ERROR_IRQ_MODE);
    }

    return 0;
}

/*
 * Clear one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void dw_gpio_irq_clear(gpio_pin_handle_t pin, uint32_t idx)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);

    gpio_control_reg->PORTA_EOI = idx;
}


/*
 * Enable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */
static void dw_gpio_irq_enable(gpio_pin_handle_t pin)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);
    uint32_t offset = gpio_pin_priv->idx;
    uint32_t val = gpio_control_reg->INTEN;
    val |= (1 << offset);
    gpio_control_reg->INTEN = val;
}


/*
 * Disable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void dw_gpio_irq_disable(gpio_pin_handle_t pin)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;
    uint32_t offset = gpio_pin_priv->idx;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);
    uint32_t val = gpio_control_reg->INTEN;
    val &= ~(1 << offset);
    gpio_control_reg->INTEN = val;
}

void dw_gpio_irqhandler(int idx)
{
    if (idx >= CONFIG_GPIO_NUM) {
        return;
    }

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(gpio_handle[idx].base + 0x30);

    uint32_t value = gpio_control_reg->INTSTATUS;
    uint8_t i;

    /* find the interrput pin */
    for (i = 0; i < 32; i++) {
        if (value & (1U << i)) {
            uint32_t pin_idx = i;

#ifndef CONFIG_CHIP_DANICA
            uint8_t j;

            if (idx > 0) {
                for (j = 0; j < idx; j++) {
                    pin_idx += gpio_handle[j].pin_num;
                }
            }

            if (pin_idx >= CONFIG_GPIO_PIN_NUM) {
                return;
            }

#endif
            dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)&gpio_pin_handle[pin_idx];

            dw_gpio_irq_clear(gpio_pin_priv, (1 << i));  //clear the gpio interrupt

            /* execute the callback function */
            if ((gpio_event_cb_t)(gpio_pin_priv->cb)) {
                ((gpio_event_cb_t)(gpio_pin_priv->cb))(gpio_pin_priv->offset);
            }
        }
    }
}

/**
  \brief       Initialize GPIO module. 1. Initializes the resources needed for the GPIO handle 2.registers event callback function
                3.get gpio_port_handle
  \param[in]   port      port_name.
  \return      gpio_port_handle
*/
gpio_port_handle_t csi_gpio_port_initialize(int32_t port)
{
    dw_gpio_priv_t *gpio_priv = NULL;

    /* obtain the gpio port information */
    uint32_t base = 0u;
    uint32_t pin_num;
    uint32_t irq;
    void *handler;
    int32_t idx = target_gpio_port_init(port, &base, &irq, &handler, &pin_num);

    if (idx < 0 || idx >= CONFIG_GPIO_NUM) {
        return NULL;
    }

    gpio_priv = &gpio_handle[idx];

    gpio_priv->base = base;
    gpio_priv->irq  = irq;
    gpio_priv->pin_num  = pin_num;

#ifdef CONFIG_LPM
    csi_gpio_power_control(gpio_priv, DRV_POWER_FULL);
#endif

    drv_irq_register(gpio_priv->irq, handler);
    drv_irq_enable(gpio_priv->irq);

    return (gpio_port_handle_t)gpio_priv;
}

/**
  \brief       De-initialize GPIO handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio port handle to operate.
  \return      error code
*/
int32_t csi_gpio_port_uninitialize(gpio_port_handle_t handle)
{
    GPIO_NULL_PARAM_CHK(handle);

    dw_gpio_priv_t *gpio_priv = handle;

    drv_irq_disable(gpio_priv->irq);
    drv_irq_unregister(gpio_priv->irq);

#ifdef CONFIG_LPM
    csi_gpio_power_control(gpio_priv, DRV_POWER_OFF);
#endif

    return 0;
}

#ifdef CONFIG_LPM
static void manage_clock(gpio_pin_handle_t handle, uint8_t enable)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)handle;
    uint8_t device[] = {CLOCK_MANAGER_GPIO0, CLOCK_MANAGER_GPIO1};

    drv_clock_manager_config(device[gpio_pin_priv->portidx], enable);
}

static void do_prepare_sleep_action(void *handle)
{
    dw_gpio_priv_t *gpio_handle = handle;
    uint32_t *gbase = (uint32_t *)(gpio_handle->base);
    uint32_t *control_base = (uint32_t *)(gpio_handle->base + 0x30);
    registers_save(gpio_handle->gpio_regs_saved, gbase, 3);
    registers_save(&gpio_handle->gpio_regs_saved[3], control_base, 4);
}

static void do_wakeup_sleep_action(void *handle)
{
    dw_gpio_priv_t *gpio_handle = handle;
    uint32_t *gbase = (uint32_t *)(gpio_handle->base);
    uint32_t *control_base = (uint32_t *)(gpio_handle->base + 0x30);
    registers_restore(gbase, gpio_handle->gpio_regs_saved, 3);
    registers_restore(control_base, &gpio_handle->gpio_regs_saved[3], 4);
}
#endif

void csi_gpio_prepare_sleep_action()
{
    uint32_t addr = AP_GPIOA_BASE;
    registers_save(gpio_regs_saved, (uint32_t *)addr, 16);
    phy_gpioretention_prepare_sleep_action();
}

void csi_gpio_wakeup_sleep_action()
{
    uint32_t addr = AP_GPIOA_BASE;
    registers_save((uint32_t *)addr, gpio_regs_saved, 16);
    phy_gpioretention_prepare_wakeup_action();
}

void csi_pinmux_prepare_sleep_action()
{
    uint32_t addr = AP_IOMUX_BASE;
    registers_save(pinmux_regs_saved, (uint32_t *)addr, 21);
}

void csi_pinmux_wakeup_sleep_action(void)
{
    uint32_t addr = AP_IOMUX_BASE;
    registers_restore((uint32_t *)addr, pinmux_regs_saved, 21);
}


/**
  \brief       Initialize GPIO handle.
  \param[in]   gpio_pin    Pointer to the int32_t.
  \param[in]   cb_event    Pointer to \ref gpio_event_cb_t.
  \param[in]   arg    Pointer to \ref arg used for the callback.
  \return      gpio_pin_handle
*/
gpio_pin_handle_t csi_gpio_pin_initialize(int32_t gpio_pin, gpio_event_cb_t cb_event)
{

    if (gpio_pin < 0 || gpio_pin >= CONFIG_GPIO_PIN_NUM) {
        return NULL;
    }

    uint32_t i;

    for (i = 0; i < CONFIG_GPIO_NUM; i++) {
        csi_gpio_port_initialize(i);
    }

    /* obtain the gpio pin information */
    uint32_t port_idx;
    int32_t pin_idx = target_gpio_pin_init(gpio_pin, &port_idx);

    if (pin_idx < 0) {
        return NULL;
    }

    int32_t idx = pin_idx;

    for (i = 0; i < port_idx; i++) {
        idx += (gpio_handle[i].pin_num);
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[idx]);
    gpio_pin_priv->portidx = port_idx;
    gpio_pin_priv->idx = pin_idx;
    gpio_pin_priv->cb = cb_event;
    gpio_pin_priv->offset = gpio_pin;

    return (gpio_pin_handle_t)gpio_pin_priv;
}

/**
  \brief       De-initialize GPIO pin handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio pin handle to operate.
  \return      error code
*/
int32_t csi_gpio_pin_uninitialize(gpio_pin_handle_t handle)
{
    if (handle == NULL) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)handle;
    gpio_pin_priv->cb = NULL;

    dw_gpio_irq_disable(handle);

    return 0;
}

/**
  \brief       control gpio power.
  \param[in]   idx  gpio index.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_gpio_power_control(gpio_pin_handle_t handle, csi_power_stat_e state)
{
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config pin mode
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \return      error code
*/
int32_t csi_gpio_pin_config_mode(gpio_pin_handle_t handle,
                                 gpio_mode_e mode)
{
    GPIO_NULL_PARAM_CHK(handle);

    /* config the gpio pin mode direction mask bits */
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    uint8_t offset = gpio_pin_priv->offset;

    int32_t ret = drv_pin_config_mode(gpio_pin_priv->portidx, offset, mode);

    return ret;
}
/**
  \brief       config pin direction
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config_direction(gpio_pin_handle_t handle,
                                      gpio_direction_e dir)
{
    int ret =0;
    GPIO_NULL_PARAM_CHK(handle);

    /* config the gpio pin mode direction mask bits */
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t *gpio_priv = &gpio_handle[gpio_pin_priv->portidx];

    gpio_priv->dir = dir;
    gpio_priv->mask = 1 << gpio_pin_priv->idx;
    ret = dw_gpio_set_direction(gpio_priv, dir);
    if(ret) {
       return ret;
    }
    phy_gpioretention_register(gpio_pin_priv->idx);
    return 0;
}

/**
  \brief       config pin
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config(gpio_pin_handle_t handle,
                            gpio_mode_e mode,
                            gpio_direction_e dir)
{
    GPIO_NULL_PARAM_CHK(handle);

    /* config the gpio pin mode direction mask bits */
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t *gpio_priv = &gpio_handle[gpio_pin_priv->portidx];

    gpio_priv->mode = mode;
    gpio_priv->dir = dir;
    gpio_priv->mask = 1 << gpio_pin_priv->idx;

    uint8_t offset = gpio_pin_priv->offset;
    int32_t ret = drv_pin_config_mode(gpio_pin_priv->portidx, offset, mode);

    if (ret) {
        return ret;
    }

    ret = dw_gpio_set_direction(gpio_priv, dir);

    if (ret) {
        return ret;
    }

    return 0;

}

/**
  \brief       Set one or zero to the selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_pin_write(gpio_pin_handle_t handle, bool value)
{
    GPIO_NULL_PARAM_CHK(handle);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    uint8_t offset = gpio_pin_priv->idx;
    uint32_t port_value = value << offset;

    port_handle->value = port_value;
    dw_gpio_write(port_handle, (1 << offset));

    return 0;

}

/**
  \brief       Get the value of  selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[out]  value     buf to store the pin value
  \return      error code
*/
int32_t csi_gpio_pin_read(gpio_pin_handle_t handle, bool *value)
{
    GPIO_NULL_PARAM_CHK(handle);
    GPIO_NULL_PARAM_CHK(value);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;
    uint32_t port_value = 0;
    uint8_t offset = gpio_pin_priv->idx;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_read(port_handle, gpio_pin_priv->portidx, &port_value);
    *value = (port_value & (1 << offset)) >> offset;

    return 0;
}

/**
  \brief       set GPIO interrupt mode.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      the irq mode to be set
  \param[in]   enable    the enable flag
  \return      error code
*/
int32_t csi_gpio_pin_set_irq(gpio_pin_handle_t handle, gpio_irq_mode_e mode, bool enable)
{
    GPIO_NULL_PARAM_CHK(handle);

    uint32_t ret = 0;

    if (enable) {
        ret = dw_gpio_set_irq_mode(handle, mode);

        if (ret) {
            return ret;
        }

        dw_gpio_irq_enable(handle);

    } else {
        dw_gpio_irq_disable(handle);

    }

    return ret;

}

