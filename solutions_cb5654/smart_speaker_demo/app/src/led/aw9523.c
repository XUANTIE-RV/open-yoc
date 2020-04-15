/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <app_config.h>
#include <aos/aos.h>
#include <aos/log.h>
#include <pinmux.h>
#include <pin_name.h>
#include <drv/gpio.h>
#include "app_main.h"

#ifdef AW9523_ENABLED

/* reg define */
#define P0_INPUT		0x00
#define P1_INPUT 		0x01
#define P0_OUTPUT 		0x02
#define P1_OUTPUT 		0x03
#define P0_CONFIG		0x04
#define P1_CONFIG 		0x05
#define P0_INT			0x06
#define P1_INT			0x07
#define ID_REG			0x10
#define CTL_REG			0x11
#define P0_LED_MODE		0x12
#define P1_LED_MODE		0x13
#define P1_0_DIM0		0x20
#define P1_1_DIM0		0x21
#define P1_2_DIM0		0x22
#define P1_3_DIM0		0x23
#define P0_0_DIM0		0x24
#define P0_1_DIM0		0x25
#define P0_2_DIM0		0x26
#define P0_3_DIM0		0x27
#define P0_4_DIM0		0x28
#define P0_5_DIM0		0x29
#define P0_6_DIM0		0x2A
#define P0_7_DIM0		0x2B
#define P1_4_DIM0		0x2C
#define P1_5_DIM0		0x2D
#define P1_6_DIM0		0x2E
#define P1_7_DIM0		0x2F
#define SW_RSTN			0x7F

#define AW9523_ADDR 0x5B
#define AW9523_I2C_TIMEOUT  200
#define LED_DEFAULT_DIM     0x05

#define TAG "AW9523"

#ifdef BOARD_MIT_V3
const static int led_map[] = {
    0,
    1,
    2,
    3,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,    
};
#else
const static int led_map[] = {
    6,
    5,
    4,
    3,
    2,
    15,
    1,
    0,
    11,
    10,
    9,
    8,    
};
#endif


static int aw9523_i2c_write_reg(i2c_dev_t *dev, uint8_t addr, uint8_t val)
{
    uint8_t reg_val[2] = {addr, val};

    return hal_i2c_master_send(dev, AW9523_ADDR, reg_val, 2, AW9523_I2C_TIMEOUT);
}


