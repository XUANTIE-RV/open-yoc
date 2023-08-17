/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_GPIOPIN_H_
#define _DEVICE_GPIOPIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum     rvm_hal_gpio_dir_t
 * \brief    GPIO dir define
 */
typedef enum {
    RVM_GPIO_DIRECTION_INPUT       = 0,    ///< GPIO as input
    RVM_GPIO_DIRECTION_OUTPUT,             ///< GPIO as output
} rvm_hal_gpio_dir_t;

/**
 * \enum     rvm_hal_gpio_pin_data_t
 * \brief    GPIO pin data define
 */
typedef enum {
    RVM_GPIO_PIN_LOW                = 0,   ///< GPIO low level
    RVM_GPIO_PIN_HIGH,                     ///< GPIO high level
} rvm_hal_gpio_pin_data_t;

/**
 * \enum     rvm_hal_gpio_mode_t
 * \brief    GPIO mode define
 */
typedef enum {
    RVM_GPIO_MODE_PULLNONE         = 0,    ///< Pull none
    RVM_GPIO_MODE_PULLUP,                  ///< Pull up for input
    RVM_GPIO_MODE_PULLDOWN,                ///< Pull down for input
    RVM_GPIO_MODE_OPEN_DRAIN,              ///< Open drain mode for output
    RVM_GPIO_MODE_PUSH_PULL,               ///< Push-pull mode for output
} rvm_hal_gpio_mode_t;

/**
 * \enum     rvm_hal_gpio_irq_mode_t
 * \brief    GPIO irq triger type
 */
typedef enum {
    RVM_GPIO_IRQ_MODE_RISING_EDGE  = 0,    ///< Interrupt mode for rising edge
    RVM_GPIO_IRQ_MODE_FALLING_EDGE,        ///< Interrupt mode for falling edge
    RVM_GPIO_IRQ_MODE_BOTH_EDGE,           ///< Interrupt mode for both edge
    RVM_GPIO_IRQ_MODE_LOW_LEVEL,           ///< Interrupt mode for low level
    RVM_GPIO_IRQ_MODE_HIGH_LEVEL,          ///< Interrupt mode for high level
} rvm_hal_gpio_irq_mode_t;

typedef void (*rvm_hal_gpio_pin_callback)(rvm_dev_t *dev, void *arg);

/**
  \brief       Open GPIO pin by dev name, such as "gpio_pin120"
  \param[in]   name        The GPIO pin device name, such as "gpio_pin120"
  \return      gpio_pin dev on success, else on fail.
*/
#define rvm_hal_gpio_pin_open(name)        rvm_hal_device_open(name)

/**
  \brief       Close GPIO pin by dev object
  \param[in]   dev         Pointer to device object.
  \return      0 on success, else on fail.
*/
#define rvm_hal_gpio_pin_close(dev)        rvm_hal_device_close(dev)

/**
  \brief       Open GPIO pin by dev and pin_name, such as BT_RESET_PIN
  \param[in]   name        The GPIO pin device base name, must be "gpio_pin"
  \param[in]   pin_name    GPIO pin name
  \return      gpio_pin dev on success, else on fail.
*/
rvm_dev_t *rvm_hal_gpio_pin_open_by_pin_name(const char *name, int pin_name);

/**
  \brief       Config pin direction
  \param[in]   dev         Pointer to device object.
  \param[in]   dir         \ref rvm_hal_gpio_dir_t
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_set_direction(rvm_dev_t *dev, rvm_hal_gpio_dir_t dir);

/**
  \brief       Config pin mode
  \param[in]   dev         Pointer to device object.
  \param[in]   mode        \ref rvm_hal_gpio_mode_t
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_set_mode(rvm_dev_t *dev, rvm_hal_gpio_mode_t mode);

/**
  \brief       Attach the interrupt callback to the GPIO pin
  \param[in]   dev         Pointer to device object.
  \param[in]   callback    Callback function
  \param[in]   arg         User param passed to callback
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_attach_callback(rvm_dev_t *dev, rvm_hal_gpio_pin_callback callback, void *arg);

/**
  \brief       Config pin irq params
  \param[in]   dev         Pointer to device object.
  \param[in]   irq_mode    Interrupt trigger mode \ref rvm_hal_gpio_irq_mode_t
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_set_irq_mode(rvm_dev_t *dev, rvm_hal_gpio_irq_mode_t irq_mode);

/**
  \brief       Enable or disable gpio pin interrupt
  \param[in]   dev         Pointer to device object.
  \param[in]   enable      0:disable  1:enable
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_irq_enable(rvm_dev_t *dev, bool enable);

/**
  \brief       Set debounce of pin when pin configed as input
  \param[in]   dev         Pointer to device object.
  \param[in]   enbale      0: disable   1:enable
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_set_debounce(rvm_dev_t *dev, bool enable);

/**
  \brief       Set one or zero to specified pin
  \param[in]   dev         Pointer to device object.
  \param[in]   value       Value to be set \ref rvm_hal_gpio_pin_data_t
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_write(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t value);

/**
  \brief       Get the value of specified GPIO pin
  \param[in]   dev         Pointer to device object.
  \param[out]  value       Value to be get \ref rvm_hal_gpio_pin_data_t
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_read(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t *value);

/**
  \brief       Toggle output pin value,ex.if previous value is 1, then output 0
  \param[in]   dev         Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_gpio_pin_output_toggle(rvm_dev_t *dev);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_gpiopin.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
