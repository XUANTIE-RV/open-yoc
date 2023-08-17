/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/cli.h>
#include <aos/kernel.h>
#include <devices/pwm.h>
#include <devices/devicelist.h>

#define PWM_PORT_MASK   0x3F

/* output one pwm signal */
static void pwm_output(uint8_t port, uint32_t freq, float duty_cycle)
{
    rvm_dev_t *pwm;
    rvm_hal_pwm_config_t config;

    printf(" hal_pwm_app_static_out start\r\n");

    uint8_t id = (port >> 6) & 0x03;
    int channel = port & PWM_PORT_MASK;
    char name[12];

    rvm_pwm_drv_register(id);
    snprintf(name, sizeof(name), "pwm%d", id);
    pwm = rvm_hal_pwm_open(name);
    if(pwm == NULL){
        printf("rvm_hal_pwm_open(%s) fail.\r\n", name);
        return;
    }
    config.freq = freq;
    config.duty_cycle = duty_cycle;
    config.polarity = RVM_HAL_PWM_POLARITY_NORMAL;
    rvm_hal_pwm_config(pwm, &config, channel);
    rvm_hal_pwm_start(pwm, channel);

    aos_msleep(3000);

    rvm_hal_pwm_stop(pwm, channel);

    rvm_hal_pwm_close(pwm);

    printf("hal_pwm_app_static_out end\r\n");
}


static void cmd_pwm_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 4) {
        printf("usage:\n");
        printf("\tpwm port freq dutycycle[0~100]\n");
        return;
    }

    uint8_t port = atoi(argv[1]);
    uint32_t freq = atoi(argv[2]);
    uint32_t duty_cycle = atoi(argv[3]);

    if (argc == 5) {
        uint8_t group = (uint8_t)atoi(argv[4]);
        port |= (group << 6);
    }

    printf("pwm output hwport=0x%x freq=%d dutycycle=%d\r\n", port, freq, duty_cycle);
    pwm_output(port, freq, (float)duty_cycle / 100.0f);
}

void cli_reg_cmd_pwm(void)
{
    static const struct cli_command cmd_info = { "pwm", "pwm test", cmd_pwm_func };

    aos_cli_register_command(&cmd_info);
}
