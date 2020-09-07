/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     dw_gpio.c
 * @brief    CSI Source File for GPIO Driver
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include "drv/gpio.h"
#include "drv/irq.h"
#include "drv/pmu.h"
#include "dw_gpio.h"
#include "csi_core.h"
#include "pin_name.h"
#include <aos/aos.h>

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define ERR_GPIO(errno)  (CSI_DRV_ERRNO_GPIO_BASE | errno)
#define GPIO_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_GPIO(DRV_ERROR_PARAMETER))

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef void *gpio_port_handle_t;

typedef struct {
#ifdef CONFIG_LPM
    uint8_t gpio_power_status;
    uint32_t gpio_regs_saved[4];
#endif
    uint32_t base;              ///< handle register base
    uint32_t cpu;               ///< cpu number
    uint32_t irq;               ///< irq of this handle
    uint32_t pin_num;           ///< pin number of this handle
    gpio_mode_e mode;           ///< gpio mode
    gpio_direction_e dir;       ///< gpio direction
    uint32_t mask;              ///< gpio mask pin
    uint32_t value;             ///< gpio value
} dw_gpio_priv_t;

typedef struct {
    uint8_t     portidx;
    uint8_t     pinidx;
    uint8_t     offset;
    gpio_event_cb_t cb;
} dw_gpio_pin_priv_t;

/************************************************************************************
 * Private Function Prototypes
 ************************************************************************************/

extern int32_t target_gpio_port_init(port_name_e port, uint32_t *base, uint32_t *cpu, uint32_t *irq, void **handler, uint32_t *pin_num);
extern int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx);
extern int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode);

/************************************************************************************
 * Private Data
 ************************************************************************************/

static dw_gpio_priv_t gpio_handle[CONFIG_GPIO_NUM];
static dw_gpio_pin_priv_t gpio_pin_handle[CONFIG_GPIO_PIN_NUM];

static const uint32_t g_song_gpio_inttype[] =
{
    [GPIO_IRQ_MODE_HIGH_LEVEL]   =  SONG_GPIO_INT_HIGHLEVEL,
    [GPIO_IRQ_MODE_LOW_LEVEL]    =  SONG_GPIO_INT_LOWLEVEL,
    [GPIO_IRQ_MODE_RISING_EDGE]  =  SONG_GPIO_INT_GPIOCLK_RISING,
    [GPIO_IRQ_MODE_FALLING_EDGE] =  SONG_GPIO_INT_GPIOCLK_FALLING,
    [GPIO_IRQ_MODE_DOUBLE_EDGE]  =  SONG_GPIO_INT_GPIOCLK_BOTHEDGES,
};

/************************************************************************************
 * Private Functions
 ************************************************************************************/

static inline uint32_t readreg(dw_gpio_priv_t *priv, uint32_t pinidx)
{
    return (*(volatile uint32_t *)(priv->base + pinidx));
}

static inline void writereg(dw_gpio_priv_t *priv, uint32_t pinidx, uint32_t val)
{
    *(volatile uint32_t *)(priv->base + pinidx) = val;
}

static int32_t gpio_set_direction(void *port, gpio_direction_e direction
)
{
    dw_gpio_priv_t *gpio_priv = port;
    uint32_t bit, val;

    bit = gpio_priv->mask % 16;
    val = 1 << (bit + 16);

    if (direction == GPIO_DIRECTION_OUTPUT) {
        val |= 1 << bit;
    }

    writereg(gpio_priv, SONG_GPIO_PORT_DDR(gpio_priv->mask), val);

    return 0;
}

/*
 * Read the statu of the Port choosed.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 * return: the value of the corresponding Port.
 */

static int32_t gpio_read(void *port, uint32_t *value)
{
    dw_gpio_priv_t *gpio_priv = port;

    *value = readreg(gpio_priv, SONG_GPIO_EXT_PORT(gpio_priv->mask));

    return 0;
}


/*
 * Write an output value to corresponding Port.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 *   output: value that will be written to the corresponding Port.
 * return: SUCCESS
 */

static int32_t gpio_write(void *port, uint32_t mask)
{
    dw_gpio_priv_t *gpio_priv = port;

    uint32_t bit, val;

    bit = mask % 16;
    val = (1 << (bit + 16));
    if (gpio_priv->value) {
        val |= 1 << bit;
    }

    writereg(gpio_priv, SONG_GPIO_PORT_DR(gpio_priv->mask), val);
    return 0;
}

