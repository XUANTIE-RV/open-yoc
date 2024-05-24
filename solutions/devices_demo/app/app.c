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

#if defined(CVI_SOC_CV181XC)
const csi_pinmap_t pwm_pinmap[] = {};
#else
extern const csi_pinmap_t pwm_pinmap[];
#endif

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
uint8_t is_iic_master = false;
uint8_t is_iic_task = false;
uint8_t is_uart_task = false;

uint32_t uart_pin[2] = {0};
uint32_t uart_task1_pin[2] = {0}, uart_task2_pin[2] = {0};
uint32_t iic_pin0 = 0xffff, iic_pin1 = 0xffff;
uint32_t iic_task1_pin[2] = {0}, iic_task2_pin[2] = {0};
uint32_t pwm_pin = 0xffff, pwm_id = 0, freq = 0, freq_chg = 0;
float duty_cycle = 0.0, duty_cycle_chg = 0.0;
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

        hal_gpio_in_demo(gpio_port, gpio_trigger_method);
    } else if (strcmp(argv[1], "devfs_gpio_out") == 0) {
        sscanf(argv[2], "%u", &gpio_port);

        devfs_gpio_out_demo(gpio_port);
    } else if (strcmp(argv[1], "devfs_gpio_in") == 0) {
        sscanf(argv[2], "%u", &gpio_port);
        sscanf(argv[3], "%u", &gpio_trigger_method);

        devfs_gpio_in_demo(gpio_port, gpio_trigger_method);
    }
#if !defined(CONFIG_CHIP_D1)
    else if (strcmp(argv[1], "adc")== 0) {
        sscanf(argv[2], "%u", &adc_pin);

        ret = pin_name_err(adc_pinmap, adc_pin);

        if (ret == 0) {
            hal_adc_demo(adc_pin);
            devfs_adc_demo(adc_pin);
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
            devfs_task_adc_demo(adc_pin0, adc_pin1);
        } else {
            goto help;
        }
    }
#endif
    else if (strcmp(argv[1], "wdt")== 0) {
        hal_watchdog_demo();
        devfs_wdt_demo();
    }
#if !defined(CONFIG_BOARD_HUASHANPI)
    else if (strcmp(argv[1], "uart")== 0) {
        sscanf(argv[2], "%u", &uart_pin[0]);
        sscanf(argv[3], "%u", &uart_pin[1]);
        is_uart_task = false;
        hal_uart_demo(uart_pin);
        devfs_uart_demo(uart_pin);
    } else if (strcmp(argv[1], "uart_multiple_task")== 0) {
        sscanf(argv[2], "%u", &uart_task1_pin[0]);
        sscanf(argv[3], "%u", &uart_task1_pin[1]);

        sscanf(argv[4], "%u", &uart_task2_pin[0]);
        sscanf(argv[5], "%u", &uart_task2_pin[1]);
        is_uart_task = true;
        hal_task_uart_demo(uart_task1_pin, uart_task2_pin);
        devfs_task_uart_demo(uart_task1_pin, uart_task2_pin);
    } else if (strcmp(argv[1], "uart_poll")== 0) {
        sscanf(argv[2], "%u", &uart_pin[0]);
        sscanf(argv[3], "%u", &uart_pin[1]);
        is_uart_task = false;
        hal_uart_poll_demo(uart_pin);
        devfs_uart_poll_demo(uart_pin);
    }
