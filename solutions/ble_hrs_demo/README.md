Title:  BLE HRS应用开发
---

# 1. 简介

HRS Profile提供了标准心率服务，包含了心率测量、RR 区间、能量累计、皮肤接触等功能。本文将展示如何使用蓝牙协议栈开发HRS应用的过程。

# 2. 工作流程

HRS应用主要实现了蓝牙广播、建立连接以及心率数据推送的功能。

![交互流程图](./readme/assets/hrs_procedure.png)

如上图所示，使用蓝牙协议栈来实现一个简单的HRS设备，APP只需关心：

- 应用初始化
- 蓝牙广播
- 心电传感器的驱动和应用逻辑

## 2.1. 应用初始化

应用入口函数main()，主要实现如下功能：

- 蓝牙协议栈的初始化，配置MAC地址及类型

- 注册蓝牙协议栈事件回调处理函数

- 开启广播

- 应用逻辑处理，间隔1秒更新心率数据至对端


代码分析：

```c
int main(void)
{
	......
    /* 板级初始化，各业务模块初始化 */
    board_yoc_init();
    
	/* 蓝牙协议栈初始化参数设置 */
    init_param_t init = {
        .dev_name = node_name,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };
    
    /* 蓝牙协议栈初始化 */
    ble_stack_init(&init);

    /* 注册蓝牙协议栈事件回调函数 */
    ble_stack_event_register(&ble_cb);

    /* HRS组件初始化 */
    g_hrs_handle = hrs_init(&g_hrs);
	......

    while (1) {
        /* 开始广播 */
        ret = start_adv();           
		......
            
        /* 更新心率数据至对端 */
        ret = hrs_measure_level_update(g_hrs_handle, mea_data, mea_size);
		......
            
        aos_msleep(1000);
    }
    return 0;
}
```

- 通过手机的nRF Connect软件可以看到该应用示例的广播数据为

| LEN  | TYPE | VALUE            | 字段说明                   |
| ---- | ---- | ---------------- | -------------------------- |
| 2    | 0x01 | 0x06             | AD_DATA_TYPE_FLAGS         |
| 3    | 0x03 | 0x0D18           | AD_DATA_TYPE_UUID16_ALL    |
| 8    | 0x09 | 0x596F4320485253 | AD_DATA_TYPE_NAME_COMPLETE |

## 2.2. 蓝牙协议栈事件处理

蓝牙协议栈的事件是通过event_callback()回调函数上报，应用程序需要对这些事件进行相应的处理：

- 连接成功或失败事件
- 连接参数更新事件
- 处理对端发起的MTU协商请求

代码分析：

```c
static int event_callback(ble_event_en event, void *event_data)
{
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
            LOGD(TAG, "Unhandle event %x", event);
            break;
    }
    return 0;
}
```

# 3. 例程运行

BLE HRS设备示例程序的运行步骤为：

- 连接串口调试工具, 配置为
  波特率: 115200, 数据位: 8, 校验位: None, 停止位: 1, 流控: None
- 复位开发板
- HRS设备发送广播包，设备名为'YoC HRS'

```c
[   0.255]<I>DEMO Bluetooth HRS demo!
[   0.306]<I>DEMO DEV_NAME:YoC HRS
[   0.436]<I>DEMO hrs adv start!
```

- 手机nRF Connect软件可以扫描到设备名称为'YoC HRS'的设备，点击’Connect‘后设备日志

```c
/* 连接成功事件 */
[   4.436]<D>DEMO event 5000001
[   4.450]<I>DEMO Connected

/* MTU协商事件，更改ATT MTU为247字节，实际payload最大长度为  (MTU - 3), 需要减去3字节HEAD长度 */
[   4.489]<D>DEMO event 5000010
[   4.506]<I>DEMO mtu exchange, MTU 247

/* 连接参数更新事件 */
[ 781.479]<D>DEMO event 5000004
[ 781.484]<I>DEMO LE conn param updated: int 0x0006 lat 0 to 500

[ 781.831]<D>DEMO event 5000004
[ 781.836]<I>DEMO LE conn param updated: int 0x0028 lat 0 to 500
```

- 手机nRF Connect软件使能Heart Rate Measurement属性后，串口将打印'data'显示间隔1秒刷新一次心率数据

```c
/* EVENT_GATT_CHAR_CCC_WRITE事件，示例暂时未处理该事件，需要根据实际APP的行为做处理 */
[900.439]<D>DEMO event 500000e
[ 900.444]<D>DEMO Unhandle event 500000e

/* EVENT_GATT_CHAR_CCC_CHANGE事件，HRS服务中已处理该事件，将使能notify功能，这样，应用才能发送数据至Client */
[ 900.448]<D>DEMO event 500000d
[ 900.451]<D>DEMO Unhandle event 500000d

/* 设备模拟上报HRS数据 */
[ 901.185]<D>HRS data:73
[ 902.193]<D>HRS data:74
[ 903.200]<D>HRS data:75
```

- 手机nRF Connect软件操作Disconnect

```c
/* 连接断开事件 */
[ 424.119]<D>DEMO event 5000001
[ 424.122]<I>DEMO Disconnected

/* 重新开始HRS数据广播 */
[ 424.321]<I>DEMO hrs adv start!
```

