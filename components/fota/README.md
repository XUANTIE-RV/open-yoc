# 空中下载软件服务（FOTA）

## 概述

YoC定义了一个FOTA任务用于维护FOTA新版本检测、状态机转换维护和下载控制，抽象了netio用于固件包的下载保存。即: 

1. 新版本检测
2. 新版本固件包(差分、全量)下载

### FOTA流程与状态

FOTA模块设计了4个状态转化: 

- FOTA_INIT

- FOTA_DOWNLOAD

- FOTA_STOP

- FOTA_FINISH

用户启动FOTA后状态为FOTA_INIT，当检测到新版本时，转入FOTA_DOWNLOAD状态，并开始下载固件包。下载失败之后会重新下载或者复位下载流程，取决于重试次数(retry count)。 用户可以通过自定义的方式(添加fota_event_cb回调函数)来根据FOTA在各个阶段报出的事件来执行相应的动作(fota event层)，事件回调函数返回值代表不同的含义: 

- 0: 处理完用户事件后继续原先的默认流程；

- 非零: 处理完用户事件后不会继续原先的默认流程，此时用户需要定义自己的流程去向。

用户定义代码示例如下:

```c
static int fota_event_cb(void *arg, fota_event_e event) //return 0: still do the default handle      not zero: only do the user handle
{
    fota_t *fota = (fota_t *)arg;
    switch (event) {
        case FOTA_EVENT_VERSION:
            LOGD(TAG, "FOTA VERSION :%x", fota->status);
            break;

        case FOTA_EVENT_START:
            LOGD(TAG, "FOTA START :%x", fota->status);
            break;

        case FOTA_EVENT_FAIL:
            LOGD(TAG, "FOTA FAIL :%x", fota->status);
            break;

        case FOTA_EVENT_FINISH:
            LOGD(TAG, "FOTA FINISH :%x", fota->status);
            break;

        default:
            break;
    }
    return 0;
}
```

### FOTA 平台对接

FOTA框架支持接入不同的FOTA云平台。如: COP、COAP 不同的FOTA平台需要通过对接fota_cls，实现该结构体的操作。

```c
typedef struct fota_cls {
const char *name;
int (*init)(void);
int (*version_check)(fota_info_t *info);
int (*finish)(void);
int (*fail)(void);
} fota_cls_t;
```

通过`int fota_register(const fota_cls_t *cls)`接口注册。

### FOTA 下载

FOTA框架对FOTA下载和存放进行了抽象，形成netio 类，用于可通过netio类来对接不同的下载和存放方式，例如对接ftp下载方式，对接新的 uart 存放方式等。 通过实现结构体 netio_cls 来对接

```c
struct netio_cls {
const char *name;
int (*open)(netio_t *io, const char *path);
int (*close)(netio_t *io);
int (*read)(netio_t *io, uint8_t *buffer, int length, int timeoutms);
int (*write)(netio_t *io, uint8_t *buffer, int length, int timeoutms);
int (*remove)(netio_t *io);
int (*seek)(netio_t *io, size_t offset, int whence);
};
```

用户需要实现 netio_cls 接口的 open、close、read、write、remove、seek接口，并通过`netio_register`接口注册进FOTA。

### FOTA 升级

固件下载完毕之后会设备会重启进入固件升级模式，等待升级完毕之后设备正常启动。一个完整的FOTA流程结束。

## 组件安装

```bash
yoc init
yoc install fota
```

## 配置

无。

## 接口列表

| 函数                | 说明                                                         |
| :-------------------| :----------------------------------------------------------- |
| fota_upgrade        | 开始下载镜像                                                 |
| fota_do_check       | 强制检测版本                                                 |
| fota_start          | 创建FOTA服务                                                 |
| fota_stop           | 停止FOTA功能，退出FOTA服务                                   |
| fota_finish         | FOTA完成，调用用户实现的finish接口                           |
| fota_fail           | FOTA失败，调用用户实现的fail接口，并释放FOTA资源,但不释放FOTA句柄 |
| fota_config         | 配置FOTA参数                                                 |
| fota_set_auto_check | 设置是否自动不断检测服务器固件版本并升级的功能               |
| fota_get_auto_check | 获取是否自动检测判断                                         |
| fota_get_status     | 获取升级状态                                                 |
| fota_open           | fota初始化                                                   |
| fota_close          | 关闭FOTA功能，释放所有资源                                   |
| fota_register_cop   | 注册为cop平台，即从cop平台下载固件                           |
| fota_register_coap  | 注册为coap平台，即从coap平台下载固件                         |
| fota_register       | 注册平台接口                                                 |



## 接口详细说明

### fota_upgrade

`int fota_upgrade(fota_t *fota);`

- 功能描述:
  - 开始下载镜像。

