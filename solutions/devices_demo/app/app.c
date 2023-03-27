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

#if !defined(CONFIG_CHIP_D1)
extern const csi_pinmap_t adc_pinmap[];
#endif

extern const csi_pinmap_t pwm_pinmap[];

const char* help_str = "hal demo test\n"
    "appdemohal gpio_out 0\n"
    "appdemohal gpio_in 0 1\n"
    "appdemohal adc 5\n"
    "appdemohal adc_multiple_task 5 5\n"
    "appdemohal <iic|iic_task> <master|slave|mem> 8 9\n"
    "appdemohal wdt\n"
    "appdemohal uart 11 12\n"
    "appdemohal flash\n"
    "appdemohal rtc\n"
    "appdemohal clk\n"
    "appdemohal mmc\n"
    "for more information see the README\n\n>";

uint32_t gpio_port = 0xffff, gpio_trigger_method = 0xffff;
#if !defined(CONFIG_CHIP_D1)
uint32_t adc_pin = 0xffff, adc_pin0 = 0xffff, adc_pin1 = 0xffff;
#endif
uint32_t uart_pin[2] = {0};
uint32_t uart_task1_pin[2] = {0}, uart_task2_pin[2] = {0};
uint32_t iic_pin0 = 0xffff, iic_pin1 = 0xffff;

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

// static int idx_err(const csi_pinmap_t *map, pin_name_t idx)
// {
//     while ((uint32_t)map->pin_name != 0xFFU) {
//         if (map->idx == idx) {
//             break;
//         }

//         map++;
//     }

//     if (map->pin_name == 0xFFU) {
//         return -1;
//     }

//     return 0;
// }

static void appdemohal_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        goto help;
    }

    int ret = 0;

    if (strcmp(argv[1], "gpio_out") == 0) {
        sscanf(argv[2], "%u", &gpio_port);

        hal_gpio_out_demo(gpio_port);
    } else if (strcmp(argv[1], "gpio_in") == 0) {
        sscanf(argv[2], "%u", &gpio_port);
        sscanf(argv[3], "%u", &gpio_trigger_method);

        hal_gpio_int_demo(gpio_port, gpio_trigger_method);

#if !defined(CONFIG_CHIP_D1)
    } else if (strcmp(argv[1], "adc")== 0) {
        sscanf(argv[2], "%u", &adc_pin);

        ret = pin_name_err(adc_pinmap, adc_pin);

        if (ret == 0) {
            hal_adc_demo(adc_pin);
        } else {
            goto help;
        }
    } else if (strcmp(argv[1], "adc_multiple_task")== 0) {
        sscanf(argv[2], "%u", &adc_pin0);
        sscanf(argv[3], "%u", &adc_pin1);

        ret = pin_name_err(adc_pinmap, adc_pin0);

        if (ret == -1) {
            goto help;
        }

        ret = pin_name_err(adc_pinmap, adc_pin1);

        if (ret == 0) {
            hal_task_adc_demo(adc_pin0, adc_pin1);
        } else {
            goto help;
        }
#endif

    } else if (strcmp(argv[1], "wdt")== 0) {
        hal_watchdog_demo();
    } else if (strcmp(argv[1], "uart")== 0) {
        sscanf(argv[2], "%u", &uart_pin[0]);
        sscanf(argv[3], "%u", &uart_pin[1]);

        hal_uart_demo(uart_pin);
    } else if (strcmp(argv[1], "uart_multiple_task")== 0) {
        sscanf(argv[2], "%u", &uart_task1_pin[0]);
        sscanf(argv[3], "%u", &uart_task1_pin[1]);

        sscanf(argv[4], "%u", &uart_task2_pin[0]);
        sscanf(argv[5], "%u", &uart_task2_pin[1]);

        hal_task_uart_demo(uart_task1_pin, uart_task2_pin);
    } else if (strcmp(argv[1], "iic")== 0 || strcmp(argv[1], "iic_task")== 0) {
        APP_TEST_IIC_FUNCTION function;
        if (strcmp(argv[2], "master") == 0) {
            function = APP_TEST_IIC_MASTER_SEND_RECV;
        } else if (strcmp(argv[2], "slave") == 0 ) {
            function = APP_TEST_IIC_SLAVE_RECV_SEND;
        } else if (strcmp(argv[2], "mem") == 0 ) {
            function = APP_TEST_IIC_MEM_WRITE_READ;
        } else {
            goto help;
        }
        if (argc >= 5) {
            sscanf(argv[3], "%u", &iic_pin0);
            sscanf(argv[4], "%u", &iic_pin1);
            if (strcmp(argv[1], "iic")== 0) {
                hal_iic_demo(function, iic_pin0, iic_pin1);
            } else {
                goto help;
            }
        } else {
            goto help;
        }
    } else if (strcmp(argv[1], "flash")== 0) {
        hal_flash_demo();
    } else if (strcmp(argv[1], "rtc")== 0) {
        hal_rtc_demo();
    } else if (strcmp(argv[1], "clk")== 0) {
        hal_clk_demo();
    } else if (strcmp(argv[1], "mmc")== 0) {
        hal_mmc_demo();
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