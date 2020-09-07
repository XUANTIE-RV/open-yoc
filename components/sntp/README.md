# 概述

简单网络时间协议（Simple Network Time Protocol），由 NTP 改编而来，采用UDP方式，主要用来同步因特网中的计算机时钟。

# 示例

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