#endif 
    else if (strcmp(argv[1], "iic")== 0) {
        APP_TEST_IIC_FUNCTION function;
        is_iic_task = false;
        if (strcmp(argv[2], "master_send") == 0) {
            function = APP_TEST_IIC_MASTER_SEND;
        } else if (strcmp(argv[2], "slave_recv") == 0 ) {
            function = APP_TEST_IIC_SLAVE_RECV;
        } else if (strcmp(argv[2], "master_recv") == 0 ) {
            function = APP_TEST_IIC_MASTER_RECV;
        } else if (strcmp(argv[2], "slave_send") == 0 ) {
            function = APP_TEST_IIC_SLAVE_SEND;
        } else {
            goto help;
        }
        if (argc >= 5) {
            if (strcmp(argv[1], "iic")== 0) {
                is_iic_task = false;
                sscanf(argv[3], "%u", &iic_pin0);
                sscanf(argv[4], "%u", &iic_pin1);
                
                hal_iic_demo(function, iic_pin0, iic_pin1);
                devfs_iic_demo(function, iic_pin0, iic_pin1);
            }
        }
    } else if (strcmp(argv[1], "iic_task")== 0) {
                is_iic_task = true;
                if(strcmp(argv[2], "master")== 0) {
                    is_iic_master = true;
                    sscanf(argv[3], "%u", &iic_pin0);
                    sscanf(argv[4], "%u", &iic_pin1);
                    hal_task_iic_demo(is_iic_master, iic_pin0, iic_pin1);
                    devfs_task_iic_demo(is_iic_master, iic_pin0, iic_pin1);
                } else if(strcmp(argv[2], "slave")== 0) {
                    is_iic_master = false;
                    sscanf(argv[3], "%u", &iic_pin0);
                    sscanf(argv[4], "%u", &iic_pin1);
                    hal_task_iic_demo(is_iic_master, iic_pin0, iic_pin1);
                    devfs_task_iic_demo(is_iic_master, iic_pin0, iic_pin1);
                }else {
                     goto help;
                } 
    } 
    else if (strcmp(argv[1], "flash")== 0) {
        hal_flash_demo();
    }
#if !defined(CONFIG_CHIP_D1)
    else if (strcmp(argv[1], "rtc")== 0) {
        hal_rtc_demo();
        devfs_rtc_demo();
        devfs_rtc_cancel_alarm_demo();
    }
#endif
    else if (strcmp(argv[1], "clk")== 0) {
        hal_clk_demo();
        devfs_clk_demo();
    } else if (strcmp(argv[1], "mmc")== 0) {
        hal_mmc_demo();
    }
#if defined(CONFIG_CHIP_D1) && CONFIG_CHIP_D1
#if defined(CONFIG_BOARD_DISPLAY) && CONFIG_BOARD_DISPLAY
    else if (strcmp(argv[1], "display_input")== 0) {
        devfs_display_input_demo();
    }
#endif
    else if (strcmp(argv[1], "hci")== 0) {
        devfs_hci_demo();
    }
#endif /*defined(CONFIG_CHIP_D1) && CONFIG_CHIP_D1*/
    else if (strcmp(argv[1], "wifi")== 0) {
        devfs_wifi_demo();
    } else if (strcmp(argv[1], "timer")== 0) {
        hal_timer_demo();
        devfs_timer_demo();
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
            devfs_pwm_out_demo(pwm_pin, pwm_id, freq, duty_cycle, freq_chg, duty_cycle_chg);
        } else {
             goto help;
        }
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
        // 涉及到主从的，rvm_hal 与 devfs都分开跑
        hal_spi_demo(function, spi_pin);
        devfs_spi_demo(function, spi_pin);
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


#if !defined(AOS_COMP_DEVFS) || !AOS_COMP_DEVFS
int devfs_gpio_out_demo(int port) {return 0;}
int devfs_gpio_in_demo(int port, int trigger_method) {return 0;}
int devfs_adc_demo(uint8_t gpio_pin) {return 0;}
int devfs_task_adc_demo(uint8_t gpio_pin0, uint8_t gpio_pin1) {return 0;}
int devfs_wdt_demo(void) {return 0;}
int devfs_uart_demo(uint32_t *gpio_pins) {return 0;}
int devfs_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins) {return 0;}
int devfs_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1) {return 0;}
int devfs_rtc_demo(void) {return 0;}
int devfs_rtc_cancel_alarm_demo(void) {return 0;}
int devfs_clk_demo(void) {return 0;}
int devfs_display_input_demo(void) {return 0;}
int devfs_hci_demo(void) {return 0;}
int devfs_wifi_demo(void) {return 0;}
int devfs_timer_demo(void) {return 0;}
int devfs_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle, uint32_t freq_chg, float duty_cycle_chg) {return 0;}
int devfs_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin) {return 0;}
int devfs_uart_poll_demo(uint32_t *gpio_pins) {return 0;};
int devfs_task_iic_demo(uint8_t is_master, uint8_t gpio_pin0, uint8_t gpio_pin1) {return 0;};
#endif