# 网络时间协议（ntp）

## 概述

NTP(Network Time Protocol) 是网络时间协议，它是用来同步网络中各个计算机时间的协议。

## 组件安装

```bash
yoc init
yoc install ntp
```

## 配置
无。

## 接口列表

ntp接口如下所示：

| 函数 | 说明 |
| :--- | :--- |
| ntp_sync_time | 时间同步 |

## 接口详细说明

### ntp_sync_time
`int ntp_sync_time(char *server);`

- 功能描述:
   - 时间同步。

- 参数:
   - `server`: 时间服务器,若传入NULL，则默认使用ntp1.aliyun.com。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

## 示例

```C
ntp_sync_time(NULL);
```

或者

```C
ntp_sync_time("ntp1.aliyun.com");
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