- 参数:
  - `fota`: fota 句柄。

- 返回值:
  - 0: 下载镜像成功。
  - 1: 下载镜像失败。

### fota_do_check

`void fota_do_check(fota_t *fota);`

- 功能描述:
  - 强制检测版本。

- 参数:
  - `fota`: fota 句柄。

- 返回值:
  - 0: 检测成功。
  - 1: 检测失败。

### fota_start

`int fota_start(fota_t *fota);`

- 功能描述:
  - 创建FOTA服务。

- 参数:
  - `fota`:fota 句柄。

- 返回值:
  - 0: 创建FOTA服务成功。
  - 1: 创建FOTA服务失败。

### fota_stop

`int fota_stop(fota_t *fota);`

- 功能描述:
  - 停止FOTA功能，退出FOTA服务。

- 参数:
  - `fota`: fota 句柄。

- 返回值:
  - 0: 成功。
  - 1: 失败。

### fota_finish

`void fota_finish(fota_t *fota);`

- 功能描述:
  - FOTA完成，调用用户实现的finish接口。

- 参数:
  - `fota`: fota 句柄。

- 返回值:无。

### fota_fail

`void fota_fail(fota_t *fota);`

- 功能描述:
  - FOTA失败，调用用户实现的fail接口，并释放FOTA资源,但不释放FOTA句柄。
  
- 参数:
  - `fota`: fota 句柄。
  
- 返回值:无。

### fota_config

`void fota_config(fota_t *fota, fota_config_t *config);`

- 功能描述:
  - 配置FOTA参数。
  
- 参数:
  - `fota`: fota 句柄。
  - `config`: 配置数据。
  
- 返回值:无

### fota_set_auto_check

`void fota_set_auto_check(fota_t *fota, int enable);`

- 功能描述:
  - 设置是否自动不断检测服务器固件版本并升级的功能。
  
- 参数:
  - `fota`: fota 句柄。
  - `enable`: 使能标记。
  
- 返回值:无。

### fota_get_auto_check

`int fota_get_auto_check(fota_t *fota);`

- 功能描述:
  -   获取是否自动检测判断。
  
- 参数:
  - `fota`: fota 句柄。
  
- 返回值:
  - 0: 表示不自动检测。
  - 1: 表示自动检测。

### fota_get_status

`fota_status_e fota_get_status(fota_t *fota);`

- 功能描述:
  -   获取升级状态。
  
- 参数:
  - `fota`: fota 句柄。
  
- 返回值:
  - fota_status_e类型。

### fota_open

`fota_t *fota_open(const char *fota_name, const char *dst, fota_event_cb_t event_cb);`

- 功能描述:
  -   fota初始化。
  
- 参数:
  - `fota_name`:FOTA平台名字，比如"cop"。
  - `dst`:差分包存储url。
  - `event_cb`:用户事件回调。
  
- 返回值:
  - 非空: fota句柄。
  - NULL: 失败。

### fota_close

`int fota_close(fota_t *fota);`

- 功能描述:
  - 关闭FOTA功能，释放所有资源。
  
- 参数:
  - `fota`:fota 句柄。
  
- 返回值:
  - 0: 关闭FOTA成功。
  - 1: 关闭FOTA失败。

### fota_register_cop

`int fota_register_cop(void);`

- 功能描述:
  - 注册为cop平台，即从cop平台下载固件。
  
- 参数:
  - 无。
  
- 返回值:
  - 0: 注册cop成功。
  - 1: 注册cop失败。

### fota_register_coap

`int fota_register_coap(void);`

- 功能描述:
  - 注册为coap平台，即从coap平台下载固件。
  
- 参数:
  - 无。
  
- 返回值:
  - 0: 注册cop成功。
  - 1: 注册cop失败。

### fota_register

`int fota_register(const fota_cls_t *cls);`

- 功能描述:
  -   注册平台接口。
  
- 参数:
  - `cls`:不同平台实现的接口集合，具体实现接口见`fota_cls_t`。
  
- 返回值:
  - 0: 注册平台接口成功。
  - 1: 注册平台接口失败。

## 示例

### FOTA初始化示例

cop平台FOTA模块的初始化过程: 

```c
void app_fota_init(void)
{
    int ret;
    LOGI(TAG, "======> %s\r\n", aos_get_app_version());
    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if (ret == 0 && fota_en == 0) {
        return;
    }

    fota_register_cop();
    netio_register_http();
    netio_register_flash();
    g_fota_handle = fota_open("cop", "flash://misc", fota_event_cb);
    g_fota_handle->auto_check_en = 1;
    g_fota_handle->sleep_time = 60000;
    g_fota_handle->timeoutms = 10000;
    g_fota_handle->retry_count = 0;
    fota_start(g_fota_handle);
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