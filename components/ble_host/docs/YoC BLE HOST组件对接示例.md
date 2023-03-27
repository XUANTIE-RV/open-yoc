# YoC BLE HOST组件对接示例

<a name="3imLv"></a>
# 1.简介
YoC平台中包含的BLE Host组件，实现了BT5.0协议中的大部分特性。任何支持BLE特性的芯片，通过实现一套简单的HCI接口，即可对接到该组件，从而可以方便地使用YoC平台提供的各种蓝牙方案。<br />本文将以REALTEK RTL8723DS为例，介绍如何对接到ble host组件，并进行简单的测试，验证。
<a name="fHuFJ"></a>
# 2.组件介绍
                       ![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/172330/1601212229269-7c9d866e-8597-402b-9b39-219a2e05269c.png#align=left&display=inline&height=247&margin=%5Bobject%20Object%5D&name=image.png&originHeight=494&originWidth=1022&size=32118&status=done&style=none&width=511)<br />                                                                ble_host 组件依赖 <br />上图是ble host组件对其他YoC组件的依赖情况。在对接ble host之前需要将依赖组件对接完成。

- kv

         一个以Key-Value方式进行持久化存储的轻量级组件。<br />ble host组件使用kv组件存储蓝牙协议栈所需要的信息。

- aos

aos kernel api。<br />ble host组件基于aos kernel api使用内核功能。

- rhino

Rhino实时操作系统内核，体积小、功耗低、实时性、调试方便等特点。

- ulog

sys log output,support async output。<br />ble host调试信息输出。

- drv_bt_xxxx

hci驱动组件。ble host组件通过hci接口同ble芯片的LL层进行通信。开发者只需要实现该组件，即可完成对接。<br />

<a name="vULvc"></a>
# 3. HCI Driver API介绍

<br />参考YoC Book 蓝牙驱动移植章节中，API介绍，[链接地址](https://yoc.docs.t-head.cn/yocbook/Chapter8-%E8%8A%AF%E7%89%87%E5%AF%B9%E6%8E%A5/%E8%93%9D%E7%89%99%E9%A9%B1%E5%8A%A8%E7%A7%BB%E6%A4%8D.html)

| API | 说明 |
| --- | --- |
| rvm_hal_hci_open | hci设备打开接口 |
| rvm_hal_hci_close | hci设备关闭接口 |
| rvm_hal_hci_set_event | hci设置事件回调接口 |
| rvm_hal_hci_send | hci数据发送接口 |
| rvm_hal_hci_recv | hci数据接收接口 |
| rvm_hal_hci_start | hci设备启动接口 |

<a name="Ed3HB"></a>
# 4. 示例说明
<a name="XBg3D"></a>
## 4.1 驱动注册接口
每个hci驱动组件需要实现一套自己的注册接口，作为该组件的注册入口。<br />hci驱动组件需要实现hci_driver_t中定义的api回调函数。
```c
static rvm_dev_t *h5_hal_init(driver_t *drv, void *g_uart_config, int id)
{
    hci_driver_t *h5_dev = (hci_driver_t *)rvm_hal_device_new(drv, sizeof(hci_driver_t), id);

    return (rvm_dev_t *)h5_dev;
}

#define h5_hal_uninit rvm_hal_device_free

static hci_driver_t h5_driver = {
    .drv = {
        .name   = "hci",
        .init   = h5_hal_init,
        .uninit = h5_hal_uninit,
        .lpm    = NULL,
        .open   = h5_hal_open,
        .close  = h5_hal_close,
    },
    .set_event = h5_set_event,
    .send      = h5_send_data,
    .recv      = h5_recv_data,
    .start     = h5_start,
};

/**
 * @brief  register bluetooth driver of rtl8723 
 * @param  [in] config
 * @return  
 */
void bt_rtl8723ds_register(rtl8723ds_bt_config *config)
{
    g_bt_dis_pin = config->bt_dis_pin;
    g_uart_id = config->uart_id;
    rvm_driver_register(&h5_driver.drv, NULL, 0);
}
```
solution在使用该驱动前，应当调用该注册接口，使用方式如下
```c
    rtl8723ds_bt_config config = {
        .uart_id = BT_UART_IDX,
        .bt_dis_pin = BT_DIS_PIN,
    };

    bt_rtl8723ds_register(&config);

    extern int hci_h5_driver_init();
    hci_h5_driver_init();
```
在调用bt_rtl8723ds_register后，该设备会注册进入YoC设备列表，并且自动调用h5_hal_init<br />
<br />**注意点:**<br />1、.drv.name名字需要固定为"hci"， ble host会根据该名字打开hci驱动。<br />2、REALTEK RTL8723DS芯片的hci接口，使用的是3线uart方式，采用h5的协议，支持软件流控。<br />需要实现.drv.start接口。如果使用4线uart，带硬件流控方式，则不需要对接该接口。<br />关于h4，h5协议的介绍，参考蓝牙协议文档《Bluetooth Core Specification》4.2 以上版本，Vol 4. Host Controller Interface章节。<br />3、调用bt_rtl8723ds_register接口，solution中需要根据使用的是H5协议，还是H4协议，调用hci_h5_driver_init或者hci_h4_driver_init接口。<br />开发者也可以将该接口封装到register接口中。
<a name="k4xlt"></a>

## 4.2 rvm_hal_hci_open
在hci open接口实现中，hci驱动组件需要初始化hci依赖的外设，并进行相应的配置。
```c
static int h5_hal_open(rvm_dev_t *dev)
{
    ...
    uart_opne;
    rvm_hal_uart_config;
    ...
    return 0;
}
```
ble_host将按照如下的方式使用rvm_hal_hci_open
```c
rvm_hal_hci_open("hci");
```
<a name="rO18q"></a>
## 4.3 rvm_hal_hci_start接口
在初始化的时候会调用，hci驱动组件可以做一些硬件初始化。<br />send_cmd为HCI CMD回调函数。hci驱动组件可以使用该接口，发送自定义的HCI命令。
```c
static int h5_start(rvm_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    hw_config_start(send_cmd);
    return 0;
}
```
ble_host调用rvm_hal_hci_open后，随后就会调用rvm_hal_hci_start接口。
<a name="gd10x"></a>
## 4.4  rvm_hal_hci_set_event接口
hci回调事件注册接口，hci驱动组件需要记录下设置的回调函数和参数
```c
static int h5_set_event(rvm_dev_t *dev, hci_event_cb_t event, void *priv)
{
    g_event = event;
    g_priv = priv;

    return 0;
}
```
<a name="s10Nd"></a>
## 4.5 rvm_hal_hci_send接口
hci数据发送接口，hci驱动组件需要实现将hci数据发送给SoC LL底层的功能。
```c
static int h5_send_data(rvm_dev_t *dev, uint8_t *data, uint32_t size)
{
    rvm_hal_uart_send;
}
```
ble_host组件在需要送hci命令或者数据的时候会调用该接口。
<a name="ycvpO"></a>
## 4.6 rvm_hal_hci_recv接口
hci数据接收接口，hci驱动组件，在接收到来自SoC LL的事件或者数据后，需要缓存该数据，在ble_host调用rvm_hal_hci_recv接口后，返回对应的数据内容。
```c
static int h5_recv_data(rvm_dev_t *dev, uint8_t *data, uint32_t size)
{
    rvm_hal_uart_recv;
}
```
<a name="qJ4be"></a>
# 5. hci初始化/数据发送说明
             ![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/172330/1601217453038-cfbd0b63-ed11-44f6-9dce-1d52ea8c0cd0.png#align=left&display=inline&height=314&margin=%5Bobject%20Object%5D&name=image.png&originHeight=508&originWidth=813&size=25160&status=done&style=none&width=503)<br />在ble host组件初始化的时候，会依次调用rvm_hal_hci_open,rvm_hal_rvm_hal_hci_start,rvm_hal_hci_set_event接口完成hci驱动的初始化。<br />在hci驱动初始化完成后，ble host根据协议流程，会发送hci命令，实现协议栈的初始化。
<a name="MnwrY"></a>
# 6. hci数据接收说明
       ![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/172330/1601217376807-42f33efa-4d26-4b23-b9cc-179f1f877536.png#align=left&display=inline&height=278&margin=%5Bobject%20Object%5D&name=image.png&originHeight=403&originWidth=794&size=21588&status=done&style=none&width=548)<br />hci数据接收过程相对复杂一些。hci驱动组件在初始化的时候，需要记录rvm_hal_hci_set_event接口这是的回调函数hci_event_callback。<br />当ll有数据上报的时候，hci驱动组件，需要缓存来自ll的数据或者事件，通过hci_event_callback回调，通知ble host有数据上报。ble host组件会根据上报的类型，调用rvm_hal_hci_recv接口，获取hci数据，并进行处理。
<a name="BeDmU"></a>
# 7. hci驱动组件测试验证
HCI组件对接完成后，可以运行一个简单广播示例来确认是否已经对接成功。<br />示例代码如下
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

**注意点:**<br />调用adv_test之前，应当按照4.1中说明，注册相应的hci驱动组件。
