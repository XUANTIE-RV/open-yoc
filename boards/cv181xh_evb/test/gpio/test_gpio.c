#include <drv/gpio.h>
#include <stdio.h>
#include <drv/irq.h>
#include <drv/pin.h>
#include <unistd.h>
#include "aos/cli.h"

#define GPIO_IRQ_TEST 0
#define GPIO_PIN_MASK(_gpio_num) (1 << _gpio_num)

#if GPIO_IRQ_TEST
static void gpio_irq_test(void)
{
	aos_cli_printf("goto %s() \r\n", __func__);
}
#endif

void test_gpio(int32_t argc, char **argv)
{
	/**
	 * for example:
	 * testgpio o 0 18 1
	 * testgpio o 0(gpio bank) 18(gpio pin) 1(1->output hight, 0->output low)
     * testgpio i 0(gpio bank) 18(gpio pin)
	 */

    csi_error_t ret;
    csi_gpio_t gpio = {0};
    csi_gpio_dir_t dir = GPIO_DIRECTION_OUTPUT;
    unsigned int gpio_bank,gpio_pin,gpio_value;

	if (argc < 4) {
        aos_cli_printf("\tplease run(example): testgpio [dir] [gpio_bank] [gpio_pin] [value] \r\n");
		aos_cli_printf("\tplease run(output):  testgpio o 0 18 1 \r\n");
        aos_cli_printf("\tplease run(input):   testgpio i 0 18 \r\n");
		return;
	}

    if (strcmp(argv[1], "i") == 0) {
		dir = GPIO_DIRECTION_INPUT;
    } else if (strcmp(argv[1], "o") == 0) {
        dir = GPIO_DIRECTION_OUTPUT;
    } else {
        aos_cli_printf("invaild param: %s\r\n",argv[1]);
        return ;
    }

    gpio_bank = atoi(argv[2]);
    gpio_pin = atoi(argv[3]);

    gpio_value = argv[4] == NULL ? 0 : atoi(argv[4]);

    ret = csi_gpio_init(&gpio, gpio_bank);
    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_init failed\r\n");
		return;
    }

#if (!GPIO_IRQ_TEST)   // gpio write
    ret = csi_gpio_dir(&gpio, GPIO_PIN_MASK(gpio_pin), dir);

    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_dir failed\r\n");
		return;
    }
    if (dir == GPIO_DIRECTION_OUTPUT)
    {
        csi_gpio_write(&gpio, GPIO_PIN_MASK(gpio_pin), gpio_value);
    }
    else if (dir == GPIO_DIRECTION_INPUT)
    {
        if (csi_gpio_read(&gpio, GPIO_PIN_MASK(gpio_pin) ) != 0U) {
            gpio_value = 1;
        } else {
            gpio_value = 0;
        }
        aos_cli_printf("read gpio_value : %d \r\n",gpio_value);
    }
#else   // gpio irq
    csi_gpio_mode_t gpio_mode;

    dir = GPIO_DIRECTION_INPUT;    
    if (gpio_value == 1) {
        gpio_mode = GPIO_MODE_PULLUP;
    } else {
        gpio_mode = GPIO_MODE_PULLDOWN;
    }
    ret = csi_gpio_mode(&gpio, 1 << gpio_pin, gpio_mode);
    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_mode failed\r\n");
		return;
    }

    ret = csi_gpio_dir(&gpio, 1 << gpio_pin, dir);
    if (ret != CSI_OK)
    {
	    aos_cli_printf("csi_gpio_dir failed\r\n");
		return;
    }

    csi_gpio_irq_mode(&gpio, 1 << gpio_pin, GPIO_IRQ_MODE_FALLING_EDGE);

    csi_gpio_attach_callback(&gpio, gpio_irq_test, NULL);

    csi_gpio_irq_enable(&gpio, 1 << gpio_pin, true);

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
