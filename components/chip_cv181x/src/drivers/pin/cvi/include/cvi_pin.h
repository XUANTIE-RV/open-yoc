#ifndef __CVI_PIN_H__
#define __CVI_PIN_H__

#include <soc.h>
#include "pin.h"

typedef enum {
    PIN_MODE_PULL_NONE         = 0,    ///< Pull none
    PIN_MODE_PULL_UP,                  ///< Pull up for input
    PIN_MODE_PULL_DOWN,                ///< Pull down for input
    PIN_MODE_OPEN_DRAIN,              ///< Open drain mode for output
    PIN_MODE_PUSH_PULL,               ///< Push-pull mode for output
} cvi_pin_mode_t;

typedef enum {
    CVI_PIN_SPEED_SLOW = 0U,
    CVI_PIN_SPEED_FAST,
} cvi_pin_speed_t;

typedef enum {
    PIN_DRIVE_STRENGTH0 = 0U,
    PIN_DRIVE_STRENGTH1,
} cvi_pin_drive_t;

extern pin_name_t cvi_gpio_to_pin(uint8_t gpio_bank, uint8_t pin_num, const cvi_pinmap_t *pinmap);
extern uint32_t cvi_pin_to_channel(pin_name_t pin_name, const cvi_pinmap_t *pinmap);

__ALWAYS_STATIC_INLINE pin_name_t cvi_pin_get_pinname_by_gpio(uint8_t gpio_bank, uint8_t pinnum)
{
	extern const cvi_pinmap_t cvi_gpio_pinmap[];
    return cvi_gpio_to_pin(gpio_bank, pinnum, cvi_gpio_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t cvi_pin_get_gpio_pinnum(pin_name_t pin_name)
{
    extern const cvi_pinmap_t cvi_gpio_pinmap[];
    return cvi_pin_to_channel(pin_name, cvi_gpio_pinmap);
}

cvi_error_t cvi_pin_set_mux(pin_name_t pin_name, pin_func_t pin_func);
pin_func_t cvi_pin_get_mux(pin_name_t pin_name);
cvi_error_t cvi_pin_set_mode(pin_name_t pin_name, cvi_pin_mode_t mode);
cvi_error_t cvi_pin_set_speed(pin_name_t pin_name, cvi_pin_speed_t speed);
cvi_error_t cvi_pin_set_drive(pin_name_t pin_name, cvi_pin_drive_t drive);

#endif