int aw9523_init(i2c_dev_t *i2c_dev)
{
    LOGD(TAG, "start aw9523 config");

    gpio_pin_handle_t hdl_reset;
    drv_pinmux_config(AW9523_RST_PIN, PIN_FUNC_GPIO);
    hdl_reset = csi_gpio_pin_initialize(AW9523_RST_PIN, NULL);
    csi_gpio_pin_config_mode(hdl_reset, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(hdl_reset, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(hdl_reset, 0);
    aos_msleep(1);
    csi_gpio_pin_write(hdl_reset, 1);
    csi_gpio_pin_uninitialize(hdl_reset);
    aos_msleep(20);

#ifdef BOARD_MIT_V3
    int ret;
	ret = aw9523_i2c_write_reg(i2c_dev, P0_LED_MODE, 0x20);				// 0xf0 P0_4/5/6 as GPIO. else as LED		
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

	ret = aw9523_i2c_write_reg(i2c_dev, P1_LED_MODE, 0x00);				// P1 as LED
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

	ret = aw9523_i2c_write_reg(i2c_dev, CTL_REG, 0x13);				// 00:Imax, 01:3/4Imax, 02: 1/2Imax, 03:1/4Imax
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw9523_i2c_write_reg(i2c_dev, P0_CONFIG, 0x50);                  /* P0_4/6 input, P0_5/7 output */
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    
    ret = aw9523_i2c_write_reg(i2c_dev, P0_INT, 0xFF);                     // 0xAF enable P0_4/6 port interrupt 
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw9523_i2c_write_reg(i2c_dev, P0_OUTPUT, 0x20);                  /* P0_5 high  */
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
#else
    int ret;
	ret = aw9523_i2c_write_reg(i2c_dev, P0_LED_MODE, 0x80);				// 0x80, P0_7 as GPIO. else as LED		
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

	ret = aw9523_i2c_write_reg(i2c_dev, P1_LED_MODE, 0x70);				// P1_4 P1_5 P1_6 as GPIO, else as LED
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

	ret = aw9523_i2c_write_reg(i2c_dev, CTL_REG, 0x03);				// 00:Imax, 01:3/4Imax, 02: 1/2Imax, 03:1/4Imax
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw9523_i2c_write_reg(i2c_dev, P0_CONFIG, 0x00);                  /* P0 output mode */
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    ret = aw9523_i2c_write_reg(i2c_dev, P1_CONFIG, 0x60);                  /* P1_5 P1_6 input mode */
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    
    ret = aw9523_i2c_write_reg(i2c_dev, P0_INT, 0xff);                     //disable P0 port interrupt 
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    ret = aw9523_i2c_write_reg(i2c_dev, P1_INT, 0xbf);                     //enable P1_6 port interrupt, EAR_INT 
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw9523_i2c_write_reg(i2c_dev, P0_OUTPUT, 0x00);                  /* P0 low*/
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw9523_i2c_write_reg(i2c_dev, P1_OUTPUT, 0x10);                  /* P1_4 high*/
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

#endif

    LOGD(TAG, "end aw9523 config");
    return 0;

err:
    LOGE(TAG, "aw9523 init failed");
    return -1;
}

int aw9523_shutdown(void)
{
    gpio_pin_handle_t hdl_reset;
    drv_pinmux_config(AW9523_RST_PIN, PIN_FUNC_GPIO);
    hdl_reset = csi_gpio_pin_initialize(AW9523_RST_PIN, NULL);
    csi_gpio_pin_config_mode(hdl_reset, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(hdl_reset, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(hdl_reset, 0);
    csi_gpio_pin_uninitialize(hdl_reset);

    return 0;
}

int aw9523_led_control(i2c_dev_t *i2c_dev, led_array_id_t index, uint8_t dime)
{
    CHECK_PARAM(index <= LED_ARRAY_12 && index >= LED_ARRAY_1, -1);

    int ret = -1;
    int led_id = led_map[index];

    if(led_id < 8){
        ret = aw9523_i2c_write_reg(i2c_dev, P0_0_DIM0 + led_id, dime);
    } else if(led_id < 12){
        ret = aw9523_i2c_write_reg(i2c_dev, P1_0_DIM0 + led_id - 8, dime);
    } else if(led_id < 16){
        ret = aw9523_i2c_write_reg(i2c_dev, P1_4_DIM0 + led_id - 12, dime);
    }

    return ret;
}

static int aw9523_iic_read_reg(i2c_dev_t *i2c_dev, uint8_t reg_addr, uint8_t val[], uint8_t num)
{
    int     ret;
    uint8_t wbuf[1] = {
        reg_addr,
    };

    ret = hal_i2c_master_send(i2c_dev, AW9523_ADDR, wbuf, 1, AW9523_I2C_TIMEOUT);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = hal_i2c_master_recv(i2c_dev, AW9523_ADDR, val, num, AW9523_I2C_TIMEOUT);
    CHECK_RET_WITH_RET(ret == 0, -1);

    return ret;
}

int aw9523_read_input(i2c_dev_t *i2c_dev, uint16_t *val, uint16_t input_mask)
{
    uint8_t lbyte = 0, hbyte = 0;
    int ret;

    if (input_mask & 0x00ff) {
        ret = aw9523_iic_read_reg(i2c_dev, P0_INPUT, &lbyte, 1);
        CHECK_RET_WITH_RET(ret == 0, -1);
    }

    if (input_mask & 0xff00) {
        ret = aw9523_iic_read_reg(i2c_dev, P1_INPUT, &hbyte, 1);
        CHECK_RET_WITH_RET(ret == 0, -1);
    }

    *val = (lbyte | (hbyte << 8)) & input_mask;
    return 0;
}

#endif