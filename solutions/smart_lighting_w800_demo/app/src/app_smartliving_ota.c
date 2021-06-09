/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include <aos/kernel.h>
#include <aos/debug.h>
#include <ulog/ulog.h>
#include <k_api.h>
#include "platform/wm_fwup.h"
#include "app_main.h"

#include "smartliving/exports/iot_export_errno.h"
#include "smartliving/exports/iot_export_ota.h"

#define TAG "SLOTA"

extern int dm_ota_get_ota_handle(void **handle);

tls_fwup_status_callback fw_finish_callback(int err)
{
    printf("%s: fw download finished, start to reboot...\r\n", __FUNCTION__);
    aos_reboot();

    return NULL;
}


unsigned int session_id = 0;
void HAL_Firmware_Persistence_Start(void)
{
	printf("%s, %d\r\n", __FUNCTION__, __LINE__);
    session_id = tls_fwup_enter(TLS_FWUP_IMAGE_SRC_WEB);

    tls_fwup_reg_status_callback((tls_fwup_status_callback)fw_finish_callback);
#if 1
        ktask_t* h = NULL;
        int ret = 0;
#ifdef WIFI_PROVISION_ENABLED
           extern int awss_suc_notify_stop(void);
           awss_suc_notify_stop();
#endif
#ifdef DEV_BIND_ENABLED
        extern int awss_dev_bind_notify_stop(void);
        awss_dev_bind_notify_stop();
#endif
        h = krhino_task_find("CoAPServer_yield");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }
        
        h = krhino_task_find("netmgr");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }
        
        h = krhino_task_find("b-press");
        if(h) {
            ret = krhino_task_dyn_del(h);
            aos_msleep(500);
            printf("%s, ret %d\r\n", __FUNCTION__, ret);
        }
        
        h = krhino_task_find("AWSS_TIMER");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("at&cli");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("fota");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("fota-check");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("app_task");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("at_svr");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("select");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("event_svr");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("uart_task");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }

        h = krhino_task_find("breeze_rx");
        if(h) {
            krhino_task_dyn_del(h);
            aos_msleep(500);
        }
#endif
}

int HAL_Firmware_Persistence_Stop(void)
{
    printf("%s: ota fail, start to reboot...\r\n", __FUNCTION__);
    tls_fwup_exit(session_id);

    aos_reboot();

    return SUCCESS_RETURN;
}

int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length)
{
    int ret = 0;
    ret = tls_fwup_request_sync(session_id, buffer, length);

    return ret;
}

int app_smartliving_fota_version_rpt(void)
{
    void *ota_handle = NULL;
    int res = 0;
    /* Get Ota Handle */
    res = dm_ota_get_ota_handle(&ota_handle);
    if (res != SUCCESS_RETURN) {
        printf("%s, %d, res %d\r\n", __FUNCTION__, __LINE__, res);
        return FAIL_RETURN;
    }
    
    IOT_OTA_ReportVersion(ota_handle, aos_get_app_version());

    return SUCCESS_RETURN;
}


