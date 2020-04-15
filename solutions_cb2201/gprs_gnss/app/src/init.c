#include <yoc_config.h>

#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <yoc/sysinfo.h>
#include <devices/devicelist.h>
#include <pin.h>
#include <devices/m6313.h>

#include "app_main.h"

#define KV_STR_LEN  32

#define TAG "init"

netmgr_hdl_t app_netmgr_hdl;

static void network_init()
{
    aos_kv_setint("wifi_en", 0);
    aos_kv_setint("gprs_en", 1);
    aos_kv_setint("eth_en", 0);
    m6313_param_t sim_param;
    sim_param.baud           = 115200;
    sim_param.buf_size       = 4096;
    sim_param.enable_flowctl = 0;
    sim_param.device_name    = "uart2";
    sim_param.reset_pin      = GPRS_SIM800_PIN_STATUS;
    sim_param.state_pin      = GPRS_SIM800_PIN_PWR;

    gprs_m6313_register(NULL, &sim_param);
    gnss_m6313_register(NULL, &sim_param);
    app_netmgr_hdl = netmgr_dev_gprs_init();

    utask_t *task = utask_new("netmgr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    if (app_netmgr_hdl) {
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
    }
}

void board_yoc_init(void)
{
    event_service_init(NULL);
    uart_csky_register(CONSOLE_IDX);
    uart_csky_register(2);
    uart_csky_register(1);
    flash_csky_register(0);
    console_init(0, 115200, 128);

    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");

    /* uService init */
    utask_t *task = utask_new("at&cli", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

#if defined(CONFIG_AT) && CONFIG_AT
    at_server_init(task);
#endif
static battery_pin_config_t battery_config = {12, //PA14_PWM10_ADC6, adc channel
                                              -1, //gpio pin for battery removed event
                                              NULL //handle battery event
                                              };
extern void battery_fetch_register(battery_pin_config_t *config, int idx);
    battery_fetch_register(&battery_config, 1);
    board_cli_init(task);

    network_init();
}

extern int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode);
static void board_pinmux_config(void)
{
    //console
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);

    //uart1
    drv_pinmux_config(AT_PIN_USART_TX, AT_TXD_FUNC);
    drv_pinmux_config(AT_PIN_USART_RX, AT_RXD_FUNC);
    drv_pin_config_mode(PORTA, 15, 1);
    drv_pin_config_mode(PORTA, 16, 1);

    //uart2
    drv_pinmux_config(NETM_PIN_USART_TX, NETM_PIN_USART_TX_FUNC);
    drv_pinmux_config(NETM_PIN_USART_RX, NETM_PIN_USART_RX_FUNC);

    //enc28j60
    drv_pinmux_config(ENC28J60_ETH_SPI_MISO, ENC28J60_ETH_SPI_MISO_FUNC);
    drv_pinmux_config(ENC28J60_ETH_SPI_MOSI, ENC28J60_ETH_SPI_MOSI_FUNC);
    drv_pinmux_config(ENC28J60_ETH_SPI_CS, ENC28J60_ETH_SPI_CS_FUNC);
    drv_pinmux_config(ENC28J60_ETH_SPI_SCK, ENC28J60_ETH_SPI_SCK_FUNC);
    drv_pinmux_config(ENC28J60_ETH_PIN_RST, ENC28J60_ETH_PIN_RST_FUNC);
    drv_pinmux_config(ENC28J60_ETH_PIN_INT, ENC28J60_ETH_PIN_INT_FUNC);

    //iic
    drv_pinmux_config(EXAMPLE_PIN_IIC_SDA, EXAMPLE_PIN_IIC_SDA_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_IIC_SCL, EXAMPLE_PIN_IIC_SCL_FUNC);

    //adc
    drv_pinmux_config(EXAMPLE_ADC_CH12, EXAMPLE_ADC_CH12_FUNC);

    //pwm
    drv_pinmux_config(EXAMPLE_PWM_CH, EXAMPLE_PWM_CH_FUNC);

    //dht11
    drv_pinmux_config(EXAMPLE_DHT11_PIN, EXAMPLE_DHT11_PIN_FUNC);

    //led
    drv_pinmux_config(EXAMPLE_LED1_PIN1, EXAMPLE_LED1_PIN1_FUNC);
    drv_pinmux_config(EXAMPLE_LED1_PIN2, EXAMPLE_LED1_PIN2_FUNC);
    drv_pinmux_config(EXAMPLE_LED2_PIN1, EXAMPLE_LED2_PIN1_FUNC);
    drv_pinmux_config(EXAMPLE_LED2_PIN2, EXAMPLE_LED2_PIN2_FUNC);

    //push button
    drv_pinmux_config(EXAMPLE_PUSH_BUTTON_PIN, EXAMPLE_PUSH_BUTTON_PIN_FUNC);
}

void board_init(void)
{
    ioreuse_initial();

    board_pinmux_config();
}
