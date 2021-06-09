#include "genie_service.h"
#include "switches_output.h"
#include "aos/hal/gpio.h"
#include "aos/kernel.h"

typedef struct _switch_io_s
{
    uint8_t onoff_status;
    gpio_dev_t io_a;
    gpio_dev_t io_b;
    aos_timer_t pulse_timer;
    long long  pulse_start;
} _switch_io_t;

typedef struct _switch_output_s
{
    _switch_io_t io_config[OUTPUT_IO_NUM];
} _switch_output_t;

_switch_output_t g_output_config;

static void pluse_timer_check_timeout(void *timer, void *arg)
{
    _switch_io_t *io_dev = (_switch_io_t *)arg;
    aos_timer_stop(&io_dev->pulse_timer);
    if(aos_now_ms() - io_dev->pulse_start < OUTPUT_DEFAULT_PLUSE_WIDTH){
        aos_timer_start(&io_dev->pulse_timer);
        return;
    }
    if (io_dev->onoff_status)
    {
        hal_gpio_output_low(&io_dev->io_a);
    }
    else
    {
        hal_gpio_output_low(&io_dev->io_b);
    }
}

int switch_output_gpio_init(_output_io_config_t *io_config, uint8_t size)
{
    if (size > OUTPUT_IO_NUM)
    {
        GENIE_LOG_ERR("suport no more than %d pairs IO", OUTPUT_IO_NUM);
        return -1;
    }
    if (!io_config || !size)
    {
        return -1;
    }
    for (int index = 0; index < size; index++)
    {
        g_output_config.io_config[index].io_a.port = io_config[index].port_a;
        g_output_config.io_config[index].io_a.config = OUTPUT_PUSH_PULL;
        g_output_config.io_config[index].io_b.port = io_config[index].port_b;
        g_output_config.io_config[index].io_b.config = OUTPUT_PUSH_PULL;
        hal_gpio_init(&g_output_config.io_config[index].io_a);
        hal_gpio_init(&g_output_config.io_config[index].io_b);
        hal_gpio_output_low(&g_output_config.io_config[index].io_a);
        hal_gpio_output_low(&g_output_config.io_config[index].io_b);
        aos_timer_new(&g_output_config.io_config[index].pulse_timer, pluse_timer_check_timeout, &g_output_config.io_config[index], OUTPUT_DEFAULT_PLUSE_WIDTH, 0);
    }
    return 0;
}

int switch_output_gpio_set(uint8_t index, bool onoff)
{
    if (index > OUTPUT_IO_NUM)
    {
        return -1;
    }
    if (onoff)
    {
        hal_gpio_output_high(&g_output_config.io_config[index].io_a);
    }
    else
    {
        hal_gpio_output_high(&g_output_config.io_config[index].io_b);
    }
    g_output_config.io_config[index].onoff_status = onoff;
    g_output_config.io_config[index].pulse_start  = aos_now_ms();
    aos_timer_start(&g_output_config.io_config[index].pulse_timer);
    return 0;
}
