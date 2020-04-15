#include <sntp.h>
#include <stdio.h>

static void callback(struct timeval *tv)
{
    printf("callback get time:%lld ms", ((int64_t)tv->tv_sec * 1000000L + (int64_t)tv->tv_usec) / 1000);
}

void test_sntp(void)
{
    printf("test_sntp\n");
    sntp_set_time_sync_notification_cb(callback);
    sntp_sync_start();
}