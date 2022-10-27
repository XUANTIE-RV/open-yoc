
#include <stdio.h>
#include <stdlib.h>
#include <drv/gpio.h>
#ifdef CONFIG_CSI_V1
#include <pinmux.h>
#else
#include <aos/hal/gpio.h>
#endif
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
    gpio_dev_t gpio = { 0, OUTPUT_PUSH_PULL, NULL };

    gpio.port = port;

    ret = hal_gpio_init(&gpio);

    if (ret == 0) {
        if (val) {
            ret = hal_gpio_output_high(&gpio);
        } else {
            ret = hal_gpio_output_low(&gpio);
        }
    }

    hal_gpio_finalize(&gpio);

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
