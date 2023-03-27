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

const char* help_str = "aoshal demo test\n"
    "appdemohal gpio_out 0\n"
    "appdemohal gpio_in 0 1\n"
    "appdemohal adc_signal 5\n"
    "appdemohal adc_multiple 5\n"
    "appdemohal adc_multiple_task_multiple_data 5 5\n"
    "appdemohal adc_multiple_task_signal_data 5 5\n"
    "appdemohal pwm 0 0 1000 0.1 10000 0.5\n"
    "appdemohal wdt\n"
    "appdemohal uart 27 28\n"
    "appdemohal <iic|iic_task> <master|slave|mem> 0 1\n"
    "appdemohal spi 0 1 4 5 2 3 6 7\n"
    "appdemohal spi <master|slave> <send|recv|send_recv> 25 26 27 28\n"
    "appdemohal flash\n"
    "appdemohal rtc\n"
    "for more information see the README\n\n>";

uint32_t gpio_port = 0xffff, gpio_trigger_method = 0xffff;
#if !defined(CONFIG_CHIP_D1)
uint32_t adc_pin = 0xffff, adc_pin0 = 0xffff, adc_pin1 = 0xffff;
#endif
uint32_t pwm_pin = 0xffff, pwm_id = 0, freq = 0, freq_chg = 0;
float duty_cycle = 0.0, duty_cycle_chg = 0.0;
uint32_t uart_pin[2] = {0};
uint32_t uart_task1_pin[2] = {0}, uart_task2_pin[2] = {0};
uint32_t iic_pin0 = 0xffff, iic_pin1 = 0xffff;
uint32_t spi_pin[8] = {0};

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

    if (strcmp(argv[1], "gpio_out") == 0) {
        sscanf(argv[2], "%u", &gpio_port);

        hal_gpio_out_demo(gpio_port);
    } else if (strcmp(argv[1], "gpio_in") == 0) {
        sscanf(argv[2], "%u", &gpio_port);
        sscanf(argv[3], "%u", &gpio_trigger_method);

        hal_gpio_int_demo(gpio_port, gpio_trigger_method);

#if !defined(CONFIG_CHIP_D1)
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
        sscanf(argv[6], "%u", &freq_chg);
        sscanf(argv[7], "%f", &duty_cycle_chg);

        ret = pin_name_err(pwm_pinmap, pwm_pin);

        if (ret == -1) {
            goto help;
        }

        ret = idx_err(pwm_pinmap, pwm_id);

        if (ret == 0) {
            hal_pwm_out_demo(pwm_pin, pwm_id, freq, duty_cycle, freq_chg, duty_cycle_chg);
        } else {
             goto help;
        }
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
    } else if (strcmp(argv[1], "spi")== 0) {
        APP_TEST_SPI_FUNCTION function;
        if (strcmp(argv[2], "master")== 0 || strcmp(argv[2], "slave")== 0) { //chip 只有一个 spi 时
            if (strcmp(argv[2], "master") == 0 && strcmp(argv[3], "send") == 0) {
                function = APP_TEST_SPI_MASTER_SEND;
            } else if (strcmp(argv[2], "master") == 0 && strcmp(argv[3], "recv") == 0) {
                function = APP_TEST_SPI_MASTER_RECV;
            } else if (strcmp(argv[2], "master") == 0 && strcmp(argv[3], "send_recv") == 0) {
                function = APP_TEST_SPI_MASTER_SEND_RECV;
            } else if (strcmp(argv[2], "slave") == 0 && strcmp(argv[3], "send") == 0) {
                function = APP_TEST_SPI_SLAVE_SEND;
            } else if (strcmp(argv[2], "slave") == 0 && strcmp(argv[3], "recv") == 0) {
                function = APP_TEST_SPI_SLAVE_RECV;
            } else if (strcmp(argv[2], "slave") == 0 && strcmp(argv[3], "send_recv") == 0) {
                function = APP_TEST_SPI_SLAVE_SEND_RECV;
            } else {
                goto help;
            }
            if (argc >= 8) { // appdemohal spi master send 25 26 27 28
                for (int i = 0; i < 4; i++)
                    sscanf(argv[i + 4], "%u", &spi_pin[i]);
            } else {
                goto help;
            }
        } else { // chip 有多个 spi 时
            function = APP_TEST_SPI_SLAVE_RECV_MASTER_SEND;
            if (argc >= 10) { // appdemohal spi 0 1 4 5 2 3 6 7
                for (int i = 0; i < 8; i++)
                    sscanf(argv[i + 2], "%u", &spi_pin[i]);
            } else {
                goto help;
            }
        }
        hal_spi_demo(function, spi_pin);
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
            } else if (strcmp(argv[1], "iic_task")== 0) {
                hal_iic_task_demo(function, iic_pin0, iic_pin1);
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