/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef CONFIG_HAL_ADC_DISABLED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/hal/adc.h>
#include <drv/pin.h>

static void adc_app_out(uint8_t port)
{
    uint32_t  val[64];
    adc_dev_t adc_dev;
    int       ret;
    int       avr_count = 2;

    printf("adc_app_out on port %d start\r\n", port);

    adc_dev.port = port;

    ret = hal_adc_init(&adc_dev);

    if (ret != 0) {
        printf("hal_adc_init error\n");
        goto out;
    }

    memset(val, 0, sizeof(val));
    //ret = hal_adc_value_get(&adc_dev, val, 1000);
    ret = hal_adc_value_multiple_get(&adc_dev, val, avr_count, 1000);
    if (ret == 0) {
        uint32_t avr_val = 0;
        for (int i = 0; i < avr_count; i++) {
            //printf("%d\r\n", val[i]);
            avr_val += val[i];
        }
        avr_val /= avr_count;
        printf("adc port %d value %d\r\n", adc_dev.port, avr_val);
    } else {
        printf("%s: get value error, ret %d\r\n", __func__, ret);
        goto out;
    }

out:
    hal_adc_finalize(&adc_dev);
    printf("\r\n");
}

static void cmd_gadc_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        printf("usage:\r\n\tgadc port\n");
        return;
    }

    if (argc == 2) {
        int port = atoi(argv[1]);
        adc_app_out(port);
    }
}

void cli_reg_cmd_gadc(void)
{
    static const struct cli_command cmd_info = { "gadc", "gadc test", cmd_gadc_func };

    aos_cli_register_command(&cmd_info);
}
#endif
