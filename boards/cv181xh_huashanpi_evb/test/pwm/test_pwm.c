#include <drv/pwm.h>
#include <stdio.h>
#include <aos/cli.h>
#include <drv/pin.h>
#include <posix/timer.h>
#include <unistd.h>

void test_pwm(int32_t argc, char **argv)
{
	/* test sdcard 命令实现
	 * for example:
	 * testpwm bank(0~3) channel(0~3) o period_us duty_us polarity
     * testpwm bank(0~3) channel(0~3) i
	 */

    // csi_error_t ret;
    csi_pwm_t pwm;
    int value, timeout = 5;
    uint32_t bank, channel, period_us, duty_us, polarity;

	if (argc < 4) {
		aos_cli_printf("please run(example): \n"
                        "\ttestpwm bank(0~3) channel(0~3) o period_us duty_us polarity\n"
                        "\ttestpwm bank(0~3) channel(0~3) i\n");
		return;
	}

    bank = atoi(argv[1]);
    channel = atoi(argv[2]);

    for (value=0; value<argc; value++)
		aos_cli_printf("argv[%d] = %s\r\n", value, argv[value]);

    switch (argv[3][0])
    {
        case 'o':
            // ret = csi_pin_set_mux(JTAG_CPU_TCK, JTAG_CPU_TCK__PWM_6);
            // if (ret != CSI_OK)
            // {
            //     aos_cli_printf("csi_pin_set_mux failed\r\n");
            //     return;
            // }

            period_us = atoi(argv[4]);
            duty_us = atoi(argv[5]);
            polarity = atoi(argv[6]);

            csi_pwm_init(&pwm, bank);
            csi_pwm_out_config(&pwm, channel, period_us, duty_us,
                                polarity ? PWM_POLARITY_HIGH : PWM_POLARITY_LOW);
            csi_pwm_out_start(&pwm, channel);

            // aos_cli_printf("pwm test success\n");
            break;

        case 'i':
            // ret = csi_pin_set_mux(JTAG_CPU_TMS, JTAG_CPU_TMS__PWM_7);
            // if (ret != CSI_OK)
            // {
            //     aos_cli_printf("csi_pin_set_mux failed\r\n");
            //     return;
            // }

            csi_pwm_init(&pwm, bank);
            csi_pwm_capture_config(&pwm, channel, 0, 0);
            csi_pwm_capture_start(&pwm, channel);

            #define PWM_CLK_HZ  (100*1000*1000UL)  /* pwm clock default 100 MHz */

            while(timeout--)
            {
                sleep(1);
                value = *((uint32_t *)(pwm.dev.reg_base + 0x24 + 3 * 8));
                printf("test read reg %p value: %ld\n", ((uint32_t *)(pwm.dev.reg_base + 0x24 + 3 * 8)), PWM_CLK_HZ/value);
            }

            // aos_cli_printf("pwm test success\n");
            break;

        default:
            break;
    }

}
ALIOS_CLI_CMD_REGISTER(test_pwm, testpwm, test pwm function);
