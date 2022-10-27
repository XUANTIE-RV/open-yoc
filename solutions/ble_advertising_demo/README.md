Title:  BLE Advertising应用开发
---

# 1. 简介

本文介绍了如何使用蓝牙协议栈进行蓝牙广播数据的应用开发过程。

# 2. 工作流程

BLE Advertising示例应用主要展示了蓝牙设备的数据广播、MTU协商、连接建立和断开的过程。BLE  Advertising示例应用主要工作流程分为：

- 应用初始化
- 蓝牙协议栈事件处理
- 数据广播

## 2.1. 应用初始化

应用入口函数main(void)，主要实现如下功能：

- 蓝牙协议栈的初始化，配置MAC地址及类型
- 注册蓝牙协议栈事件回调处理函数
- 蓝牙数据广播

代码分析：

```c
int main(void)
{
    /* 定义设备MAC地址以及地址类型 */
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR}; 
    /* 蓝牙协议栈初始化参数设置 */
    init_param_t init = {
        .dev_name = DEVICE_NAME,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };   
    
    /* 板级初始化，各业务模块初始化 */
    board_yoc_init(); 
	......

    /* 蓝牙协议栈初始化 */
    ble_stack_init(&init); 
    
    /* 注册蓝牙协议栈事件回调函数 */
    ble_stack_event_register(&ble_cb); 

    while (1) {
        aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
		/* 检测当前连接状态，未连接则开启广播 */
        if (!connected) {
            start_adv();
        }
    }

    return 0;
}
```

## 2.2. 蓝牙协议栈事件处理

蓝牙协议栈的事件是通过event_callback()回调函数上报，应用程序需要对这些事件进行相应的处理：

- 连接成功或失败事件
- MTU协商请求事件
- 连接参数更新事件

代码分析：

```c
static int event_callback(ble_event_en event, void *event_data)
{    
...
    switch (event) {
        /* 连接成功或失败事件 */
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;
		/* 连接参数更新事件 */
        case EVENT_GAP_CONN_PARAM_UPDATE:
            conn_param_update(event, event_data);
            break;
        /* MTU协商事件 */
        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        default:
            LOGW(TAG, "Unhandle event %x", event);
            break;
    }

    return 0;
}
```

# 3. 蓝牙数据广播

蓝牙数据广播可以设置广播类型、连接参数、AD数据，通过手机的nRF Connect软件可以看到该应用示例的广播数据为

| LEN  | TYPE | VALUE                  | 字段说明                       |
| ---- | ---- | ---------------------- | ------------------------------ |
| 2    | 0x01 | 0x06                   | AD_DATA_TYPE_FLAGS             |
| 3    | 0x03 | 0x0D18                 | AD_DATA_TYPE_UUID16_ALL        |
| 11   | 0xFF | 0x01020304050607080910 | AD_DATA_TYPE_MANUFACTURER_DATA |
| 8    | 0x09 | 0x596F4320414456       | AD_DATA_TYPE_NAME_COMPLETE     |

代码分析：

```c
static int start_adv(void)
{
    int ret;
    /* ad_data_t结构体中，len为实际数据长度。
      而BT规范中定义AD Structure数据格式的LEN = （type字段长度 + 实际数据长度），协议栈内部会根据ad_data_t结构体的内容自动填充PDU */
    ad_data_t ad[2] = {0};
    ad_data_t sd[1] = {0};

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;

    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len = 1;

    /* 设置UUID为0x0D18 */
    uint8_t uuid16_list[] = {0x0d, 0x18};
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)uuid16_list;
    ad[1].len = sizeof(uuid16_list);

    /* 自定义广播数据 */
    uint8_t manu_data[10] = {0x01, 0x02, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};
    sd[0].type = AD_DATA_TYPE_MANUFACTURER_DATA; //自定义广播数据类型
    sd[0].data = (uint8_t *)manu_data;
    sd[0].len = sizeof(manu_data);

    /* 设置参数 */
    adv_param_t param = {
        ADV_IND, //可扫描可连接广播
        ad, //广播数据，可查看BT规范了解AD Structure数据格式定义
        sd, //scan response数据，也符合AD Structure数据格式定义
        BLE_ARRAY_NUM(ad), //AD Structure个数
        BLE_ARRAY_NUM(sd), //scan response数据个数
        /* adv interval time = interval * 0.625 ms */
        ADV_FAST_INT_MIN_2, //最小广播间隔；adv_interval_min:100.000 ms
        ADV_FAST_INT_MAX_2, //最大广播间隔；adv_interval_max:150.000 ms
    };

    /* 开始广播 */
    ret = ble_stack_adv_start(&param);

    if (ret) {
        LOGE(TAG, "adv start fail %d!", ret);
    } else {
        LOGI(TAG, "adv start!");
    }

    LOGI(TAG, "adv_type:%x;adv_interval_min:%.3f ms;adv_interval_max:%.3f ms", param.type, param.interval_min * 0.625, param.interval_max * 0.625);
    return ret;
}
```

# 4. 例程运行

BLE Advertising示例程序的运行步骤为：

- 连接串口调试工具, 配置为
  波特率: 115200, 数据位: 8, 校验位: None, 停止位: 1, 流控: None
- 复位开发板
- 上电启动后，将在串口上输出'adv start'信息

```shell
[   0.006]<I>INIT find 7 partitions
[   0.041]<I>INIT Build:Feb 27 2021,13:45:26
[   0.047]<I>DEMO Bluetooth advertising demo!
[   0.156]<I>DEMO adv start!
[   0.160]<I>DEMO adv_type:0;adv_interval_min:100.000 ms;adv_interval_max:150.000 ms
```

- 手机nRF Connect软件可以扫描到设备名称为'YoC ADV'的设备，点击’Connect‘后设备日志

```c
 /* 连接成功事件 */
[ 118.760]<I>DEMO event 5000001 
[ 118.765]<I>DEMO Connected
/* connect interval time = interval * 1.25 ms */
/* 连接参数更新事件， 设备发起更新，请求连接间隔为7.5ms */
[ 119.779]<I>DEMO event 5000004
[ 119.783]<I>DEMO LE conn param updated: int 0x0006 lat 0 to 500

/* 连接参数更新事件， 手机发起更新，更新连接间隔为50ms */    
[ 119.991]<I>DEMO event 5000004
[ 119.996]<I>DEMO LE conn param updated: int 0x0028 lat 0 to 500
```

- 手机nRF Connect软件断开连接时设备日志

```c
 /* 连接断开事件 */
[1798.460]<I>DEMO event 5000001
[1798.464]<I>DEMO Disconnected

/* 判断连接断开，重新开启广播 */
[1798.470]<I>DEMO adv start!
[1798.475]<I>DEMO adv_type:0;adv_interval_min:100.000 ms;adv_interval_max:150.000 ms
```
