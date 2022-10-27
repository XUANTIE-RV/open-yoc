#include <stdlib.h>
#include <string.h>

#include <drv/pin.h>
#include <uservice/eventid.h>
#include <aos/yloop.h>
#include <devices/wifi.h>
#include <devices/device.h>

#include <aos/yloop.h>
#include <aos/cli.h>
#include <aos/kv.h>

#include "app_main.h"

#if (!CONFIG_CHIP_D1)
extern const csi_pinmap_t adc_pinmap[];
#endif

extern const csi_pinmap_t pwm_pinmap[];

const char* help_str = "aoshal demo test\n"
    "appdemohal gpio_out 0\n"
    "appdemohal gpio_in 0 1\n"
    "appdemohal adc_signal 5\n"
    "appdemohal adc_multiple 5\n"
    "appdemohal adc_multiple_task_multiple_data 5 5\n"
    "appdemohal adc_multiple_task_signal_data 5 5\n"
    "appdemohal pwm 0 0 1000 0.1\n"
    "appdemohal wdt\n"
    "appdemohal uart 27 28\n"
    "appdemohal iic_slave 8 9   appdemohal iic_master 8 9\n"
    "appdemohal spi 0 1 4 5 2 3 6 7\n"
    "appdemohal flash\n"
    "for more information see the README\n\n>";

static int pin_name_err(const csi_pinmap_t *map, pin_name_t pin)
{
    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == pin) {
            break;
        }

        map++;
    }

    if (map->pin_name == 0xFFU) {
        return -1;
    }

    return 0;
}

static int idx_err(const csi_pinmap_t *map, pin_name_t idx)
{
    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->idx == idx) {
            break;
        }

        map++;
    }

    if (map->pin_name == 0xFFU) {
        return -1;
    }

    return 0;
}

static void appdemohal_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        goto help;
    }

    int ret = 0;
    uint32_t gpio_port = 0xffff, gpio_trigger_method = 0xffff;
#if (!CONFIG_CHIP_D1)
    uint32_t adc_pin = 0xffff, adc_pin0 = 0xffff, adc_pin1 = 0xffff;
#endif
    uint32_t pwm_pin = 0xffff, pwm_id = 0, freq = 0;
    float duty_cycle = 0.0;
    uint32_t uart_pin0 = 0xffff, uart_pin1 = 0xffff;
#if (!CONFIG_CHIP_BL606P_E907)
    uint32_t iic_pin0 = 0xffff, iic_pin1 = 0xffff;
    uint32_t spi_pin[8] = {0};
#endif

    if (strcmp(argv[1], "gpio_out") == 0) {
        sscanf(argv[2], "%u", &gpio_port);

        hal_gpio_out_demo(gpio_port);
    } else if (strcmp(argv[1], "gpio_in") == 0) {
        sscanf(argv[2], "%u", &gpio_port);
        sscanf(argv[3], "%u", &gpio_trigger_method);

        hal_gpio_int_demo(gpio_port, gpio_trigger_method);

#if (!CONFIG_CHIP_D1)
    } else if (strcmp(argv[1], "adc_signal")== 0) {
        sscanf(argv[2], "%u", &adc_pin);

        ret = pin_name_err(adc_pinmap, adc_pin);

        if (ret == 0) {
            hal_adc_out_signal_demo(adc_pin);
        } else {
            goto help;
        }
    } else if (strcmp(argv[1], "adc_multiple")== 0) {
        sscanf(argv[2], "%u", &adc_pin);

        ret = pin_name_err(adc_pinmap, adc_pin);

        if (ret == 0) {
            hal_adc_out_multiple_demo(adc_pin);
        } else {
            goto help;
        }
    } else if (strcmp(argv[1], "adc_multiple_task_multiple_data")== 0) {
        sscanf(argv[2], "%u", &adc_pin0);
        sscanf(argv[3], "%u", &adc_pin1);

        ret = pin_name_err(adc_pinmap, adc_pin0);

        if (ret == -1) {
            goto help;
        }

        ret = pin_name_err(adc_pinmap, adc_pin1);

        if (ret == 0) {
            hal_task_adc_multiple_demo(adc_pin0, adc_pin1);
        } else {
            goto help;
        }
    } else if (strcmp(argv[1], "adc_multiple_task_signal_data")== 0) {
        sscanf(argv[2], "%u", &adc_pin0);
        sscanf(argv[3], "%u", &adc_pin1);

        ret = pin_name_err(adc_pinmap, adc_pin0);

        if (ret == -1) {
            goto help;
        }

        ret = pin_name_err(adc_pinmap, adc_pin1);

        if (ret == 0) {
            hal_task_adc_signal_demo(adc_pin0, adc_pin1);
        } else {
            goto help;
        }
#endif

    } else if (strcmp(argv[1], "pwm")== 0) {
        sscanf(argv[2], "%u", &pwm_pin);
        sscanf(argv[3], "%u", &pwm_id);
        sscanf(argv[4], "%u", &freq);
        sscanf(argv[5], "%f", &duty_cycle);

        ret = pin_name_err(pwm_pinmap, pwm_pin);

        if (ret == -1) {
            goto help;
        }

        ret = idx_err(pwm_pinmap, pwm_id);

        if (ret == 0) {
            hal_pwm_out_demo(pwm_pin, pwm_id, freq, duty_cycle);
        } else {
             goto help;
        }
    } else if (strcmp(argv[1], "wdt")== 0) {
        hal_watchdog_demo();
    } else if (strcmp(argv[1], "uart")== 0) {
        sscanf(argv[2], "%u", &uart_pin0);
        sscanf(argv[3], "%u", &uart_pin1);

        hal_uart_demo(uart_pin0, uart_pin1);

#if (!CONFIG_CHIP_BL606P_E907)
    } else if (strcmp(argv[1], "spi")== 0) {

        for (int i = 0; i < 8; i++) {
            sscanf(argv[i + 2], "%u", &spi_pin[i]);
        }

        hal_spi_demo(spi_pin);
    } else if (strcmp(argv[1], "iic_master")== 0) {
        sscanf(argv[2], "%u", &iic_pin0);
        sscanf(argv[3], "%u", &iic_pin1);

        hal_iic_master_demo(iic_pin0, iic_pin1);
    } else if (strcmp(argv[1], "iic_slave")== 0) {
        sscanf(argv[2], "%u", &iic_pin0);
        sscanf(argv[3], "%u", &iic_pin1);

        hal_iic_slave_demo(iic_pin0, iic_pin1);
#endif

    } else if (strcmp(argv[1], "flash")== 0) {
        hal_flash_demo();
    } else {
        goto help;
    }

    return;

help:   
    printf("Argument failed\n%s", help_str);
}

void app_init(void)
{
    static const struct cli_command cmd_info = {
        "appdemohal",
        "appdemohal test",
        appdemohal_cmd
    };
    aos_cli_register_command(&cmd_info);
}