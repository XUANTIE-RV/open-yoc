#ifndef __CVI_GPIO_H__
#define __CVI_GPIO_H__

#include <soc.h>
#include "hal_gpio.h"
#include "cvi_pin.h"

typedef enum {
    GPIO_DIR_INPUT       = 0,    ///< GPIO as input
    GPIO_DIR_OUTPUT,             ///< GPIO as output
} cvi_gpio_dir_t;

typedef enum {
    GPIO_IRQ_FLAG_RISING_EDGE  = 0,    ///< Interrupt mode for rising edge
    GPIO_IRQ_FLAG_FALLING_EDGE,        ///< Interrupt mode for falling edge
    GPIO_IRQ_FLAG_BOTH_EDGE,           ///< Interrupt mode for both edge
    GPIO_IRQ_FLAG_LOW_LEVEL,           ///< Interrupt mode for low level
    GPIO_IRQ_FLAG_HIGH_LEVEL,          ///< Interrupt mode for high level
} cvi_gpio_irq_flag_t;

typedef enum {
    GPIO_STATE_LOW                = 0,   ///< GPIO low level
    GPIO_STATE_HIGH,                     ///< GPIO high level
} cvi_gpio_state_t;

typedef struct _cvi_gpio_t {
    cvi_dev_t           dev;
    void (*callback)(struct _cvi_gpio_t *gpio, uint32_t pins, void *arg);
    void                *args;
    void                *priv;
} cvi_gpio_t;

cvi_error_t cvi_gpio_init(cvi_gpio_t *gpio);
void cvi_gpio_uninit(cvi_gpio_t *gpio);
cvi_error_t cvi_gpio_set_dir(cvi_gpio_t *gpio, uint32_t pin_mask, cvi_gpio_dir_t dir);
cvi_error_t cvi_gpio_set_mode(cvi_gpio_t *gpio, uint32_t pin_mask, cvi_pin_mode_t mode);
cvi_error_t cvi_gpio_irq_mode(cvi_gpio_t *gpio, uint32_t pin_mask, cvi_gpio_irq_flag_t mode);
cvi_error_t cvi_gpio_debounce(cvi_gpio_t *gpio, uint32_t pin_mask, bool enable);
uint32_t cvi_gpio_read(cvi_gpio_t *gpio, uint32_t pin_mask);
void cvi_gpio_write(cvi_gpio_t *gpio, uint32_t pin_mask, cvi_gpio_state_t value);
void cvi_gpio_toggle(cvi_gpio_t *gpio, uint32_t pin_mask);
void cvi_gpio_irq_enable(cvi_gpio_t *gpio, uint32_t pin_mask, bool enable);
void cvi_gpio_configure_irq(cvi_gpio_t *gpio, void *callback, void *arg);

#endif
