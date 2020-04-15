/******************************************************************************
 * Copyright (c) 2013-2016 Realtek Semiconductor Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include "yoc_config.h" /* out/config/yoc_config.h */
#include <aos/aos.h>
#include <sdmmc_host.h>
#include <sdmmc_spec.h>
#include "osdep_service.h"
#include "wifi_io.h"
#include "soc.h"
#include "sdio.h"

#include <device_lock.h>

#define DEBUG_SDIO 0

/* test wifi driver */
#define ADDR_MASK 0x10000
#define LOCAL_ADDR_MASK 0x00000

#define dbg_host printf

static sdio_card_t SDIO_Card;
struct sdio_func *wifi_sdio_func = NULL;
extern struct osdep_service_ops osdep_service;
static const char* TAG = "WIFI_IO";

void SDMMCHOST_Disable_Interrupt(int idx);
void SDMMCHOST_RegisterInterrupt(sdmmchost_interrupt_t interrupt_func);
void SDMMCHOST_Enable_Interrupt(int idx);
void rtl8723ds_set_invalid();

#if 0
static void sdio_inserted_callback(bool isInserted, void *userData)
{
    LOGD(TAG, "SDIO Card Inserted Callback!");
}

static void sdio_removed_callback(bool isInserted, void *userData)
{
    LOGD(TAG, "SDIO Card Removed Callback!");
}
#endif

typedef void(*rtl8723ds_irq_handler)(struct sdio_func *);
static struct sdio_func *func_data;
static rtl8723ds_irq_handler irq_handler;
static _mutex claim_host_mutex;
static int sdio_thread_running = 0;
int sdio_thread_exit = 1;
// _sema irq_sema;
aos_event_t  g_wifi_irq_event;
#ifndef CONFIG_CHIP_YUNVOICE_CPU0
static int g_idx = 0;
#endif
//#undef CONFIG_CHIP_YUNVOICE_CPU0


void rtl8723ds_irq_callback(int idx, void *user_data)
{
#ifndef CONFIG_CHIP_YUNVOICE_CPU0
    //printf("#");
    g_idx = idx;
    SDMMCHOST_Disable_Interrupt(idx);
    aos_event_set(&g_wifi_irq_event, 1, AOS_EVENT_OR);
    // osdep_service.rtw_up_sema_from_isr(&irq_sema);
#endif

}



void sdio_irq_thread(void *arg)
{
    sdio_thread_exit = 0;

#ifdef CONFIG_CHIP_YUNVOICE_CPU0
    LOGD(TAG, "sdio_irq_thread enter the LOOP routine");
    while (sdio_thread_running) {
        device_mutex_lock(RT_DEV_LOCK_WLAN);
        osdep_service.rtw_mutex_get(&claim_host_mutex);
        if (irq_handler)
            irq_handler(func_data);
        osdep_service.rtw_mutex_put(&claim_host_mutex);
        device_mutex_unlock(RT_DEV_LOCK_WLAN);
        aos_msleep(10);
    }
#else
    LOGD(TAG, "sdio_irq_thread enter IRQ routine");

    SDMMCHOST_RegisterInterrupt(rtl8723ds_irq_callback);
    while (sdio_thread_running) {
        uint32_t act_flags;
        // if (osdep_service.rtw_down_timeout_sema(&irq_sema, -1) == _SUCCESS) {
        if (aos_event_get(&g_wifi_irq_event, 1, AOS_EVENT_OR_CLEAR, &act_flags, -1) == 0) {

            if (sdio_thread_running) {
                device_mutex_lock(RT_DEV_LOCK_WLAN);
                osdep_service.rtw_mutex_get(&claim_host_mutex);
                if (irq_handler) {
                    irq_handler(func_data);
                }
                SDMMCHOST_Enable_Interrupt(g_idx);
                osdep_service.rtw_mutex_put(&claim_host_mutex);
                device_mutex_unlock(RT_DEV_LOCK_WLAN);

            }
        } else {
            //printf("sdio_irq_thread down sema failed");
            //aos_msleep(10);
        }
    }
#endif
    sdio_thread_exit = 1;
    LOGD(TAG, "sdio_irq_thread exit\n");
}

