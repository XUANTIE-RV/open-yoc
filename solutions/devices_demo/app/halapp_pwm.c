/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/pwm.h>
#include <devices/devicelist.h>


extern const csi_pinmap_t pwm_pinmap[];

static uint32_t get_pwm_pin_func(uint8_t gpio_pin, uint8_t pwm_id, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin && map->idx == pwm_id) {
            ret = (uint32_t)map->pin_func;
            break;
        }

        map++;
    }

    return ret;
}

static uint32_t get_pwm_channel(uint8_t gpio_pin, uint8_t pwm_id, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin &&  map->idx == pwm_id) {
            ret = (uint32_t)map->channel;
            break;
        }

        map++;
    }

    return ret;
}

/* output one pwm signal */
int hal_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle,
                     uint32_t freq_chg, float duty_cycle_chg)
{
    int32_t ret;
    uint8_t channel;

    printf(" hal_pwm_out_demo start\r\n");

    uint32_t pin_func = get_pwm_pin_func(gpio_pin, pwm_id, pwm_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    channel = get_pwm_channel(gpio_pin, pwm_id, pwm_pinmap);

    rvm_pwm_drv_register(pwm_id);

    rvm_dev_t *pwm_dev = NULL;
    char filename[8];
    sprintf(filename, "pwm%d", pwm_id);

    pwm_dev = rvm_hal_pwm_open(filename);
    
    rvm_hal_pwm_config_t config;
    config.duty_cycle = duty_cycle;
    config.freq       = freq;
    config.polarity   = RVM_HAL_PWM_POLARITY_NORMAL;
    ret = rvm_hal_pwm_config(pwm_dev, &config, channel);
    if (ret != 0) {
        printf("rvm_hal_pwm_config fail !\n");
        return -1;
    }

    rvm_hal_pwm_config_t config_get;
    rvm_hal_pwm_config_get(pwm_dev, &config_get, channel);
    if ((config_get.duty_cycle != duty_cycle) || (config_get.freq != freq)) {
        printf("rvm_hal_pwm_config_get fail !\n");
        return -1;
    }

    ret = rvm_hal_pwm_start(pwm_dev, channel);
    if (ret != 0) {
        printf("rvm_hal_pwm_start fail !\n");
        return -1;
    }    

    aos_msleep(5000);

#if !defined(CONFIG_CHIP_D1)
    rvm_hal_pwm_config_t config_chg;
    config_chg.duty_cycle = duty_cycle_chg;
    config_chg.freq = freq_chg;
    config_chg.polarity = RVM_HAL_PWM_POLARITY_NORMAL;
    ret = rvm_hal_pwm_config(pwm_dev, &config_chg, channel);
    if (ret != 0) {
        printf("rvm_hal_pwm_config fail !\n");
        return -1;
    }

    aos_msleep(5000);
#endif

    ret = rvm_hal_pwm_stop(pwm_dev, channel);
    if (ret != 0) {
        printf("rvm_hal_pwm_stop fail !\n");
        return -1;
    }

    printf("hal_pwm_out_demo end\r\n");

    return 0;
}

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle,
                       uint32_t freq_chg, float duty_cycle_chg)
{
    aos_msleep(1000);

    int32_t ret;

    printf(" devfs_pwm_out_demo start\r\n");

    uint32_t pin_func = get_pwm_pin_func(gpio_pin, pwm_id, pwm_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    uint8_t channel = get_pwm_channel(gpio_pin, pwm_id, pwm_pinmap);

    rvm_pwm_drv_register(pwm_id);

    char pwmdev[16];
    sprintf(pwmdev, "/dev/pwm%d", pwm_id);

    int fd = open(pwmdev, O_RDWR);
    printf("open rtc0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", pwmdev, fd);
        return -1;
    }

    rvm_hal_pwm_config_t config;
    config.duty_cycle = duty_cycle;
    config.freq       = freq;
    config.polarity = RVM_HAL_PWM_POLARITY_NORMAL;
    rvm_pwm_dev_msg_t msg_pwm;
    msg_pwm.config = config;
    msg_pwm.channel = channel;
    ret = ioctl(fd, PWM_IOC_CONFIG, &msg_pwm);
    if (ret < 0) {
        printf("PWM_IOC_CONFIG msg_pwm fail !\n");
        goto failure;
    }

    rvm_pwm_dev_msg_t msg_pwm_cfg; 
    msg_pwm_cfg.channel = channel;
    ret = ioctl(fd, PWM_IOC_CONFIG_GET, &msg_pwm_cfg);
    if ((msg_pwm_cfg.config.duty_cycle != duty_cycle) || (msg_pwm_cfg.config.freq != freq)) {
        printf("PWM_IOC_CONFIG_GET fail !\n");
        return -1;
    }

    ret = ioctl(fd, PWM_IOC_START, &channel);
    if (ret < 0) {
        printf("PWM_IOC_START fail !\n");
        goto failure;
    }

    aos_msleep(5000);

#if !defined(CONFIG_CHIP_D1)
    rvm_hal_pwm_config_t config_chg;
    config_chg.duty_cycle = duty_cycle_chg;
    config_chg.freq = freq_chg;
    config_chg.polarity = RVM_HAL_PWM_POLARITY_NORMAL;
    rvm_pwm_dev_msg_t msg_pwm_chg;
    msg_pwm_chg.config = config_chg;
    msg_pwm_chg.channel = channel;
    ret = ioctl(fd, PWM_IOC_CONFIG, &msg_pwm_chg);
    if (ret < 0) {
        printf("PWM_IOC_CONFIG msg_pwm_ch fail !\n");
        goto failure;
    }

    aos_msleep(5000);
#endif

    ret = ioctl(fd, PWM_IOC_STOP, &channel);
    if (ret < 0) {
        printf("PWM_IOC_STOP fail !\n");
        goto failure;
    }

    close(fd);
    printf("devfs pwm demo successfully !\n");
    return 0;
failure:
    close(fd);
    return -1;
}

#endif
