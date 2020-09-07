/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     silan_gpio.c
 * @brief    CSI Source File for GPIO Driver
 * @version  V1.0
 * @date     17. August 2018
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <soc.h>
#include <drv/gpio.h>
#include <drv/pmu.h>
#include <silan_gpio.h>
#include <silan_pic.h>
#include <csi_core.h>
#include <pin_name.h>
#include <pinmux.h>

extern int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode);

#define ERR_GPIO(errno) (CSI_DRV_ERRNO_GPIO_BASE | errno)
#define GPIO_NULL_PARAM_CHK(para)                         \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_GPIO(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

#define GPIO_CONFIG_OFFSET(x)    (x / (GPIO1_MAX_NUM+1))
#define GPIO_CONFIG_PIN(x)       (x % (GPIO1_MAX_NUM+1))
#define IO_TO_GPIO(x)            ((x>=8) ? (x-8) : (34+x))

typedef void *gpio_port_handle_t;

typedef struct {
    uint32_t base;              ///< handle register base
    uint32_t irq;               ///< irq of this handle
    uint32_t pin_num;           ///< pin number of this handle
    gpio_mode_e mode;           ///< gpio mode
    gpio_direction_e dir;       ///< gpio direction
    uint32_t mask;              ///< gpio mask bit
    uint32_t value;             ///< gpio value
} silan_gpio_priv_t;

typedef struct {
    uint8_t     portidx;
    uint8_t     idx;
    gpio_event_cb_t cb;
} silan_gpio_pin_priv_t;

extern int32_t target_gpio_port_init(port_name_e port, uint32_t *base, uint32_t *irq, uint32_t *pin_num);
extern int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx);

static silan_gpio_priv_t gpio_handle[CONFIG_GPIO_NUM];
static silan_gpio_pin_priv_t gpio_pin_handle[CONFIG_GPIO_PIN_NUM];

static inline void silan_gpio_control_mode(uint32_t addr, uint32_t pin, int32_t control)
{
   if (control == GPIO_HARDWARE_CONTROL) {
       __sREG32(addr, GPIO_AFSEL) |= pin;
   } else if (control == GPIO_SOFTWARE_CONTROL) {
       __sREG32(addr, GPIO_AFSEL) &= ~pin;
   }
}

static inline void silan_gpio_input(uint32_t addr, uint32_t pin)
{
    __sREG32(addr, GPIO_DIR) &= ~pin;
}

static inline void silan_gpio_output(uint32_t addr, uint32_t pin)
{
    __sREG32(addr, GPIO_DIR) |= pin;
}

static inline void silan_gpio_set_high(uint32_t addr, uint32_t pin)
{
    __sREG32(addr, GPIO_DATA) |= pin;
}

static inline void silan_gpio_set_low(uint32_t addr, uint32_t pin)
{
    __sREG32(addr, GPIO_DATA) &= ~pin;
}

static inline int silan_gpio_get(uint32_t addr, uint32_t pin)
{
    if (__sREG32(addr, GPIO_DATA) & pin) {
        return 1;
    }
    return 0;
}

static inline int silan_get_interrupt_which_gpio_pin(uint32_t addr, uint32_t pin)
{
    if (__sREG32(addr, GPIO_MIS) & pin) {
        return 1;
    }
    return 0;
}

static inline void silan_gpio_irq_enable(uint32_t addr, uint32_t pin)
{
    __sREG32(addr, GPIO_IE) |= pin;
}

static inline void silan_gpio_irq_clear(uint32_t addr, uint32_t pin)
{
    __sREG32(addr, GPIO_IC) |= pin;
}

static inline void silan_gpio_irq_mode(uint32_t addr, uint32_t pin, int mode)
{
    uint32_t base = addr;

    if (mode == GPIO_IRQ_MODE_LOW_LEVEL)
    {
        __sREG32(base,GPIO_IS) |= pin;
    }
    else
    {
        __sREG32(base,GPIO_IS) &= ~pin;
        if (mode == GPIO_IRQ_MODE_DOUBLE_EDGE)
        {
            __sREG32(base,GPIO_IBE) |= pin;
        }
    }
    if ((mode == GPIO_IRQ_MODE_HIGH_LEVEL) || (mode == GPIO_IRQ_MODE_RISING_EDGE))
    {
        __sREG32(base,GPIO_IEV) |= pin;
    }
    else
    {
        __sREG32(base,GPIO_IEV) &= ~pin;
    }
}

static inline void silan_gpio_filsel_open(uint32_t addr,uint32_t pin)
{
    __sREG32(addr, GPIO_FILSEL) |= pin;
}

static inline void silan_gpio_filsel_close(uint32_t addr,uint32_t pin)
{
    __sREG32(addr, GPIO_FILSEL) &= ~pin;
}

static inline void silan_gpio_filsel_div(uint32_t addr,int div)
{
    __sREG32(addr, GPIO_FILDIV) |= ((div & 0xf) << 1) | 0x1;
}

static void silan_io_pullup_config(unsigned int mod,unsigned int func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
    int io_reg;

    reg_bias = mod / 4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE + ((SILAN_IOMUX_BIAS_ADDR + reg_bias) << 2));
    io_attr.d8 = (io_reg >> (data_bias*8)) & 0xFF;
    io_attr.b.ren = func;
    io_reg &= ~(0xFF << (data_bias*8));
    io_reg |=  ((io_attr.d8) << (data_bias*8));
    __REG32(SILAN_SYSCFG_BASE + ((SILAN_IOMUX_BIAS_ADDR + reg_bias) << 2)) = io_reg;
}

