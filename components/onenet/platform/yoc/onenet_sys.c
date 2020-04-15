/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <yoc_config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <aos/aos.h>

#include <yoc/nvram.h>
#include <yoc/sysinfo.h>

#include <cis_if_sys.h>
#include <cis_def.h>
#include <cis_config.h>
#include <cis_log.h>
#include <time.h>
#include <drv/tee.h>

/* TODO: remove static imei, imsi */
static char imei[20] = {0};
static char imsi[20] = {0};

cis_ret_t cissys_init(const cis_cfg_sys_t *cfg)
{
    const char *tmp_imei = aos_get_imei();
    const char *tmp_imsi = aos_get_imsi();

    if (tmp_imei== NULL || tmp_imsi == NULL) {
        LOGE("onetsys", "set imei & imsi first");    
        return CIS_RET_ERROR;
    }

    strncpy(imei, tmp_imei, sizeof(imei) - 1);
    strncpy(imsi, tmp_imsi, sizeof(imsi) - 1);

    LOGD("onetsys", "imei = %s", imei);
    LOGD("onetsys", "imsi = %s", imsi);

    return CIS_RET_OK;
}
uint32_t cissys_gettime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void cissys_sleepms(uint32_t ms)
{
    usleep(ms * 1000);
}

void cissys_logwrite(uint8_t *buffer, uint32_t length)
{
    CIS_LOGD("%s", buffer);
}

void cissys_assert(bool flag)
{
    aos_check(flag, EINVAL);
/*
    if (!flag) {
        CIS_LOGD("cissys_assert");
        while(1){};
    }
*/
}

/* dnk for mem debug */
#if 0
void *cissys_malloc(size_t length)
{
    return malloc(length);
}
void cissys_free(void *buffer)
{
    free(buffer);
}
#endif

void cissys_lockcreate(void** mutex)
{
    aos_mutex_t aos_mutex;
    aos_mutex_new(&aos_mutex);
    *mutex = aos_mutex.hdl;
}

void cissys_lockdestory(void* mutex)
{
    aos_mutex_t aos_mutex = { mutex };
    aos_mutex_free(&aos_mutex);
}

cis_ret_t cissys_lock(void* mutex,uint32_t ms)
{
    aos_mutex_t aos_mutex = { mutex };
    return aos_mutex_lock(&aos_mutex, ms);
}

void cissys_unlock(void* mutex)
{
    aos_mutex_t aos_mutex = { mutex };
    aos_mutex_unlock(&aos_mutex);
}

void *cissys_memset(void *s, int c, size_t n)
{
    return memset(s, c, n);
}
void *cissys_memcpy(void *dst, const void *src, size_t n)
{
    return memcpy(dst, src, n);
}
void *cissys_memmove(void *dst, const void *src, size_t n)
{
    return memmove(dst, src, n);
}
int cissys_memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}
void cissys_fault(uint16_t id)
{
    CIS_LOGE("cissys_fault");
}
uint32_t cissys_rand()
{
    return rand();
}
uint8_t cissys_getIMEI(uint8_t *buffer, uint32_t maxlen)
{
    cissys_memcpy(buffer, imei, strlen(imei));
    return strlen(imei);
}
uint8_t cissys_getIMSI(uint8_t *buffer, uint32_t maxlen)
{
    cissys_memcpy(buffer, imsi, strlen(imsi));
    return strlen(imsi);
}

bool cissys_save(uint8_t* buffer,uint32_t length)
{
    return false;
}

bool cissys_load(uint8_t* buffer,uint32_t length)
{
    return false;
}

