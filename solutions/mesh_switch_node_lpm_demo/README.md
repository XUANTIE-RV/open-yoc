## 蓝牙Mesh低功耗节点应用开发指南

# 1. 蓝牙Mesh低功耗介绍

​       该低功耗方案支持TX_ONLY(只发送)、TX_RX（发送接收）、RX_TX（接收发送）三种工作模式， 相比LPN方案需要低功耗节点与Friend节点建立连接，本低功耗的方案无需建立连接，休眠和激活的占空比是固定的，可以较为准确的计算出设备的待机功耗，运行也相对稳定，该方案需配合蓝牙Mesh网关使用。

​      本文档主要介绍RX_TX模式，以及如何基于RX_TX模式进行低功耗开关等品类的开发。该示例支持三个Element,可以参考该示例进行多Element应用开发。该模式下设备工作在上下行状态。 该类低功耗设备的主要应用场景是对数据实时性要求较高、对数据丢失敏感且需要接收下行控制消息的品类，如单火开关等，在该模式下设备需以固定的时间间隔唤醒（如1.2s)，可使用以下两种方法对设备进行唤醒：
（1）I/O口唤醒设备
  需要芯片支持低功耗唤醒功能，在设备唤醒后，需在较短的时间内完成用户指定的动作
（2）定时唤醒设备
  设备可按照用户指定的周期（最大不超过1.2s），对设备进行唤醒
  该模式下设备唤醒后，需对协议栈进行resume,开启指定周期和大小的rx窗口(60ms)，如单火品类在打开RX窗口后，若接收到相应的控制消息，在执行完相应的开关动作后，将会以一定 的发包间隔进行ACK操作。

 

# 2. 应用开发

## 2.1. 应用初始化

应用入口函数app_main()，主要实现如下功能：

- 板级初始化
- MESH 低功耗开关 Model定义
- Mesh节点初始化，注册Model事件回调处理函数

代码分析：

