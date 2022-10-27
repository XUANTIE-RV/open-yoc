# 简单网络时间协议（sntp）

## 概述

简单网络时间协议（Simple Network Time Protocol），由 NTP 改编而来，采用UDP方式，主要用来同步因特网中的计算机时钟。

## 组件安装

```bash
yoc init
yoc install sntp
```

## 配置
无。

## 接口列表

sntp接口如下所示：

| 函数 | 说明 |
| :--- | :--- |
| sntp_sync_start | 时间同步 |
| sntp_add_server | 添加指定的服务器 |
| sntp_set_op_mode | 设置同步模式 |
| sntp_set_time_sync_notification_cb | 设置同步的回调函数 |
| sntp_sync_stop | 停止同步 |

## 接口详细说明

### sntp_sync_start
`void sntp_sync_start(void);`

- 功能描述:
   - 时间同步。

- 参数:
   - 无。

- 返回值:
   - 无。

### sntp_add_server
`void sntp_add_server(char *server)`

- 功能描述:
   - 添加指定的服务器。

- 参数:
   - `server`: 时间服务器。

- 返回值:
   - 无。

### sntp_set_op_mode
`void sntp_set_op_mode(sntp_op_mode_t op_mode);`

- 功能描述:
   - 设置同步模式。

- 参数:
   - `op_mode`: 模式。

- 返回值:
   - 无。

#### sntp_op_mode_t
| 类型 | 说明 |
| :--- | :--- |
| SNTP_CLIENT_UNICAST | 单播 |
| SNTP_CLIENT_BROADCAST | 广播 |

### sntp_set_time_sync_notification_cb
`void sntp_set_time_sync_notification_cb(sntp_sync_time_cb_t callback);`

- 功能描述:
   - 设置同步的回调函数。

- 参数:
   - `callback`: 回调函数。

- 返回值:
   - 无。

#### sntp_sync_time_cb_t
typedef void (*sntp_sync_time_cb_t) (struct timeval *tv);

### sntp_sync_stop
`void sntp_sync_stop(void);`

- 功能描述:
   - 停止同步。

- 参数:
   - 无。

- 返回值:
   - 无。

## 示例

```C
static void callback(struct timeval *tv)
{
    printf("callback get time:%lld ms", ((int64_t)tv->tv_sec * 1000000L + (int64_t)tv->tv_usec) / 1000);
}
```

```C
void test_sntp(void)
{
    printf("test_sntp\n");
    sntp_set_time_sync_notification_cb(callback);
    sntp_sync_start();
}
```

## 诊断错误码
无。

## 运行资源
无。

## 依赖资源

  - csi
  - aos

## 组件参考
无。
