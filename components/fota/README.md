# 概述

YoC定义了一个FOTA任务用于维护FOTA新版本检测、状态机转换维护和下载控制，抽象了netio用于固件包的下载保存。即：
1. 新版本检测 
2. 新版本固件包(差分、全量)下载

# FOTA流程与状态

FOTA模块设计了4个状态转化：

- FOTA_INIT
- FOTA_DOWNLOAD
- FOTA_STOP
- FOTA_FINISH

用户启动FOTA后状态为FOTA_INIT，当检测到新版本时，转入FOTA_DOWNLOAD状态，并开始下载固件包。下载失败之后会重新下载或者复位下载流程，取决于重试次数(retry count)。
用户可以通过自定义的方式(添加fota_event_cb回调函数)来根据FOTA在各个阶段报出的事件来执行相应的动作(fota event层)，事件回调函数返回值代表不同的含义：

- 0：处理完用户事件后继续原先的默认流程；
- 非零：处理完用户事件后不会继续原先的默认流程，此时用户需要定义自己的流程去向

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

## FOTA 平台对接

FOTA框架支持接入不同的FOTA云平台。如：COP、COAP
不同的FOTA平台需要通过对接fota_cls，实现该结构体的操作。

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

## FOTA 下载

FOTA框架对FOTA下载和存放进行了抽象，形成netio 类，用于可通过netio类来对接不同的下载和存放方式，例如对接ftp下载方式，对接新的 uart 存放方式等。
通过实现结构体 netio_cls 来对接

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

用于需要实现 netio_cls 接口的 open、close、read、write、remove、seek接口，并通过`netio_register`接口注册进FOTA。

## FOTA 升级

固件下载完毕之后会设备会重启进入固件升级模式，等待升级完毕之后设备正常启动。一个完整的FOTA流程结束。


# 示例

```
本示例以cop平台的FOTA流程为例，展示在solution的app_main中，FOTA模块的初始化过程：
```

```c
    fota_t *g_fota_handle;
    fota_register_cop();
    netio_register_http();
    netio_register_flash();
    g_fota_handle = fota_open("cop", "flash://misc", fota_event_cb);
    g_fota_handle->sleep_time = 60000;
    g_fota_handle->timeoutms = 10000;
    g_fota_handle->retry_count = 0;
    fota_start(g_fota_handle);
```