```
/* 定义设备名称，使用手机APP扫描时可以看到该名称 */
#define DEVICE_NAME "YoC Mesh Switch"

/* 定义设备UUID */
#define DEVICE_UUID {0xcf, 0xa0, 0xea, 0x72, 0x17, 0xd9, 0x11, 0xe8, 0x86, 0xd1, 0x5f, 0x1c, 0xe2, 0x8a, 0xdf, 0x00}

node_config_t g_node_param = {
    /* 设置节点的UUID */
    .dev_uuid = DEVICE_UUID,
    /* 设置节点的Device Name */
    .dev_name = DEVICE_NAME,
    /* 注册Model的事件处理回调函数 */
    .user_model_cb = app_models_event_cb,
    /* 注册Provision的事件处理回调函数 */
    .user_prov_cb = app_prov_event_cb,
    /* Health model回调函数*/
    .health_cb = &g_app_health_cb,
};

/*Mesh lpm回调处理函数*/
#ifdef CONFIG_BT_MESH_LPM
__attribute__((section(".__sram.code"))) static void switches_lpm_cb(mesh_lpm_wakeup_reason_e reason, mesh_lpm_status_e status, void *arg)
{   
    /*判定是否为唤醒状态*/
    if (status == STATUS_WAKEUP) {
        if (reason == WAKEUP_BY_IO) {
            _mesh_lpm_io_status_list_t *list = (_mesh_lpm_io_status_list_t *)arg;
            for (int i = 0; i < list->size; i++) {
                if (list->io_status[i].trigger_flag == true) {
                    extern void _key_pres_process(uint8_t port);
                    /*判定是否为I/O唤醒*/
                    _key_pres_process(list->io_status[i].port);
                }
            }
        }
    } else {
        LOGI(TAG,"sleep");
    }
}
#endif

/*I/O唤醒最终的处理函数*/
static void handle_input_event(uint8_t press_data)
{
#ifdef CONFIG_BT_MESH_LPM
    static uint8_t sleep_toggle_flag = 0;
#endif
    uint8_t port = press_data & 0x0F;
    uint8_t press_type = (press_data & 0xF0) >> 4;

    switch (press_type) {
    /*短按处理*/
    case SWITCH_PRESS_ONCE: {
        LOGI(TAG,"port %d short press", port);
        uint8_t elem_id = 0;
        /*判断按下的I/O口*/
        if (port == INPUT_EVENT_PIN_1) {
            elem_id = 0;
        } else if (port == INPUT_EVENT_PIN_2) {
            elem_id = 1;
        } else if (port == INPUT_EVENT_PIN_3) {
            elem_id = 2;
        }
        g_status_data[elem_id].onoff = !g_status_data[elem_id].onoff;
        /*根据按下的I/O口点亮对应的灯*/
        switch_led_set(elem_id, g_status_data[elem_id].onoff);
        /*上报对应I/O口的开关状态*/
        switches_report_start(elem_id,g_status_data[elem_id].onoff);
    }
    break;
    /*长按处理*/
    case SWITCH_PRESS_LONG: {
        LOGD(TAG,"port %d long press", port);
#ifdef CONFIG_BT_MESH_LPM
        /*长按切换低功耗开关状态*/
        if (sleep_toggle_flag == 0) {
            mesh_lpm_disable();
            sleep_toggle_flag = 1;
            LOGD(TAG,"genie_lpm_disable");
        } else if (sleep_toggle_flag == 1) {
            mesh_lpm_enable(FALSE);
            sleep_toggle_flag = 0;
            LOGD(TAG,"genie_lpm_enable");
        }
#endif
    }
    break;

    default:
        break;
    }
}


/*配置Mesh lpm*/
static void switches_lpm_init()
{
#ifdef CONFIG_BT_MESH_LPM
/*配置唤醒I/O口以及I/O唤醒方式*/
 mesh_lpm_wakeup_io_config_t io[] = {MESH_WAKEUP_PIN(INPUT_EVENT_PIN_1,      INPUT_PIN_POL_PIN_1),
                                        MESH_WAKEUP_PIN(INPUT_EVENT_PIN_2, INPUT_PIN_POL_PIN_2),
                                        MESH_WAKEUP_PIN(INPUT_EVENT_PIN_3, INPUT_PIN_POL_PIN_3)
                                       };

    mesh_lpm_conf_t      lpm_config;
    /*Mesh lpm是否允许自动进入，使能后节点会在配网完成后自动进入低功耗*/
    lpm_config.is_auto_enable = 1;
    /*Mesh lpm工作模式，这里配置为RX_TX模式*/
    lpm_config.lpm_mode   = MESH_LPM_MODE_RX_TX;
    /*Mesh lpm唤醒模式，这里设置为I/O唤醒、定时器唤醒模式*/
    lpm_config.lpm_wakeup_mode = WAKEUP_BY_IO_TIMER_MODE;
    /*I/O唤醒端口个数*/
    lpm_config.lpm_wakeup_io_config.io_list_size = sizeof(io) / sizeof(io[0]);
    /*I/O唤醒端口配置列表*/
	lpm_config.lpm_wakeup_io_config.io_config = &io;
    /*Mesh lpm延迟进入睡眠时间，这里设置为设备在配网状态下，重启后20S进入低功耗状态，需注意若想重新配置网络的部分参数，需在设备还未进入低功耗状态进行*/
    lpm_config.delay_sleep_time = 20 * 1000; //Unit:ms
    /*Mesh lpm,唤醒回调处理函数*/
    lpm_config.mesh_lpm_cb = switches_lpm_cb;
    /*Mesh lpm初始化*/
    mesh_lpm_init(&lpm_config);
#endif
    return;
}

int app_main(int argc, char *argv[])
{   
    /* 板级初始化，各业务模块初始化 */
    board_yoc_init(); 
    ......
   
    /* MESH 应用Model定义,Mesh Node初始化*/
    ret = mesh_dev_init();
    ......

    /*I/O硬件接口初始化、I/O中断消息处理队列初始化*/
    body_sensor_init();

#ifdef CONFIG_BT_MESH_LPM
    /*Mesh lpm初始化*/
    switches_lpm_init();
    /*判定是否已配网*/
    if(bt_mesh_is_provisioned()) {
        /*开启mesh lpm*/
        mesh_lpm_start();
    }
#endif
    ......
    /* Wait for button signal to publish on/off command to default group */
    while (1) {
		if (queue_recv_data(&queue_mesg) < 0)
        {
            continue;
        }

        if (QUEUE_MESG_TYPE_INPUT == queue_mesg.type)
        {   
            /*I/O中断消息处理*/
            press_data = queue_mesg.data;
            handle_input_event(press_data);
        }
    }

    return 0;
}
```

