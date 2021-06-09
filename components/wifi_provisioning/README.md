# WiFi配网功能的接口模块(wifi_provisioning)

## 概述

WiFi Provisioning API 是一套简单易用的WiFi网络配置接口。

## 组件安装

```bash
yoc init
yoc install  wifi_provisioning
```

## 接口列表

| 函数                      | 说明                   |
| :------------------------ | :--------------------- |
| wifi_prov_method_register | 注册配置方法           |
| wifi_prov_start           | 启动配置               |
| wifi_prov_stop            | 停止配置               |
| wifi_prov_get_method_id   | 按照名称获取配置方法id |
| wifi_prov_get_stauts      | 获取配置方法状态       |

## 接口详细说明

### wifi_prov_method_register

`int wifi_prov_method_register(wifi_prov_t *prov);`

- 功能描述:
  - 注册配置方法。

- 参数:
  - `prov`: 配置方式。

- 返回值:
  - method_id: 成功。
  - -1: 失败。

### wifi_prov_start

`int wifi_prov_start(uint32_t method_ids, wifi_prov_cb cb, uint32_t timeout_s);`

- 功能描述:
  - 启动配置。

- 参数:
  - `method_ids`: 配置方法id。
  - `cb`: 回调函数。
  - `timeout_s`: 超时时间。

- 返回值:
  - method_ids: 失败。
  - 0: 成功。

### wifi_prov_stop

`void wifi_prov_stop(void);`

- 功能描述:
  - 停止配置。

- 参数:
  - 无。

- 返回值:
  - 无。

### wifi_prov_get_method_id

`uint32_t wifi_prov_get_method_id(char *method_name);`

- 功能描述:
  - 按照名称获取配置方法id。

- 参数:
  - `method_name`: 配置方法名称。

- 返回值:
  - method_id: 成功。
  - 0: 失败。

### wifi_prov_get_stauts

`wifi_prov_status_t wifi_prov_get_stauts();`

- 功能描述:
  - 获取配置方法状态。

- 参数:
  - 无。

- 返回值:
  - 配置状态。

## 示例

```c
static void wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    switch (event) {
        case WIFI_PROV_EVENT_TIMEOUT:
            /* timeout */
            break;
        case WIFI_RPOV_EVENT_GOT_RESULT:
            /* we got the result */
            break;
    }
}

void main()
{
    wifi_prov_softap_register("YoC");

    wifi_prov_start(wifi_prov_get_method_id("softap"), wifi_pair_callback, 120);
}

```

## 诊断错误码

无。

## 运行资源

- csi

- aos

- lwip

## 依赖资源

无。

## 组件参考

无。