void print_buf(char *buf, int len)
{
    int j = 0;

    for (j = 0; j < len; j++) {
        if (0 == (j & (16 - 1))) { //print 16 hex every line
            printf("\n%06x ", j);
        }

        printf(" %02x", buf[j] & 0xff);
    }

    printf("\n");

}


/**
    Register SDIO interrupt function
*/
int sdio_claim_irq(struct sdio_func *func, void(*handler)(struct sdio_func *))
{
    //LOGD(TAG, "%s", __FUNCTION__);
    irq_handler = handler;
    func_data = func;

    aos_task_t task_handle;
    sdio_thread_running = 1;
    if (0 != aos_task_new_ext(&task_handle, "sdio_irq", sdio_irq_thread, NULL, 1024 * 4, AOS_DEFAULT_APP_PRI + 4)) {
        LOGE("SDIO", "Create sdio_irq_thread task failed.");
    }

    return 0;
}

/**
    Release SDIO interrupt function
*/
int sdio_release_irq(struct sdio_func *func)
{
    LOGD(TAG, "%s", __FUNCTION__);

    SDMMCHOST_RegisterInterrupt(NULL);
    irq_handler = NULL;
    sdio_thread_running = 0;
#ifndef CONFIG_CHIP_YUNVOICE_CPU0
    // osdep_service.rtw_up_sema_from_isr(&irq_sema);
    aos_event_set(&g_wifi_irq_event, 1, AOS_EVENT_OR);
#endif

    return 0;
}

#if 0
/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t sdio_CardDetect = {
    .cdType = kSDMMCHOST_DetectCardByHostCD,
    .cdTimeOut_ms = (~0U),
    .card_inserted = sdio_inserted_callback,
    .card_removed = sdio_removed_callback,
};
#endif

int sdio_card_reset()
{
    SDIO_CardReset(&SDIO_Card);
    return 0;
}

int sdio_bus_probe()
{
#ifndef SDMMC_BASE_ADDR
#define SDMMC_BASE_ADDR CSKY_SDIO0_BASE
#endif
    LOGD(TAG, "%s", __FUNCTION__);
    memset(&SDIO_Card, 0, sizeof(SDIO_Card));
    SDIO_Card.usrParam.cd = NULL;
    SDIO_Card.host.base  = (sdif_handle_t)SDMMC_BASE_ADDR;

    int ret = 0;
    if (kStatus_Success != (ret= SDIO_Init(&SDIO_Card))) {
        LOGE(TAG, "SDIO Init failed ret=%d", ret);
        rtl8723ds_set_invalid();
        return -1;
    } else {
        LOGD(TAG, "SDIO Init success");
        aos_msleep(100); /** wait card reset */
    }

    // FIXME: only for FPGA
    //set force to 1 bit
// #ifdef CONFIG_CHIP_PANGU_CPU0
//     if (kStatus_Success != SDIO_SetDataBusWidth(&SDIO_Card, kSDIO_DataBus1Bit)) {
//         printf("!!!!!!!SDIO_SetDataBusWidth error\n");
//         return kStatus_SDMMC_SetDataBusWidthFailed;
//     }
// #endif

    //set force to 512
    if (SDIO_SetBlockSize(&SDIO_Card, kSDIO_FunctionNum1, 512) != kStatus_Success) {
        LOGE(TAG, "SDIO_SetBlockSize 512 error");
        rtl8723ds_set_invalid();
        return  1;
    } else {
        LOGD(TAG, "SDIO_SetBlockSize 512 success");
    }

    wifi_sdio_func = aos_zalloc(sizeof(struct sdio_func)); //TBD here
    wifi_sdio_func->num = 1;
    aos_event_new(&g_wifi_irq_event, 0);
    // osdep_service.rtw_init_sema(&irq_sema, 0);
    osdep_service.rtw_mutex_init(&claim_host_mutex);

    //extern int rtw_fake_driver_probe(struct sdio_func *func);
    //rtw_fake_driver_probe(wifi_sdio_func);

    return 0;
}