## 2.2. 设备模型定义

MESH解决方案中，模型用于定义设备节点的功能。以MESH Switch Lpm Demo为例，可以定义下列模型：

- Configuration Server Model

  这个模型用于存储节点的MESH网络配置。

- Health Server Model

  该模型主要用于MESH网络诊断。

- Generic Onoff Srv Model

  该模型用于设置传感器的开关属性

代码分析：

```
/*Element 0 Sig Model组成*/
static struct bt_mesh_model elem0_root_models[] = {
    /* 本设备节点的Configuration Server模型定义 */
    BT_MESH_MODEL_CFG_SRV_NULL(),
    /* 本设备节点的Health Server模型定义 */
    BT_MESH_MODEL_HEALTH_SRV_NULL(),
    /* 本设备节点的Generic Onoff Cli Server模型定义 */
    MESH_MODEL_GEN_ONOFF_CLI_NULL(),
};
/*Element 0 Vendor Model组成*/
static struct bt_mesh_model elem0_vnd_models[] = {
    MESH_MODEL_VENDOR_SRV_NULL(),
};
/*Element 1 Sig Model组成*/
static struct bt_mesh_model elem1_root_models[] = {
    MESH_MODEL_GEN_ONOFF_SRV_NULL(),
};
/*Element 1 Vendor Model组成*/
static struct bt_mesh_model elem1_vnd_models[] = {
    MESH_MODEL_VENDOR_SRV_NULL(),
};

/*Element 2 Sig Model组成*/
static struct bt_mesh_model elem1_root_models[] = {
    MESH_MODEL_GEN_ONOFF_SRV_NULL(),
};
/*Element 2 Vendor Model组成*/
static struct bt_mesh_model elem1_vnd_models[] = {
    MESH_MODEL_VENDOR_SRV_NULL(),
};

/* 本设备节点的Elements定义 */
static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, elem0_root_models, elem0_vnd_models, 0),
    BT_MESH_ELEM(0, elem1_root_models, elem1_vnd_models, 0),
    BT_MESH_ELEM(0, elem2_root_models, elem2_vnd_models, 0),
};

/* 本设备节点的Composition Datas定义，定义CompanyID、Elements */
static const struct bt_mesh_comp mesh_comp = {
    /* 设置设备节点的CompanyID */
    .cid = 0x01A8,
     /* 设置设备节点的Elements */
    .elem = elements,
    /* 设置设备节点Elements的个数 */
    .elem_count = ARRAY_SIZE(elements),
};

int mesh_dev_init()
{
    int ret;
    /* Models初始化 */
    ret = ble_mesh_model_init(&mesh_comp);
    return ret;
}
```

***注意：\***

***网络中需要存在多个节点时，需要更改代码中设备UUID后编译烧录该镜像，并烧录镜像时设置不同的MAC地址，避免组网出现问题 。\***

# 3. 例程运行

## 3.1. 编译&烧录

现对Solution中部分配置选项进行说明：

