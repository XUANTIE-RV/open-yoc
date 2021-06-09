## 概述

RTL8723块是一款低功耗WIFI+蓝牙二合一WIFI模块，SDIO接口，基本速率：1Mbps2,3Mbps for enhanced data rate量可达150Mbps，蓝牙支持BT2.1+EDR/BT3.0和BT4.2

## 配置

无

## 接口说明

蓝牙HCI驱动需要移植 `hci_impl.h` 中的结构体

```c
typedef struct hci_driver {
    driver_t drv;
    int (*set_event)(aos_dev_t *dev, hci_event_cb_t event, void *priv);
    int (*start)(aos_dev_t *dev, hci_driver_send_cmd_t send_cmd);
    int (*send)(aos_dev_t *dev, uint8_t *data, uint32_t size);
    int (*recv)(aos_dev_t *dev, uint8_t *data, uint32_t size);
} hci_driver_t;
```

| API       | 说明                        |
| --------- | --------------------------- |
| open      | hci设备打开接口             |
| close     | hci设备关闭接口             |
| set_event | hci设置事件回调接口         |
| set_event | hci数据发送接口             |
| recv      | hci数据接收接口             |
| start     | hci设备启动接口             |

## 示例

### 驱动注册

```c
#include <devices/rtl8723ds.h>
#include <pin.h>

void board_yoc_init()
{
    rtl8723ds_bt_config config = {
        .uart_id = BT_UART_IDX,
        .bt_dis_pin = BT_DIS_PIN,
    };

    bt_rtl8723ds_register(&config);

    extern int hci_h5_driver_init();
    hci_h5_driver_init();
}
```

在调用bt_rtl8723ds_register后，该设备会注册进入YoC设备列表，并且自动调用h5_hal_init

**注意点:**
1、.drv.name名字需要固定为"hci"， ble host会根据该名字打开hci驱动。
2、REALTEK RTL8723DS芯片的hci接口，使用的是3线uart方式，采用h5的协议，支持软件流控。
需要实现.drv.start接口。如果使用4线uart，带硬件流控方式，则不需要对接该接口。
关于h4，h5协议的介绍，参考蓝牙协议文档《Bluetooth Core Specification》4.2 以上版本，Vol 4. Host Controller Interface章节。
3、在调用bt_rtl8723ds_register接口，solution中需要根据使用的是H5协议，还是H4协议，调用hci_h5_driver_init或者hci_h4_driver_init接口。
开发者也可以将该接口封装到register接口中。

### 驱动验证

HCI组件对接完成后，可以运行一个简单广播示例来确认是否已经对接成功。
示例代码如下
```c
#include "aos/ble.h"
#include <aos/aos.h>

#define DEVICE_NAME "YoC ADV"
#define DEVICE_ADDR {0xCC,0x3B,0xE3,0x82,0xBA,0xC0}
#define TAG "DEMO"

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
        LOGE(TAG, "adv start fail %d!", ret);
    } else {
        LOGI(TAG, "adv start!");
    }

    LOGI(TAG, "adv_type:%x;adv_interval_min:%.3f ms;adv_interval_max:%.3f ms", param.type, param.interval_min * 0.625, param.interval_max * 0.625);
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

    LOGI(TAG, "Bluetooth advertising demo!");
    ble_stack_init(&init);

    start_adv();

    return 0;
}
```
正常运行后，应当可以看到如下Log信息
```
[   0.244]<I>DEMO Bluetooth advertising demo!
[   0.488]<I>DEMO adv start!
[   0.511]<I>DEMO adv_type:0;adv_interval_min:100.000 ms;adv_interval_max:150.000 ms
```
通过手机蓝牙测试APP，可以扫描到一个“YoC ADV"设备，并能进行连接。

**注意点:**
调用adv_test之前，应当按照4.1中说明，注册相应的hci驱动组件。

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。