int sdio_bus_remove()
{
    LOGD(TAG, "%s", __FUNCTION__);
    aos_free(wifi_sdio_func);

    SDIO_Deinit(&SDIO_Card);

    return 0;
}


/**
    Get the SDIO privilege before IO
*/
void sdio_claim_host(struct sdio_func *func)
{
    osdep_service.rtw_mutex_get(&claim_host_mutex);
    return;
}

/**
    Release the SDIO privilege after IO
*/
void sdio_release_host(struct sdio_func *func)
{
    osdep_service.rtw_mutex_put(&claim_host_mutex);
    return;
}



static int sdio_enable_func(struct sdio_func *func)
{
    LOGD(TAG, "Enable SDIO Func %d", func->num);

    if (func->num == 1) {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    SDIO_EnableIO(&SDIO_Card, func->num, 1);

    return 0;
}


int sdio_disable_func(struct sdio_func *func)
{
    LOGD(TAG, "Disable SDIO Func %d", func->num);

    if (func->num == 1) {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    SDIO_EnableIO(&SDIO_Card, func->num, 0);
    return 0;
}

static int32_t sdio_io_rw_extended_block(struct sdio_func *func,
        int32_t               rw,
        uint32_t              addr,
        int32_t               op_code,
        uint8_t              *buf,
        uint32_t              len)
{
    uint32_t flags = SDIO_EXTEND_CMD_BLOCK_MODE_MASK;
    uint32_t block_size;

    if (func->num == 1) {
        block_size = SDIO_Card.ioFBR[0].ioBlockSize;
//        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        block_size = SDIO_Card.io0block_size;
//        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    if (op_code == 1) {
        flags |= SDIO_EXTEND_CMD_OP_CODE_MASK;
    }

    status_t ret;

    if (rw == 0) {
        ret = SDIO_IO_Read_Extended(&SDIO_Card, func->num, addr, buf, len / block_size, flags);
#if DEBUG_SDIO
        printf("[Bytes addr=%x len=%d][R]\n", addr, len);
        print_buf(buf, len);
#endif
    } else {

#if DEBUG_SDIO
        printf("[Bytes addr=%x len=%d][W]\n", addr,  len);
        print_buf(buf, len);
#endif
        ret = SDIO_IO_Write_Extended(&SDIO_Card, func->num, addr, buf, len / block_size, flags);
    }

    if (ret != kStatus_Success) {
        LOGE(TAG, "sdio_io_rw_extended_block error rw=%d,ret=%d", rw, ret);
        return 1;
    }

    return 0;
}



static int32_t sdio_io_rw_extended_byte(struct sdio_func *func,
                                        int32_t               rw,
                                        uint32_t              addr,
                                        int32_t               op_code,
                                        uint8_t              *buf,
                                        uint32_t              len)
{
    uint32_t flags = 0;//SDIO_EXTEND_CMD_BLOCK_MODE_MASK;

    // if (func->num == 1) {
    //     SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    // } else {
    //     SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    // }

    if (op_code == 1) {
        flags |= SDIO_EXTEND_CMD_OP_CODE_MASK;
    }

    status_t ret;

    if (rw == 0) {
        ret = SDIO_IO_Read_Extended(&SDIO_Card, func->num, addr, buf, len, flags);
    } else {
        ret = SDIO_IO_Write_Extended(&SDIO_Card, func->num, addr, buf, len, flags);
    }

    if (ret != kStatus_Success) {
        LOGE(TAG, "sdio_io_rw_extended_byte error rw=%d,ret=%d", rw, ret);
        return 1;
    }

    return 0;
}


//size < 512, using byte mode, others using block mode
int sdio_memcpy_fromio(struct sdio_func *func, void *dst, unsigned int addr, int count)
{
    if (count >= 512) {
        if (count % 512 != 0)
            LOGE(TAG, "warning sdio rx need byte mode transfer %d bytes", count % 512);
        return sdio_io_rw_extended_block(func, 0, addr, 1, dst, count);
    } else {
        return sdio_io_rw_extended_byte(func, 0, addr, 1, dst, count);
    }
}

int sdio_memcpy_toio(struct sdio_func *func, unsigned int addr, void *src, int count)
{
    int ret = 0;

    if (count >= 512) {
        if (count % 512 != 0)
            LOGE(TAG, "warning sdio tx need byte mode transfer %d bytes", count % 512);
        ret = sdio_io_rw_extended_block(func, 1, addr, 1, src, count);
    } else {
        ret = sdio_io_rw_extended_byte(func, 1, addr, 1, src, count);
    }

    return ret;
}

uint8_t g_tmp_buf[4];
u8 sdio_readb(struct sdio_func *func, unsigned int addr, int *err_ret)
{
    status_t ret;

    // if (func->num == 1) {
    //     SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    // } else {

    //     SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    // }

    ret = SDIO_IO_Read_Direct(&SDIO_Card, func->num, addr, g_tmp_buf);

    if (ret != kStatus_Success) {
        LOGE(TAG, "sdio_io_readb error");

        if (err_ret) {
            *err_ret = -1;
        }

        return *(u8 *)g_tmp_buf;
    }

    if (err_ret) {
        *err_ret = 0;
    }

#if DEBUG_SDIO
    printf("[U8][R][%x=%02x]\n", addr, *(u8 *)g_tmp_buf);
#endif

    return *(u8 *)g_tmp_buf;
}

u16 sdio_readw(struct sdio_func *func, unsigned int addr, int *err_ret)
{
    int ret;

    if (err_ret) {
        *err_ret = 0;
    }

    ret = sdio_memcpy_fromio(func, g_tmp_buf, addr, 2);

    if (ret) {
        if (err_ret) {
            *err_ret = ret;
        }
    }

#if DEBUG_SDIO
    printf("[U16][R][%x=%04x]\n", addr, *(u16 *)g_tmp_buf);
#endif
    uint16_t *res = (uint16_t*)g_tmp_buf;

    return *res;
}


u32 sdio_readl(struct sdio_func *func, unsigned int addr, int *err_ret)
{
    int ret;

    if (err_ret) {
        *err_ret = 0;
    }

    ret = sdio_memcpy_fromio(func, g_tmp_buf, addr, 4);

    if (ret) {
        if (err_ret) {
            *err_ret = ret;
        }
    }

#if DEBUG_SDIO
    printf("[U32][R][%x=%08x]\n", addr, *(u32 *)g_tmp_buf);
#endif
    uint32_t *res = (uint32_t*)g_tmp_buf;

    return *res;
}


void sdio_writeb(struct sdio_func *func, u8 b, unsigned int addr, int *err_ret)
{
    uint8_t data_tmp = b;
    status_t ret;

#if DEBUG_SDIO
    printf("[U8][W][%x=%02x]\n", addr, *(u8 *)g_tmp_buf);
#endif

    // if (func->num == 1) {
    //     SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    // } else {
    //     SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    // }

    ret = SDIO_IO_Write_Direct(&SDIO_Card, func->num, addr, &data_tmp, 0);

    if (err_ret) {
        *err_ret = 0;
    }

    if (ret != kStatus_Success) {
        if (err_ret) {
            *err_ret = 1;
        }

        LOGE(TAG, "sdio_io_writeb error reg 0x%x", addr);
        return;
    }
}

void sdio_writew(struct sdio_func *func, u16 b, unsigned int addr, int *err_ret)
{
    int ret;

    if (err_ret) {
        *err_ret = 0;
    }

#if DEBUG_SDIO
    printf("[U16][W][%x=%04x]\n", addr, *(u16 *)g_tmp_buf);
#endif
    uint16_t *res = (uint16_t*)g_tmp_buf;

    *res = b;

    ret = sdio_memcpy_toio(func, addr, res, 2);

    if (ret) {
        if (err_ret) {
            *err_ret = ret;
        }

        return;
    }

    return;

}

void sdio_writel(struct sdio_func *func, u32 b, unsigned addr, int *err_ret)
{

    int ret;

    if (err_ret) {
        *err_ret = 0;
    }

#if DEBUG_SDIO
    printf("[U32][W][%x=%08x]\n", addr, *(u32 *)g_tmp_buf);
#endif
    uint32_t *res = (uint32_t*)g_tmp_buf;

    *res = b;

    ret = sdio_memcpy_toio(func, addr, res, 4);

    if (ret) {
        if (err_ret) {
            *err_ret = ret;
        }

        return;
    }

    return;
}

/////////////////////////////// Porting code end //////////////////////////////////

int wifi_read(struct sdio_func *func, u32 addr, u32 cnt, void *pdata)
{
    int err;

    sdio_claim_host(func);
    err = sdio_memcpy_fromio(func, pdata, addr, cnt);

    if (err) {
        LOGE(TAG, "%s: FAIL(%d)! ADDR=%#x Size=%d", __func__, err, addr, cnt);
    }

    sdio_release_host(func);


    return err;
}

int wifi_write(struct sdio_func *func, u32 addr, u32 cnt, void *pdata)
{
    int err;
    u32 size;

    sdio_claim_host(func);
    size = cnt;
    err = sdio_memcpy_toio(func, addr, pdata, size);

    if (err) {
        LOGE(TAG, "%s: FAIL(%d)! ADDR=%#x Size=%d(%d)", __func__, err, addr, cnt, size);
    }

    sdio_release_host(func);

    return err;
}

u8 wifi_readb(struct sdio_func *func, u32 addr)
{
    int err;
    u8 ret = 0;

    sdio_claim_host(func);
    ret = sdio_readb(func, ADDR_MASK | addr, &err);
    sdio_release_host(func);

    if (err) {
        LOGE(TAG, "%s: FAIL!(%d) addr=0x%05x", __func__, err, addr);
    }

    return ret;
}

u16 wifi_readw(struct sdio_func *func, u32 addr)
{
    int err;
    u16 v;

    sdio_claim_host(func);
    v = sdio_readw(func, ADDR_MASK | addr, &err);
    sdio_release_host(func);

    if (err) {
        LOGE(TAG, "%s: FAIL!(%d) addr=0x%05x", __func__, err, addr);
    }

    return  v;
}

u32 wifi_readl(struct sdio_func *func, u32 addr)
{
    int err;
    u32 v;

    sdio_claim_host(func);
    v = sdio_readl(func, ADDR_MASK | addr, &err);
    sdio_release_host(func);

    return  v;
}

void wifi_writeb(struct sdio_func *func, u32 addr, u8 val)
{
    int err;
    sdio_claim_host(func);
    sdio_writeb(func, val, ADDR_MASK | addr, &err);
    sdio_release_host(func);
}

void wifi_writew(struct sdio_func *func, u32 addr, u16 v)
{
    int err;
    sdio_claim_host(func);
    sdio_writew(func, v, ADDR_MASK | addr, &err);
    sdio_release_host(func);
}

void wifi_writel(struct sdio_func *func, u32 addr, u32 v)
{
    int err;

    sdio_claim_host(func);
    sdio_writel(func, v, ADDR_MASK | addr, &err);
    sdio_release_host(func);
}

u8 wifi_readb_local(struct sdio_func *func, u32 addr)
{
    int err;
    u8 ret = 0;
    ret = sdio_readb(func, LOCAL_ADDR_MASK | addr, &err);

    return ret;
}

void wifi_writeb_local(struct sdio_func *func, u32 addr, u8 val)
{
    int err;
    sdio_writeb(func, val, LOCAL_ADDR_MASK | addr, &err);
}


SDIO_BUS_OPS rtw_sdio_bus_ops = {
    sdio_bus_probe,
    sdio_bus_remove,
    sdio_enable_func,
    sdio_disable_func,
    NULL,
    NULL,
    sdio_claim_irq,
    sdio_release_irq,
    sdio_claim_host,
    sdio_release_host,
    sdio_readb,
    sdio_readw,
    sdio_readl,
    sdio_writeb,
    sdio_writew,
    sdio_writel,
    sdio_memcpy_fromio,
    sdio_memcpy_toio
};