-   CONFIG_BT_MESH_GATT_PROXY: 1 - PROXY功能 
-  CONFIG_BT_MESH_PB_GATT: 1 - GATT功能，PROYX和GATT有依赖关系，需同时打开/关闭，打开后功耗将上升，若需支持Gatt配网或者OTA需打开
-   CONFIG_PM_SLEEP: 1 - 芯片底层低功耗使能
-   CONFIG_BT_MESH_LPM: 1 - MESH协议栈低功耗使能，若使能了该功能，需同时打开CONFIG_PM_SLEEP
-   MESH_DEBUG_TX: 1 - MESH 发包打印（调试用，打开后功耗上升）
-   MESH_DEBUG_RX: 1-  MESH 收包打印（调试用，打开后功耗上升）
-  CONFIG_OTA_CLIENT: 1- OTA功能
- CONFIG_OCC_AUTH: 1 - MESH OCC鉴权入网，需要烧录KP文件后同网关配合使用

根据实际需要修改Solution 下的yaml配置文件后，参考《[PHY6220快速上手手册](../../bluetooth/board/quick_start_of_PHY6220.html)》编译和烧录 mesh_body_sensor_demo示例

## 3.2. 运行

- 连接串口调试工具, 配置为 波特率: 115200, 数据位: 8, 校验位: None, 停止位: 1, 流控: None
- 开发板拨码开关拨至GND
- 按RESET按键，复位开发板
- 设备将开启Unprovisoin Device Beacon广播，串口上输出'Mesh Temperature Sensor'信息

```shell
[   0.006]<I>INIT find 8 partitions
/*硬复位计数，首次上电没有存储信息，3秒内再次复位开发板计数将加1，重复5次即可清除入网信息*/
[   0.088]<I>MESH_RESET reset_by_repeat_init, number = 1
[   0.099]<I>INIT Build:Jul 11 2021,16:09:05
[   0.105]<I>DEMO Mesh Switch node lpm demo v1.0.0
[   0.222]<I>MESH_LPM lpm mode: 02 wakeup mode:03
[   0.235]<I>MESH_LPM sleep:1100 ms, wakeup:60 ms
```

- MESH Lpm低功耗demo需配合网关使用，这里介绍使用网关AT功能对设备进行配网以及配置

  - 网关输入如下命令入网设备：

    ```shell
    /*打开设备发现*/
    -> AT+MESHPROVSHOWDEV=0x01
    /*发现该设备*/
    <- +MESHDEV:C1:00:00:00:00:03,00,0300000000c111e886d15f1ce28adf00,00,01
    /*添加设备*/
    ->AT+MESHADDDEV=C1:00:00:00:00:03,00,0300000000c111e886d15f1ce28adf00,00,01
    /*设备添加成功*/
    <- +MESHNODEADD:0007,03,0300000000c111e886d15f1ce28adf00 
    <- +MESHAPPKEYADD:0007,0
    /*自动配置设备*/
    ->AT+MESHAUTOCONFIG=0x0007
    /*自动配置成功*/
    <- +MESHAUTOCONFIG:0007,0
    /*对于需要收包的低功耗节点，使用前需将需设置对应节点首要element unicast_addr为低功耗节点*/
    -> AT+MESHPROVSETNODELPM=0x0007,0x01
    /*设置低功耗开关各element onoff srv pub地址*/
    -> AT+MESHSETPUB=0x0007,0x1000,0xC004,0x03,0x40,0,0x0
    /*设置成功*/
    <- +MESHPUBSET:0007,0
    -> AT+MESHSETPUB=0x0008,0x1000,0xC004,0x03,0x40,0,0x0
    /*设置成功*/
    <- +MESHPUBSET:0008,0
    -> AT+MESHSETPUB=0x0009,0x1000,0xC004,0x03,0x40,0,0x0
    /*设置成功*/
    <- +MESHPUBSET:0009,0
    /*设置成功*/
    <- +MESHPUBSET:0008,0
    /*打开低功耗开关0*/
    <- +MESHPUBSET:0006,0
    /*网关订阅该地址*
    ->AT+MESHSETSUB=0x0001,0x1000,0xC004
    /*打开低功耗开关*/
    -> AT+MESHONOFF=0x07,0x01,0x01
    /*按下开发板INPUT_EVENT_PIN_1按键，模拟一次开关触发信号，网关收到该开关状态信号*/
    <- +MESHONOFF:0007,01
    ```

    

