# Change log
20210514
1.bt_gatt_indicate_func_t 函数类型增加一个指针参数，增加指向bt_gatt_indicate_params类型的指针，用于将应用调用bt_gatt_indicate接口时传入的bt_gatt_indicate_params，在回调中传回给应用。

# 概述

蓝牙低功耗（Bluetooth Low Energy，或称Bluetooth LE、BLE，旧商标Bluetooth Smart）也称蓝牙低能耗、低功耗蓝牙，是蓝牙技术联盟设计和销售的一种个人局域网技术，旨在用于医疗保健、运动健身、信标、安防、家庭娱乐等领域的新兴应用。相较经典蓝牙，低功耗蓝牙旨在保持同等通信范围的同时显著降低功耗和成本。

本组件包含了完整的BLE Host协议栈，并提供了SIG标准定义的profile，包含HRS，HID，BAS等。

开发者如果需要完整地运行该组件功能，需要准备一个或者多个支持BLE功能的开发板，支持BLE功能的智能手机，相应的测试APP。配套的示例程序，开发者可以选用YoC Solutions中包含的ble_xxx相关的示例。

开发者如果需要将该组件移植到其他的支持BLE功能的芯片上，可以参考《[YoC BLE HOST组件对接示例](https://yoc.docs.t-head.cn/yocbook/Chapter8-%E8%8A%AF%E7%89%87%E5%AF%B9%E6%8E%A5/%E8%93%9D%E7%89%99%E9%A9%B1%E5%8A%A8%E5%AF%B9%E6%8E%A5%E4%BE%8B%E7%A8%8B.html)》《[YoC蓝牙驱动移植](https://yoc.docs.t-head.cn/yocbook/Chapter8-%E8%8A%AF%E7%89%87%E5%AF%B9%E6%8E%A5/%E8%93%9D%E7%89%99%E9%A9%B1%E5%8A%A8%E7%A7%BB%E6%A4%8D.html) 》文档。

## 组件安装

```bash
yoc init
yoc install ble_host
```

# 配置

本组件的默认配置位于components\ble_host\bt_host\include\aos\bt_default_config.h文件中，对于开发者，不需要修改默认值，如果有修改的需求，需要将新的定义配置到相应Solution中的package.yaml文件，新的配置将覆盖默认值。

下面是本组件的一些配置选项

| 配置                        | 默认值     | 说明                                                         |
| --------------------------- | ---------- | ------------------------------------------------------------ |
| CONFIG_BT                   | 1          | 本组件功能宏，引入该组件后默认定义                           |
| CONFIG_BT_CENTRAL           | 0          | 启用CENTRAL特性                                              |
| CONFIG_BT_PERIPHERAL        | 1          | 启用PERIPHERAL特性                                           |
| CONFIG_BT_BROADCASTER       | 1          | 启用BROADCASTER特性                                          |
| CONFIG_BT_OBSERVER          | 0          | 启用OBSERVER特性                                             |
| CONFIG_BT_MAX_CONN          | 1          | 配置最大可连接数                                             |
| CONFIG_BT_HCI_TX_STACK_SIZE | 1536       | 发送任务栈大小，单位bytes，通常不应当修改，如果出现发送任务栈偏小导致的异常，可以适当加大 |
| CONFIG_BT_RX_STACK_SIZE     | 2048       | 接收任务栈大小，单位bytes，通常不应当修改，如果出现接收任务栈偏小导致的异常，可以适当加大 |
| CONFIG_BT_SETTINGS          | 1          | 启用协议栈信息存储功能                                       |
| CONFIG_BT_SMP               | 0          | 启用SMP安全特性                                              |
| CONFIG_BT_PRIVACY           | 0          | 启用Privacy特性，启用后将使用Resolvable Private Addresses    |
| CONFIG_BT_SIGNING           | 0          | 启用数据签名特性                                             |
| CONFIG_BT_GATT_CLIENT       | 1          | 启用GATT Client特性                                          |
| CONFIG_BT_DEVICE_NAME       | “YoC Test” | 默认蓝牙设备名称，在应用方案中可以通过API修改。              |
| CONFIG_BT_DEVICE_APPEARANCE | 0          | 蓝牙设备外观，含义查看https://www.bluetooth.com/specifications/assigned-numbers/ |
| CONFIG_BT_ECC               | 0          | 启用ECC加密特性，启用后协议栈支持Secure Connections          |
| CONFIG_BT_SHELL             | 0          | 启用协议栈Shell特性，该配置打开后，支持通过Shell命令控制协议栈，相关shell命令参考[《BLE SHELL用户手册》](docs/BLE_SHELL_user_manual.md) |
| CONFIG_BT_DEBUG_LOG         | 0          | 启用协议栈调试特性，该配置打开后，其余子模块的调试配置生效。 |
| CONFIG_BT_DEBUG_SETTINGS    | 0          | 启用协议栈信息存储特性调试                                   |
| CONFIG_BT_DEBUG_HCI_CORE    | 0          | 启用协议栈HCI特性调试                                        |
| CONFIG_BT_DEBUG_CONN        | 0          | 启用协议栈连接特性调试                                       |
| CONFIG_BT_DEBUG_KEYS        | 0          | 启用协议栈密钥调试                                           |
| CONFIG_BT_DEBUG_L2CAP       | 0          | 启用协议栈L2CAP调试                                          |
| CONFIG_BT_DEBUG_SMP         | 0          | 启用协议栈SMP调试                                            |
| CONFIG_BT_DEBUG_ATT         | 0          | 启用协议栈ATT调试                                            |
| CONFIG_BT_DEBUG_GATT        | 0          | 启用协议栈GATT调试                                           |
| CONFIG_BT_BREDR             | 0          | 启用协议栈BR特性                                      |
| CONFIG_BT_HFP_HF            | 0          | 启用协议栈HFP特性                                          |
| CONFIG_BT_HFP               | 0          | 启用协议栈HFP_HF特性                                          |
| CONFIG_BT_HFP_AUDIO_I2S     | 0          | HFP的音频传输使用I2S传输                                            |
| CONFIG_BT_A2DP              | 0          | 启用协议栈A2DP特性                                            |
| CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE | 0          | A2DP的音频播放在协议栈内部完成                          |
| CONFIG_BT_MAX_PAIRED        | 2          | 保存的linkkey个数                                           |

# 接口

本组件对外头文件位于components\ble_host\bt_host\include\aos\ble.h，通过#include <aos/ble.h>进行引用。其中API相关说明，参考《[BLE API](docs/ble_api/ble_api.md)》文档

# 示例

如下是一个简单的广播示例，其他示例参考Solutions中ble_xxx相关示例程序。

```c
/* 引用BLE Host组件头文件 */
#include <aos/ble.h>
/* 引用AOS头文件 */
#include <aos/aos.h>

/* 定义蓝牙设备名称 */
#define DEVICE_NAME "YoC ADV"
/* 定义蓝牙设备地址 */
#define DEVICE_ADDR {0xCC,0x3B,0xE3,0x82,0xBA,0xC0}

#define TAG "DEMO"

/* 蓝牙广播开始函数 */
static int start_adv(void)
{
    int ret;
    ad_data_t ad[2] = {0};
    ad_data_t sd[1] = {0};

    /* 配置广播数据和扫描响应数据 */
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

    /* 配置广播参数 */
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

    return ret;
}

/* 连接事件处理 */
void conn_change(ble_event_en event, void *event_data)
{
    /* 连接事件对应的事件数据 */
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        LOGI(TAG, "Connected");
    } else {
        LOGI(TAG, "Disconnected");
    }
}

/* BLE Host事件回调函数 */
static int event_callback(ble_event_en event, void *event_data)
{
    LOGI(TAG, "event %x", event);

    switch (event) {
        /* 连接变化事件 */
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        default:
            LOGW(TAG, "Unhandle event %x", event);
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};

int adv_test(void)
{
    /* BLE Host初始化参数配置 */
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = DEVICE_NAME,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };

    LOGI(TAG, "Bluetooth advertising demo!");
    
    /* BLE Host初始化 */
    ble_stack_init(&init);

    /* BLE Host注册事件回调 */
    ble_stack_event_register(&ble_cb);
    
    /* 开始BLE广播 */
    start_adv();

    return 0;
}
```

# 错误码

ble_stack_xxx 返回错误码说明

| BLE Stack错误码说明       |              |
| ------------------------- | ------------ |
| BLE_STACK_OK = 0,         | 成功       |
| BLE_STACK_ERR_NULL=1,     | 参数为空指针 |
| BLE_STACK_ERR_PARAM=2,    | 参数错误     |
| BLE_STACK_ERR_INTERNAL=3, | 内部错误     |
| BLE_STACK_ERR_INIT=4,     | 初始化错误   |
| BLE_STACK_ERR_CONN=5,     | 连接错误     |
| BLE_STACK_ERR_ALREADY=6,  | 重复操作   |

HOST协议栈其他错误请参考 errno.h 中定义

HCI底层返回错误码参考 hci_err.h 中定义 BT_HCI_ERR_XXX

# 运行资源

不同方案下资源消耗参考

| Solution                     | Code size/Bytes | Ro data/Bytes | RW Data/Bytes | BSS/Bytes |
| ---------------------------- | --------------- | ------------- | ------------- | --------- |
| ble_advertising_demo         | 24650           | 756           | 1204          | 11041     |
| ble_hid_keyboard_demo        | 38808           | 1266          | 2621          | 11804     |
| ble_uart_profile_client_demo | 32030           | 969           | 1164          | 11384     |
| ble_uart_profile_server_demo | 36140           | 1023          | 1574          | 11612     |
| ble_shell_demo               | 63950           | 8389          | 3835          | 14105     |
| genie_mesh_light_ctl         | 33942           | 1812          | 1188          | 11775     |
| genie_mesh_node_ctl          | 33942           | 1807          | 1188          | 11775     |
| genie_mesh_switches          | 33942           | 1815          | 1188          | 11775     |

# 依赖资源

- kv