/*
 * Disable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void gpio_irq_disable(gpio_pin_handle_t handle)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;
    uint8_t idx = gpio_pin_priv->pinidx;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

    uint32_t inttype = SONG_GPIO_INT_DISABLE;
    uint32_t reg, bit;

    bit = idx % 4;
    reg = (inttype << (bit * 4)) | (1 << (bit + 16));
    writereg(port_handle, SONG_GPIO_INTR_CTRL(idx), reg);

    /* unmask it */
    bit = idx % 16;
    reg = 1 << (bit + 16);
    writereg(port_handle, SONG_GPIO_INTR_MASK(port_handle->cpu, idx), reg);
}

void dw_gpio_irqhandler(int idx)
{
    if (idx >= CONFIG_GPIO_NUM) {
        return;
    }
    uint8_t i, j;
    uint32_t status = 0;
    uint32_t start_pinidx = 0;

    if (idx > 0) {
        for (j = 0; j < idx; j++) {
            start_pinidx += gpio_handle[j].pin_num;
        }
    }

    if (start_pinidx >= CONFIG_GPIO_PIN_NUM) {
        return;
    }

    dw_gpio_priv_t *port_handle = &gpio_handle[idx];

    for (i = start_pinidx; i < start_pinidx + gpio_handle->pin_num; i += 32){

        status = readreg(port_handle, SONG_GPIO_INTR_STATUS(gpio_handle->cpu, i));

        for (j = 0; j < 32; j++) {
            if (status & (1U << j)) {
                uint32_t pinidx = i + j;
                if (pinidx >= CONFIG_GPIO_PIN_NUM) {
                    break;
                }
                dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)&gpio_pin_handle[pinidx];

                /* execute the callback function */
                if ((gpio_event_cb_t)(gpio_pin_priv->cb)) {
                    ((gpio_event_cb_t)(gpio_pin_priv->cb))(gpio_pin_priv->pinidx);
                }
            }
        }

        if (status) {
            writereg(port_handle, SONG_GPIO_INTR_CLR(i), status);
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
    uint32_t base = 0;
    uint32_t cpu = 0;
    uint32_t pin_num;
    uint32_t irq;
    void *handler;
    int32_t idx = target_gpio_port_init(port, &base, &cpu, &irq, &handler, &pin_num);

    if (idx < 0 || idx >= CONFIG_GPIO_NUM) {
        return NULL;
    }

    gpio_priv = &gpio_handle[idx];

    gpio_priv->base = base;
    gpio_priv->cpu = cpu;
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
    uint8_t device[] = {CLOCK_MANAGER_GPIO0};

    drv_clock_manager_config(device[gpio_pin_priv->portidx], enable);
}

static void do_prepare_sleep_action(void *handle)
{
    dw_gpio_priv_t *gpio_handle = handle;
    uint32_t *gbase = (uint32_t *)(gpio_handle->base);
    registers_save(gpio_handle->gpio_regs_saved, gbase + SONG_GPIO_PORT_DR(gpio_handle->mask), 1);
    registers_save(&gpio_handle->gpio_regs_saved[1], gbase + SONG_GPIO_PORT_DDR(gpio_handle->mask), 1);
    registers_save(&gpio_handle->gpio_regs_saved[2], gbase + SONG_GPIO_INTR_CTRL(gpio_handle->mask), 1);
    registers_save(&gpio_handle->gpio_regs_saved[3], gbase + SONG_GPIO_INTR_MASK(gpio_handle->cpu, gpio_handle->mask), 1);
}

static void do_wakeup_sleep_action(void *handle)
{
    dw_gpio_priv_t *gpio_handle = handle;
    uint32_t *gbase = (uint32_t *)(gpio_handle->base);
    registers_restore(gbase + SONG_GPIO_PORT_DR(gpio_handle->mask), gpio_handle->gpio_regs_saved, 1);
    registers_restore(gbase + SONG_GPIO_PORT_DDR(gpio_handle->mask), &gpio_handle->gpio_regs_saved[1], 1);
    registers_restore(gbase + SONG_GPIO_INTR_CTRL(gpio_handle->mask), &gpio_handle->gpio_regs_saved[2], 1);
    registers_restore(gbase + SONG_GPIO_INTR_MASK(gpio_handle->.cpu, gpio_handle->mask), &gpio_handle->gpio_regs_saved[3], 1);
}
#endif

/**
  \brief       Initialize GPIO handle.
  \param[in]   gpio_pin    Pointer to the int32_t.
  \param[in]   cb_event    Pointer to \ref gpio_event_cb_t.
  \param[in]   arg    Pointer to \ref arg used for the callback.
  \return      gpio_pin_handle
*/
gpio_pin_handle_t csi_gpio_pin_initialize(int32_t gpio_pin, gpio_event_cb_t cb_event)
{
    static int8_t port_init_status = 0;

    if (gpio_pin < 0 || gpio_pin >= CONFIG_GPIO_PIN_NUM) {
        return NULL;
    }

    uint32_t i;

    if (!port_init_status) {
        for (i = 0; i < CONFIG_GPIO_NUM; i++) {
            csi_gpio_port_initialize(i);
        }
        port_init_status = 1;
    }

    /* obtain the gpio pin information */
    uint32_t port_idx = 0;
    uint32_t pin_num = 0;

    for (port_idx = 0; port_idx < CONFIG_GPIO_NUM; port_idx++) {
        if(gpio_pin < gpio_handle[port_idx].pin_num) {
            break;
        }
        else {
            pin_num += gpio_handle[port_idx].pin_num;
        }
    }

    if (port_idx >= CONFIG_GPIO_NUM) {
        return NULL;
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[gpio_pin]);
    gpio_pin_priv->portidx = port_idx;


    gpio_pin_priv->pinidx = gpio_pin;
    gpio_pin_priv->cb = cb_event;
    gpio_pin_priv->offset = gpio_pin - pin_num;

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

    gpio_irq_disable(handle);

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
    GPIO_NULL_PARAM_CHK(handle);

#ifdef CONFIG_LPM
    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)handle;
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(&gpio_handle[gpio_pin_priv->portidx], state, &callback);
#else
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
#endif
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

    uint8_t idx = gpio_pin_priv->pinidx;

    int32_t ret = drv_pin_config_mode(gpio_pin_priv->portidx, idx, mode);

    return ret;
}
/**
  \brief       config pin direction
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config_direction(gpio_pin_handle_t handle, gpio_direction_e dir)
{
    GPIO_NULL_PARAM_CHK(handle);

    /* config the gpio pin mode direction mask bits */
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t *gpio_priv = &gpio_handle[gpio_pin_priv->portidx];

    gpio_priv->dir = dir;
    gpio_priv->mask = gpio_pin_priv->pinidx;

    uint32_t ret = gpio_set_direction(gpio_priv, dir);

    if (ret) {
        return ret;
    }

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
    gpio_priv->mask = gpio_pin_priv->pinidx;

    uint32_t ret = gpio_set_direction(gpio_priv, dir);

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

    uint8_t idx = gpio_pin_priv->pinidx;
    uint32_t port_value = value;

    port_handle->mask = idx;
    port_handle->value = port_value;
    gpio_write(port_handle, idx);

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
    uint32_t port_value;
    uint8_t idx = gpio_pin_priv->pinidx;

    /* convert portidx to port handle */
    dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];
    port_handle->mask = idx;

    gpio_read(port_handle, &port_value);
    *value = ((port_value & (1 << (idx % 32))) != 0);

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

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    uint8_t idx = gpio_pin_priv->pinidx;

    if (enable) {

        /* convert portidx to port handle */
        dw_gpio_priv_t *port_handle = &gpio_handle[gpio_pin_priv->portidx];

        uint32_t inttype = g_song_gpio_inttype[mode];
        uint32_t reg, bit;

        bit = idx % 4;
        reg = (inttype << (bit * 4)) | (1 << (bit + 16));
        writereg(port_handle, SONG_GPIO_INTR_CTRL(idx), reg);

        /* unmask it */
        bit = idx % 16;
        reg = 1 << (bit + 16);
        writereg(port_handle, SONG_GPIO_INTR_MASK(port_handle->cpu, idx), reg);


    } else {
        gpio_irq_disable(handle);
    }

    return 0;

}

