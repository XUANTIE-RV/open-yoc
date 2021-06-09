# 概述

SmartLiving 是阿里云智能生活SDK，包含阿里云连接、智能生活APP绑定、远程控制、配网等功能。

### 配网

WiFi设备需要连接到WiFi热点(WiFi AP)之后才能与其它设备进行基于IP的通信, 我们将WiFi设备获取到WiFi热点的SSID/密码的步骤称为WiFi配网。

smartliving组件支持三种配网方式，分别是BLE辅助配网、一键配网及设备热点配网。

### 升级

Smartliving OTA模块，通过基于TLS的HTTPS建立安全的下载通道，从云端将经过压缩的OTA image下载到芯片Flash的misc分区，下载完成并完成完整性校验后，系统重启，然后经由BootLoader将misc分区的image解压并搬移到应用程序运行的主分区prim分区，然后运行新版本的程序并向云端报告新版本的版本号。这样就完成了整个OTA过程。

# 示例代码

#### 初始化

程序初始化时，注册所需配网模块，可同时注册多种。

启动配网服务，首先需要，然后调用`wifi_prov_start`函数启动服务

```c
#include <wifi_provisioning.h>

/*注册生活物联网平台配网服务*/
wifi_prov_sl_register();
```

#### 启动配网

需要配网时，启动对应的配网模块，进入配网流程。

```c
#include <wifi_provisioning.h>

/*启动配网服务，超时时间120秒*/
case WIFI_PROVISION_SL_DEV_AP:
     wifi_prov_start(wifi_prov_get_method_id("sl_dev_ap"), wifi_pair_callback, 120);
     break;

case WIFI_PROVISION_SL_SMARTCONFIG:
     wifi_prov_start(wifi_prov_get_method_id("sl_smartconfig"), wifi_pair_callback, 120);
     break;

case WIFI_PROVISION_SL_BLE:
     wifi_prov_start(wifi_prov_get_method_id("sl_ble"), wifi_pair_callback, 120);
     break;
```

#### 事件回调函数

配网成功或失败都会调用该函数，函数的参数中`event`变量可以确认结果，如果由多种配网同时启动可以从`method_id`确认那种配网，配网参数从`result`返回，可使用该参数去连接网络。

```c
#include <wifi_provisioning.h>

static void wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    if (event == WIFI_PROV_EVENT_TIMEOUT) {
        /*配网超时*/
        LOGD(TAG, "wifi pair timeout...");
    } else if (event == WIFI_RPOV_EVENT_GOT_RESULT) {
        /*配网成功，获取配网参数*/
        LOGD(TAG, "wifi pair got passwd ssid=%s password=%s...", result->ssid, result->password);
    }
}
```

#### 停止配网

调用该函数停止配网流程，若启动多种配网也会全部停止，退出配网状态。

```c
#include <wifi_provisioning.h>

/*停止配网*/
wifi_prov_stop();
```

