/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <aos/version.h>
#include <aos/kernel.h>

#include "iot_import.h"
//#include "iotx_hal_internal.h"
#include <csi_kernel.h>
#include <drv/tee.h>

static char _product_key[PRODUCT_KEY_LEN + 1];
static char _product_secret[PRODUCT_SECRET_LEN + 1];
static char _device_name[DEVICE_NAME_LEN + 1];
static char _device_secret[DEVICE_SECRET_LEN + 1];


/*
 * This need to be same with app version as in uOTA module (ota_version.h)

    #ifndef SYSINFO_APP_VERSION
    #define SYSINFO_APP_VERSION "app-1.0.0-20180101.1000"
    #endif
 *
 */

int HAL_GetFirmwareVesion(_OU_ char *version)
{
    memset(version, 0x0, FIRMWARE_VERSION_MAXLEN);

    strcpy(version, aos_get_os_version());

    return strlen(version);
}


int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    #if 0
    if (!kvfile) {
        kvfile = kv_open("/tmp/kvfile.db");
        if (!kvfile) {
            return -1;
        }
    }

    return kv_set_blob(kvfile, (char *)key, (char *)val, len);
    #endif
    return -1;
}

int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len)
{
    #if 0
    if (!kvfile) {
        kvfile = kv_open("/tmp/kvfile.db");
        if (!kvfile) {
            return -1;
        }
    }

    return kv_get_blob(kvfile, (char *)key, buffer, buffer_len);
    #endif
    return -1;
}

int HAL_Kv_Del(const char *key)
{
    #if 0
    if (!kvfile) {
        kvfile = kv_open("/tmp/kvfile.db");
        if (!kvfile) {
            return -1;
        }
    }

    return kv_del(kvfile, (char *)key);
    #endif
    return -1;
}


void *HAL_MutexCreate(void)
{
    return csi_kernel_mutex_new();
}

void HAL_MutexDestroy(void *mutex)
{
    csi_kernel_mutex_del(mutex);
}

void HAL_MutexLock(void *mutex)
{
    csi_kernel_mutex_lock(mutex, -1);
}

void HAL_MutexUnlock(void *mutex)
{
    csi_kernel_mutex_unlock(mutex);
}

void *HAL_Malloc(uint32_t size)
{
    return aos_malloc(size);
}

void HAL_Free(void *ptr)
{
    aos_free(ptr);
}

uint64_t HAL_UptimeMs(void)
{
    struct timespec tv = { 0 };
    uint64_t time_ms;

    clock_gettime(CLOCK_MONOTONIC, &tv);

    time_ms = tv.tv_sec * 1000 + tv.tv_nsec / 1000000;

    return time_ms;
}

void HAL_SleepMs(uint32_t ms)
{
    usleep(1000 * ms);
}

int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

void HAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int HAL_GetPartnerID(_OU_ char pid_str[PID_STRLEN_MAX])
{
    return 0;
}

#if 0
char *HAL_GetChipID(char *cid_str)
{
    memset(cid_str, 0x0, HAL_CID_LEN);
#ifdef __DEMO__
    strncpy(cid_str, "rtl8188eu 12345678", HAL_CID_LEN);
    cid_str[HAL_CID_LEN - 1] = '\0';
#endif
    return cid_str;
}
#endif

int HAL_GetDeviceID(char *device_id)
{
    memset(device_id, 0x0, DEVICE_ID_LEN);

    HAL_Snprintf(device_id, DEVICE_ID_LEN, "%s.%s", _product_key, _device_name);
    device_id[DEVICE_ID_LEN - 1] = '\0';

    return strlen(device_id);
}

int HAL_SetProductKey(char *product_key)
{
    int len = strlen(product_key);

    if (len > PRODUCT_KEY_LEN) {
        return -1;
    }
    memset(_product_key, 0x0, PRODUCT_KEY_LEN + 1);
    strcpy(_product_key, product_key);

    return len;
}


int HAL_SetDeviceName(char *device_name)
{
    int len = strlen(device_name);

    if (len > DEVICE_NAME_LEN) {
        return -1;
    }
    memset(_device_name, 0x0, DEVICE_NAME_LEN + 1);
    strcpy(_device_name, device_name);

    return len;
}


int HAL_SetDeviceSecret(char *device_secret)
{
    int len = strlen(device_secret);

    if (len > DEVICE_SECRET_LEN) {
        return -1;
    }
    memset(_device_secret, 0x0, DEVICE_SECRET_LEN + 1);
    strcpy(_device_secret, device_secret);

    return len;
}


int HAL_SetProductSecret(char *product_secret)
{
    int len = strlen(product_secret);

    if (len > PRODUCT_SECRET_LEN) {
        return -1;
    }
    memset(_product_secret, 0x0, PRODUCT_SECRET_LEN + 1);
    strcpy(_product_secret, product_secret);

    return len;
}

int HAL_GetProductKey(char *product_key)
{
    memset(product_key, 0x0, PRODUCT_KEY_LEN);

    strcpy(product_key, _product_key);

    return strlen(product_key);
}

int HAL_GetProductSecret( char *product_secret)
{
    memset(product_secret, 0x0, PRODUCT_SECRET_LEN);

    strcpy(product_secret, _product_secret);

    return strlen(product_secret);
}

int HAL_GetDeviceName(_OU_ char *device_name)
{
    memset(device_name, 0x0, DEVICE_NAME_LEN);

    strcpy(device_name, _device_name);

    return strlen(device_name);
}

int HAL_GetDeviceSecret(_OU_ char *device_secret)
{
    memset(device_secret, 0x0, DEVICE_SECRET_LEN);

    strcpy(device_secret, _device_secret);

    return strlen(device_secret);
}

char *HAL_GetTimeStr(char *buf, int len)
{
    struct timeval tv;
    struct tm      tm;
    int str_len    = 0;

    if (buf == NULL || len < 28) {
        return NULL;
    }
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(buf, 28, "%m-%d %H:%M:%S", &tm);
    str_len = strlen(buf);
    if (str_len + 3 < len) {
        snprintf(buf + str_len, len, ".%3.3d", (int)(tv.tv_usec) / 1000);
    }
    return buf;
}

void HAL_Srandom(uint32_t seed)
{
    return;
}

uint32_t HAL_Random(uint32_t region)
{
    uint32_t output;

#if defined(CONFIG_TEE_CA)
    csi_tee_rand_generate((uint8_t*)&output, 4);
#else
    output= rand();
#endif

    return (region > 0) ? (output % region) : 0;
}

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

int HAL_GetModuleID(char *mid_str)
{
    return 0;
}

int HAL_GetNetifInfo(char *nif_str)
{
    memset(nif_str, 0x0, NIF_STRLEN_MAX);
#ifdef __DEMO__
    /* if the device have only WIFI, then list as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    const char *net_info = "WiFi|03ACDEFF0032";
    strncpy(nif_str, net_info, strlen(net_info));
    /* if the device have ETH, WIFI, GSM connections, then list all of them as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    // const char *multi_net_info = "ETH|0123456789abcde|WiFi|03ACDEFF0032|Cellular|imei_0123456789abcde|iccid_0123456789abcdef01234|imsi_0123456789abcde|msisdn_86123456789ab");
    // strncpy(nif_str, multi_net_info, strlen(multi_net_info));
#endif
    return strlen(nif_str);
}
