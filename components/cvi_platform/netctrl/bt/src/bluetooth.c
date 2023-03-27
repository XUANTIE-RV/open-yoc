#include <aos/ble.h>
#include <aos/aos.h>
#include <board.h>
#include <drv/pin.h>
#include "pinctrl-mars.h"
#if (CONFIG_APP_RTL8723DS_BT_SUPPORT == 1)
#include <devices/rtl8723ds_bt.h>
#endif
#ifndef CVI_BT_UART_ID
#define CVI_BT_UART_ID 3
#endif

#ifndef CVI_BT_DIS_PIN_ID
#define CVI_BT_DIS_PIN_ID -1
#endif


#define BLUETOOLTH_TEST 1
#define DEVICE_NAME "YoC ADV"
#define DEVICE_ADDR {0xCC,0x3B,0xE3,0x82,0xBA,0xC0}
#define TAG "DEMO"

void _bt_drive_register(void)
{
    #if (CONFIG_APP_RTL8723DS_BT_SUPPORT == 1)
    rtl8723ds_bt_config config = {
        .uart_id    = CVI_BT_UART_ID,
        .bt_dis_pin = CVI_BT_DIS_PIN_ID,
    };
    bt_rtl8723ds_register(&config);
    extern int hci_h5_driver_init();
    hci_h5_driver_init();
    #endif
}

#if (BLUETOOLTH_TEST == 1)
static int start_adv(void)
{
    int ret;
    ad_data_t ad[2] = {0};
    ad_data_t sd[1] = {0};

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;

    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len = 1;

    uint8_t uuid16_list[] = {0x0d, 0x18};
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)uuid16_list;
    ad[1].len = sizeof(uuid16_list);

    uint8_t manu_data[10] = {0x01, 0x02, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};
    sd[0].type = AD_DATA_TYPE_MANUFACTURER_DATA;
    sd[0].data = (uint8_t *)manu_data;
    sd[0].len = sizeof(manu_data);

    adv_param_t param = {
        ADV_IND,
        ad,
        sd,
        BLE_ARRAY_NUM(ad),
        BLE_ARRAY_NUM(sd),
        ADV_FAST_INT_MIN_2,
        ADV_FAST_INT_MAX_2,
    };

    ret = ble_stack_adv_start(&param);

    if (ret) {
        printf("adv start fail %d!\r\n", ret);
    } else {
        printf("adv start!\r\n");
    }

    printf("adv_type:%x;adv_interval_min:%.3f ms;adv_interval_max:%.3f ms\r\n", param.type, param.interval_min * 0.625, param.interval_max * 0.625);
    return ret;
}

int adv_test(void)
{
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = DEVICE_NAME,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };

    ble_stack_init(&init);
    start_adv();
    return 0;
}
#endif

void APP_BtInit(void)
{
    _bt_drive_register();
#if (BLUETOOLTH_TEST == 1)
    adv_test();
#else
    /* ble stack initialization */
    init_param_t init = {
        .dev_name = DEVICE_NAME,
        .dev_addr = NULL,
    };
    ble_stack_init(&init);
#endif
    ble_stack_setting_load();
}
