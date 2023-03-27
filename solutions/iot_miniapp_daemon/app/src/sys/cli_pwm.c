/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/cli.h>
#include <aos/kernel.h>
#include <aos/hal/pwm.h>

/* output one pwm signal */
static void pwm_output(uint8_t port, uint32_t freq, float duty_cycle)
{
    int32_t ret;
    pwm_dev_t pwm = {0};

    printf(" hal_pwm_app_static_out start\r\n");

    pwm.port = port;
    pwm.config.freq = freq;
    pwm.config.duty_cycle = duty_cycle;
    pwm.priv = NULL;

    ret = hal_pwm_init(&pwm);
    if(ret){
        printf("hal_pwm_init fail,ret:%d\r\n",ret);
        return;
    }

    hal_pwm_start(&pwm);

    aos_msleep(3000);

    hal_pwm_stop(&pwm);

    hal_pwm_finalize(&pwm);

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