/**
  \brief       Initialize GPIO module. 1. Initializes the resources needed for the GPIO handle 2.registers event callback function
                3.get gpio_port_handle
  \param[in]   port      port_name.
  \return      gpio_port_handle
*/
gpio_port_handle_t csi_gpio_port_initialize(int32_t port)
{
    silan_gpio_priv_t *gpio_priv = NULL;

    /* obtain the gpio port information */
    uint32_t base = 0u;
    uint32_t pin_num;
    uint32_t irq;
    int32_t idx = target_gpio_port_init(port, &base, &irq, &pin_num);

    if (idx < 0 || idx >= CONFIG_GPIO_NUM) {
        return NULL;
    }

    gpio_priv = &gpio_handle[idx];

    gpio_priv->base = base;
    gpio_priv->irq  = irq;
    gpio_priv->pin_num  = pin_num;

    return (gpio_port_handle_t)gpio_priv;
}

void silan_gpio_irqhandler(int idx)
{
    if (idx >= CONFIG_GPIO_PIN_NUM) {
        return;
    }

    int32_t offset, pin, gpio, i;

    for (i = PA0; i <= PD10; i++) {
        gpio = IO_TO_GPIO(i);
        offset = GPIO_CONFIG_OFFSET(gpio);
        pin = GPIO_CONFIG_PIN(gpio);
        if (silan_get_interrupt_which_gpio_pin(SILAN_GPIO_BASE(offset), GPIO_PIN(pin))) {
            break;
        }
    }

    if (i >= CONFIG_GPIO_PIN_NUM) {
        return;
    }

    silan_gpio_pin_priv_t *gpio_pin_priv = &gpio_pin_handle[i];

    if ((gpio_event_cb_t)(gpio_pin_priv->cb)) {
        ((gpio_event_cb_t)(gpio_pin_priv->cb))(i);
    }

    silan_gpio_irq_clear(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
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

    silan_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[pin_idx]);
    gpio_pin_priv->portidx = port_idx;

    gpio_pin_priv->idx = pin_idx;
    gpio_pin_priv->cb = cb_event;

    int32_t pin, gpio;

    gpio = IO_TO_GPIO(gpio_pin_priv->idx);
    pin = GPIO_CONFIG_PIN(gpio);
    silan_pic_request(port_idx + PIC_IRQID_GPIO1, pin, (hdl_t)silan_gpio_irqhandler);
    return (gpio_pin_handle_t)gpio_pin_priv;
}

/**
  \brief       De-initialize GPIO pin handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio pin handle to operate.
  \return      error code
*/
int32_t csi_gpio_pin_uninitialize(gpio_pin_handle_t handle)
{
    GPIO_NULL_PARAM_CHK(handle);

    silan_gpio_pin_priv_t *gpio_pin_priv = handle;
    silan_pic_free(gpio_pin_priv->portidx + PIC_IRQID_GPIO1, gpio_pin_priv->idx);
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
    silan_gpio_pin_priv_t *gpio_pin_priv = handle;

    if (mode == GPIO_MODE_PULLUP) {
        silan_io_pullup_config(gpio_pin_priv->idx, 0);
        return 0;
    } else if (mode == GPIO_MODE_PULLNONE) {
        silan_io_pullup_config(gpio_pin_priv->idx, 1);
        return 0;
    } else if (mode == GPIO_MODE_PUSH_PULL) {
        return 0;
    } else {
        return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
    }
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
    GPIO_NULL_PARAM_CHK(handle);

    int32_t offset, pin, gpio;
    /* config the gpio pin mode direction mask bits */
    silan_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    silan_gpio_priv_t *gpio_priv = &gpio_handle[gpio_pin_priv->portidx];

    gpio_priv->dir = dir;

    gpio = IO_TO_GPIO(gpio_pin_priv->idx);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    if (dir == GPIO_DIRECTION_INPUT) {
        silan_gpio_input(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    } else if (dir == GPIO_DIRECTION_OUTPUT){
        silan_gpio_output(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    } else {
        return ERR_GPIO(GPIO_ERROR_DIRECTION);
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
    csi_gpio_pin_config_mode(handle, mode);
    csi_gpio_pin_config_direction(handle, dir);

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

    silan_gpio_pin_priv_t *gpio_pin_priv = handle;

    int32_t offset, pin, gpio;

    gpio = IO_TO_GPIO(gpio_pin_priv->idx);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    if (value) {
        silan_gpio_set_high(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    } else {
        silan_gpio_set_low(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    }

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

    if ((uint32_t)value <= 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    silan_gpio_pin_priv_t *gpio_pin_priv = handle;

    int32_t offset, pin, gpio;

    gpio = IO_TO_GPIO(gpio_pin_priv->idx);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    *value = silan_gpio_get(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));

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
    if (mode > GPIO_IRQ_MODE_HIGH_LEVEL) {
        return  ERR_GPIO(GPIO_ERROR_IRQ_MODE);
    }
    silan_gpio_pin_priv_t *gpio_pin_priv = handle;

    int32_t offset, pin, gpio;
    gpio = IO_TO_GPIO(gpio_pin_priv->idx);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    if (enable) {
        silan_gpio_irq_mode(SILAN_GPIO_BASE(offset), GPIO_PIN(pin), mode);
        silan_gpio_irq_enable(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    } else {
        silan_gpio_irq_clear(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    }

    return 0;
}
