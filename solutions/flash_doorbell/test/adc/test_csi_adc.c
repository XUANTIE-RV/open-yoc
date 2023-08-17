#include <drv/adc.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/cli.h>

void test_adc(int32_t argc, char **argv)
{
    /**
     * for example:
     * testadc 0
     * testadc 0(adc bank)
     */

    csi_adc_t adc;
    #if (!ADC_IRQ_TEST)
    int value;
    #endif
    #if ADC_IRQ_TEST
    static uint32_t result;
    #endif
    if (argc < 2) {
        aos_cli_printf("please run(example): testadc 0\r\n");
        return;
    }
    csi_adc_init(&adc, atoi(argv[1]));
    #if (!ADC_IRQ_TEST)
    // csi_adc_freq_div(&adc, 3);
    // freq = csi_adc_get_freq(&adc);
    csi_adc_channel_enable(&adc, 1, true);
    csi_adc_start(&adc);
    value = csi_adc_read(&adc);
    aos_cli_printf("adc get value: 0x%d\n", value);
    #else
    csi_adc_set_buffer(&adc, &result, 1);
    csi_adc_channel_enable(&adc, 1, true);
    csi_adc_attach_callback(&adc, adc_irq_test, NULL);
    csi_adc_start(&adc);
    aos_cli_printf("adc irq test start\n");
    while(1)
    {
        mdelay(1000);
    }
    #endif
    // csi_adc_uninit(&adc);
    free(adc.priv);
    aos_cli_printf("test adc success.\r\n");
}
ALIOS_CLI_CMD_REGISTER(test_adc, testadc, test adc function);
