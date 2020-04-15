/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <app_config.h>
#include <aos/aos.h>
#include <aos/log.h>
#include "app_main.h"

#ifdef AW2013_ENABLED

#define IMAX            0x01    //LED IMAX,0x00=omA,0x01=5mA,0x02=10mA,0x03=15mA,
#define FLASH_RISE_TIME       0x00    //LED rise time,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define FLASH_HOLD_TIME       0x00    //LED max light time light 0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s
#define FLASH_FALL_TIME       0x00    //LED fall time,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define FLASH_OFF_TIME        0x00    //LED off time ,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define BREATH_RISE_TIME       0x04    //LED rise time,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define BREATH_HOLD_TIME       0x02    //LED max light time light 0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s
#define BREATH_FALL_TIME       0x04    //LED fall time,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define BREATH_OFF_TIME        0x05    //LED off time ,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s

#define DELAY_TIME      0x00    //LED Delay time ,0x00=0s,0x01=0.13s,0x02=0.26s,0x03=0.52s,0x04=1.04s,0x05=2.08s,0x06=4.16s,0x07=8.32s,0x08=16.64s
#define PERIOD_NUM      0x00    //LED breath period number,0x00=forever,0x01=1,0x02=2.....0x0f=15

#define AW2013_ADDR         0x45
#define AW2013_I2C_TIMEOUT  200

uint8_t config_regs[3] = {0x31, 0x32, 0x33};    // switch breath mode and PWM mode with the MSB
uint8_t led_dime_regs[3] = {0x34, 0x35, 0x36};
uint8_t rise_hold_regs[3] = {0x37, 0x3a, 0x3d};
uint8_t fall_off_regs[3] = {0x38, 0x3b, 0x3e};
uint8_t delay_regs[3] = {0x39, 0x3c, 0x3f};

#define TAG "AW2013"

static int aw2013_i2c_write_reg(i2c_dev_t *dev, uint8_t addr, uint8_t val)
{
    uint8_t reg_val[2] = {addr, val};

    return hal_i2c_master_send(dev, AW2013_ADDR, reg_val, 2, AW2013_I2C_TIMEOUT);
}

int aw2013_init(i2c_dev_t *i2c_dev)
{
    LOGD(TAG, "start aw2013 config");

    int ret;
    ret = aw2013_i2c_write_reg(i2c_dev, 0x01, 0x01); // enable LED
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw2013_i2c_write_reg(i2c_dev, 0x31, IMAX | 0x70); //config mode, IMAX = 5mA
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    ret = aw2013_i2c_write_reg(i2c_dev, 0x32, IMAX | 0x70); //config mode, IMAX = 5mA
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    ret = aw2013_i2c_write_reg(i2c_dev, 0x33, IMAX | 0x70); //config mode, IMAX = 5mA
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = aw2013_i2c_write_reg(i2c_dev, 0x34, 0x00); // LED0 off
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    ret = aw2013_i2c_write_reg(i2c_dev, 0x35, 0x00); // LED1 off
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    ret = aw2013_i2c_write_reg(i2c_dev, 0x36, 0x00); // LED2 off
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);


    ret = aw2013_i2c_write_reg(i2c_dev, 0x30, 0x07); //led on=0x01 ledoff=0x00
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    LOGD(TAG, "end aw2013 config");
    return 0;

err:
    LOGE(TAG, "aw2013 init failed");
    return -1;
}

int aw2013_low_power(i2c_dev_t *i2c_dev)
{
    int ret;
    
    ret = aw2013_i2c_write_reg(i2c_dev, 0x00, 0x55); //led on=0x01 ledoff=0x00
    CHECK_RET_TAG_WITH_RET(ret == 0, -1);

    return 0;
}

int aw2013_led_control(i2c_dev_t *i2c_dev, led_rgb_t led, led_mode_t mode, uint8_t dime)
{
    CHECK_PARAM(led <= LED_BLUE && led >= LED_RED, -1);
    CHECK_PARAM(mode <= LED_BREATH && mode >= LED_OFF, -1);

    int ret;

    switch(mode) {
        case LED_OFF:
            ret = aw2013_i2c_write_reg(i2c_dev, led_dime_regs[led], 0); // LED0 level,
            CHECK_RET_WITH_RET(ret == 0, -1);
            break;

        case LED_LIGHT:
            ret = aw2013_i2c_write_reg(i2c_dev, config_regs[led], IMAX | 0x00); //config BREATH mode, IMAX = 5mA
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, led_dime_regs[led], dime);
            CHECK_RET_WITH_RET(ret == 0, -1);
            break;

        case LED_FLASH:
            ret = aw2013_i2c_write_reg(i2c_dev, config_regs[led], IMAX | 0x70); //config PWM mode, IMAX = 5mA
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, rise_hold_regs[led], FLASH_RISE_TIME << 4 | FLASH_HOLD_TIME);
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, fall_off_regs[led], FLASH_FALL_TIME << 4 | FLASH_OFF_TIME);
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, delay_regs[led], DELAY_TIME << 4 | PERIOD_NUM);
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, led_dime_regs[led], dime);
            CHECK_RET_WITH_RET(ret == 0, -1);
            break;

        case LED_BREATH:
            ret = aw2013_i2c_write_reg(i2c_dev, config_regs[led], IMAX | 0x70); //config PWM mode, IMAX = 5mA
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, rise_hold_regs[led], BREATH_RISE_TIME << 4 | BREATH_HOLD_TIME);
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, fall_off_regs[led], BREATH_FALL_TIME << 4 | BREATH_OFF_TIME);
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, delay_regs[led], DELAY_TIME << 4 | PERIOD_NUM);
            CHECK_RET_WITH_RET(ret == 0, -1);
            ret = aw2013_i2c_write_reg(i2c_dev, led_dime_regs[led], dime);
            CHECK_RET_WITH_RET(ret == 0, -1);
            break;        

        default:
            break;
    }

    return 0;
}

#endif