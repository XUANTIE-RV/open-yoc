
#include <stdio.h>
#include <stdlib.h>
#include <drv/gpio.h>
#ifdef CONFIG_CSI_V1
#include <pinmux.h>
#else
#include <devices/gpiopin.h>
#include <devices/devicelist.h>
#endif /*CONFIG_CSI_V1*/
#include <aos/kernel.h>

#include "drv_amp.h"
#include "drv_amp_ops.h"

/******************************************
 * Defalt PA PIN Control
 *****************************************/
#ifndef CONFIG_CSI_V1
static int32_t gpio_output_set(int port, int val)
{
    int32_t    ret  = -1;
    rvm_dev_t *gpio_pin;
    rvm_gpio_pin_drv_register(port);
    gpio_pin = rvm_hal_gpio_pin_open_by_pin_name("gpio_pin", port);
    if (gpio_pin) {
        rvm_hal_gpio_pin_set_mode(gpio_pin, RVM_GPIO_MODE_PULLNONE);
        if (val) {
            ret = rvm_hal_gpio_pin_write(gpio_pin, RVM_GPIO_PIN_HIGH);
        } else {
            ret = rvm_hal_gpio_pin_write(gpio_pin, RVM_GPIO_PIN_LOW);
        }
        rvm_hal_gpio_pin_close(gpio_pin);
    }
    return ret;
}
#endif

static int def_amplifier_init(amplifier_pa_priv_t *priv)
{
    return 0;
}

static int def_amplifier_uninit(amplifier_pa_priv_t *priv)
{

#ifndef CONFIG_CSI_V1
    if (priv->pa_ctrl_pin >= 0) {
        gpio_output_set(priv->pa_ctrl_pin, 0);
    }
    if (priv->pa_power_pin >= 0) {
        gpio_output_set(priv->pa_power_pin, 0);
    }
#endif
    return 0;
}

static int def_amplifier_onoff(amplifier_pa_priv_t *priv, int onoff, int amp_mode)
{
#ifdef CONFIG_CSI_V1
    int ret = csi_gpio_pin_write(priv->pa_mute_hdl, onoff);
#else
    int ret = gpio_output_set(priv->pa_ctrl_pin, onoff);
#endif
    return ret;
}

const struct amplifier_pa_ops g_padef_ops = {.name         = "gpioamp",
                                             .init         = def_amplifier_init,
                                             .uninit       = def_amplifier_uninit,
                                             .onoff        = def_amplifier_onoff,
                                             .probe        = NULL,
                                             .cfgbin_read  = NULL,
                                             .cfgbin_write = NULL,
                                             .getid        = NULL,
                                             .reset        = NULL};
