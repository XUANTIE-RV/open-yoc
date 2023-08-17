#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>
#include "aos/cli.h"

#define GPIO_IRQ_TEST 0
#define GPIO_PIN_MASK(_gpio_num) (1 << _gpio_num)

#if GPIO_IRQ_TEST
static void gpio_irq_test(void)
{
	aos_cli_printf("goto %s() \n", __func__);
}
#endif

void test_gpio(int32_t argc, char **argv)
{
	/**
	 * for example:
	 * testgpio 0 18 1
	 * testgpio 0(gpio bank) 18(gpio pin) 1(1->output hight, 0->output low)
	 */

    csi_error_t ret;
    csi_gpio_t gpio = {0};

	if (argc < 4) {
		aos_cli_printf("please run(example): testgpio 0 18 1 \r\n");
		return;
	}

    ret = csi_gpio_init(&gpio, atoi(argv[1]));
    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_init failed\r\n");
		return;
    }

#if (!GPIO_IRQ_TEST)   // gpio write
    ret = csi_gpio_dir(&gpio, GPIO_PIN_MASK(atoi(argv[2])), GPIO_DIRECTION_OUTPUT);

    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_dir failed\r\n");
		return;
    }

    csi_gpio_write(&gpio, GPIO_PIN_MASK(atoi(argv[2])), atoi(argv[3]));

#else   // gpio irq
    ret = csi_gpio_mode(&gpio, 1 << atoi(argv[2]), GPIO_MODE_PULLUP);
    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_mode failed\r\n");
		return;
    }

    ret = csi_gpio_dir(&gpio, 1 << atoi(argv[2]), GPIO_DIRECTION_INPUT);
    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_dir failed\r\n");
		return;
    }

    csi_gpio_irq_mode(&gpio, 1 << atoi(argv[2]), GPIO_IRQ_MODE_FALLING_EDGE);

    csi_gpio_attach_callback(&gpio, gpio_irq_test, NULL);

    csi_gpio_irq_enable(&gpio, 1 << atoi(argv[2]), true);

    while(1)
    {
        mdelay(1000);
    }
#endif

	// csi_gpio_uninit(&gpio);
	free(gpio.priv);

	aos_cli_printf("test gpio success.\r\n");
}
ALIOS_CLI_CMD_REGISTER(test_gpio, testgpio, test gpio function);
