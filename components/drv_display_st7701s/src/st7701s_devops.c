#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <aos/kernel.h>
#include <devices/device.h>
#include <drv/iic.h>
#include <drv/pin.h>
#include <pinctrl-mars.h>
#include <drv/gpio.h>
#include <devices/impl/display_impl.h>
#include <drv/pwm.h>
#include <devices/display.h>
#include "st7701s_devops.h"
#include "cvi_mipi_tx.h"
#include "cvi_mipi_tx.h"
#include "cvi_region.h"
#include "cvi_math.h"
#include <cvi_base.h>
#include "vo_uapi.h"
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_vo.h"
#include "cvi_buffer.h"
#include <debug/dbg.h>

#define DRV_CHECK_RET(actual, fmt, arg...)                                   \
    do {																		 \
        if ((actual) != 0) {													 \
            printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
            return -1;																\
        }																		 \
    } while (0)
#define CVI_VO_DEV 0
#define CVI_VO_CHN 0
#define CVI_OSD_HANDLE 10
#ifndef CVI_DISPLAY_CANVAS_NUM
#define CVI_DISPLAY_CANVAS_NUM 2
#endif
static st7701s_gpio_pin g_st7701_pin = {0};
static RGN_CANVAS_INFO_S g_stCanvasInfo = {0};
static unsigned char * g_CanvasBuf[CVI_DISPLAY_CANVAS_NUM];
static unsigned int g_CanvasUpdate = 0;
static rvm_hal_display_event _display_event = NULL;
static rvm_hal_display_event _event = NULL;
static rvm_hal_display_event _aysn_event = NULL;
#if (CVI_DISPLAY_CANVAS_NUM == 1)
static unsigned char * g_CanvstmpBuf = NULL;
#endif
static void *_event_priv;
static void *_event_dev;
static uint8_t s_brightness = 255;

static int _set_event(rvm_dev_t *dev, rvm_hal_display_event event_cb, void *priv)
{
    _display_event = event_cb;
    _event_priv = priv;
    return 0;
}

static int _get_info(rvm_dev_t *dev, rvm_hal_display_info_t *info)
{
    info-> x_res = PANEL_WIDTH;
    info-> y_res = PANEL_HEIGHT;
    info-> bits_per_pixel = 32;
    info-> pixel_format = DISPLAY_PIXEL_FORMAT_ARGB_8888;
    for (int i =0 ; i < CVI_DISPLAY_CANVAS_NUM; i++) {
        info-> smem_start[i] = g_CanvasBuf[i];
    }
#if (CVI_DISPLAY_CANVAS_NUM == 1)
    info-> supported_feature = DISPLAY_FEATURE_ONE_FB;
    info-> smem_start[0] = g_CanvstmpBuf;
#else
    info-> supported_feature = DISPLAY_FEATURE_DOUBLE_FB;
#endif
    info-> smem_len = PANEL_WIDTH * PANEL_HEIGHT * info-> bits_per_pixel /8;
    return 0;
}

static int _get_framebuffer(rvm_dev_t *dev, void ***smem_start, size_t *smem_len)
{
    *smem_start = (void **)g_CanvasBuf;

    if (g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
        *smem_len = g_stCanvasInfo.stSize.u32Width * g_stCanvasInfo.stSize.u32Height * 4;
    }

    return 0;
}

static int _set_brightness(rvm_dev_t *dev, uint8_t brightness)
{
    csi_pwm_t pwm;
    if(csi_pwm_init(&pwm, g_st7701_pin.blank_pwm_bank) == 0 ){
        csi_pwm_out_stop(&pwm, g_st7701_pin.blank_pwm_chn);
        if(brightness != 0) {
            csi_pwm_out_config(&pwm, g_st7701_pin.blank_pwm_chn, 255, brightness, PWM_POLARITY_HIGH);
            csi_pwm_out_start(&pwm, g_st7701_pin.blank_pwm_chn);
            s_brightness = brightness;
            return 0;
        } else {
            return 0;
        }
    } else {
        printf("_blank_on_off err \n");
        return -1;
    }
    return 0;
}

static int _get_brightness(rvm_dev_t *dev, uint8_t *brightness)
{
    *brightness = s_brightness;

    return 0;
}

static int _write_area(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data)
{
    if (CVI_RGN_GetCanvasInfo(CVI_OSD_HANDLE, &g_stCanvasInfo) != CVI_SUCCESS) {
        printf("CVI_RGN_GetCanvasInfo failed \n");
        return -1;
    }

    uint32_t area_width = (area->x_leght - area->x_start + 1);
    if (g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
        for (uint32_t i = area->y_start; i < area->y_leght; i++) {
            unsigned char * tmp = g_CanvasBuf[g_CanvasUpdate % CVI_DISPLAY_CANVAS_NUM];
            memcpy(tmp + (i * g_stCanvasInfo.stSize.u32Width * 4 + area->x_start * 4)
                  , data + ((i - area->y_start) * area_width * 4), area_width * 4);
        }
    }

    CVI_VO_GetWaitVSync(0);
    CVI_RGN_UpdateCanvas(CVI_OSD_HANDLE);
    g_CanvasUpdate ++;
// #if (CVI_DISPLAY_CANVAS_NUM > 1)
//     //刷新备用缓冲
//     if (g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
//         for (uint32_t i = area->y_start; i < area->y_leght; i++) {
//             unsigned char * tmp = g_CanvasBuf[g_CanvasUpdate % CVI_DISPLAY_CANVAS_NUM] + (i * g_stCanvasInfo.stSize.u32Width * 4 + area->x_start * 4);
//             memcpy(tmp, data + (i * area->x_leght), area->x_leght * 4);
//         }
//     }
// #endif
    return 0;
}

static int _write_area_async(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data)
{
    if (CVI_RGN_GetCanvasInfo(CVI_OSD_HANDLE, &g_stCanvasInfo) != CVI_SUCCESS) {
        printf("CVI_RGN_GetCanvasInfo failed \n");
        return -1;
    }

    uint32_t area_width = (area->x_leght - area->x_start + 1);
    if (g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
        for (uint32_t i = area->y_start; i < area->y_leght; i++) {
            unsigned char * tmp = g_CanvasBuf[g_CanvasUpdate % CVI_DISPLAY_CANVAS_NUM];
            memcpy(tmp + (i * g_stCanvasInfo.stSize.u32Width * 4 + area->x_start * 4)
                  , data + ((i - area->y_start) * area_width * 4), area_width * 4);
        }
    }
    CVI_RGN_UpdateCanvas(CVI_OSD_HANDLE);
    g_CanvasUpdate ++;
// #if (CVI_DISPLAY_CANVAS_NUM > 1)
//     //刷新备用缓冲
//     if (g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
//         for (uint32_t i = area->y_start; i < area->y_leght; i++) {
//             unsigned char * tmp = g_CanvasBuf[g_CanvasUpdate % CVI_DISPLAY_CANVAS_NUM] + (i * g_stCanvasInfo.stSize.u32Width * 4 + area->x_start * 4);
//             memcpy(tmp, data + (i * area->x_leght), area->x_leght * 4);
//         }
//     }
// #endif
    _event_dev = dev;
    _aysn_event = _display_event;
    return 0;
}

static int _read_area(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data)
{
    return -1;
}

static int _pan_display(rvm_dev_t *dev)
{
    if (CVI_RGN_GetCanvasInfo(CVI_OSD_HANDLE, &g_stCanvasInfo) != CVI_SUCCESS) {
        printf("CVI_RGN_GetCanvasInfo failed \n");
        return -1;
    }
#if (CVI_DISPLAY_CANVAS_NUM == 1)
    if(g_CanvstmpBuf) {
        memcpy(g_CanvasBuf[g_CanvasUpdate % CVI_DISPLAY_CANVAS_NUM], g_CanvstmpBuf, g_stCanvasInfo.stSize.u32Width * g_stCanvasInfo.stSize.u32Height* 4);
    }
#endif
    // CVI_VO_GetWaitVSync(0);
    CVI_RGN_UpdateCanvas(CVI_OSD_HANDLE);
    _event = _display_event;
    return 0;
}

static int _blank_on_off(rvm_dev_t *dev, uint8_t on_off)
{
    if(on_off < 0) {
        return -1;
    }
    csi_pwm_t pwm;
    if(csi_pwm_init(&pwm, g_st7701_pin.blank_pwm_bank) == 0 ){
        csi_pwm_out_stop(&pwm, g_st7701_pin.blank_pwm_chn);
        if(on_off != 0) {
            csi_pwm_out_config(&pwm, g_st7701_pin.blank_pwm_chn, 255, 255, PWM_POLARITY_HIGH);
            csi_pwm_out_start(&pwm, g_st7701_pin.blank_pwm_chn);
            return 0;
        } else {
            return 0;
        }
    } else {
        printf("_blank_on_off err \n");
        return -1;
    }
    return 0;
}

static int osd_create_overlayex()
{
    int s32Ret;
    RGN_ATTR_S stRegion = {0};
    RGN_CHN_ATTR_S stRgnChnAttr = {0};
    MMF_CHN_S stMMFChn = {0};

    stRegion.enType = OVERLAY_RGN;
    stRegion.unAttr.stOverlay.enPixelFormat = PIXEL_FORMAT_ARGB_8888;
    stRegion.unAttr.stOverlay.stSize.u32Width = PANEL_WIDTH;
    stRegion.unAttr.stOverlay.stSize.u32Height = PANEL_HEIGHT;
    stRegion.unAttr.stOverlay.u32BgColor = 0x00000000; // transparent
    stRegion.unAttr.stOverlay.u32CanvasNum = CVI_DISPLAY_CANVAS_NUM;

    s32Ret = CVI_RGN_Create(CVI_OSD_HANDLE, &stRegion);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_Create failed with %#x!\n", s32Ret);
        return CVI_FAILURE;
    }

    stMMFChn.enModId = CVI_ID_VO;
    stMMFChn.s32DevId = CVI_VO_DEV;
    stMMFChn.s32ChnId = CVI_VO_CHN;
    stRgnChnAttr.bShow = CVI_TRUE;
    stRgnChnAttr.enType = OVERLAY_RGN;
    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 0;
    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 0;
    stRgnChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
    s32Ret = CVI_RGN_AttachToChn(CVI_OSD_HANDLE, &stMMFChn, &stRgnChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        return CVI_FAILURE;
    }

    for (int i =0 ; i < CVI_DISPLAY_CANVAS_NUM; i++) {
        if (CVI_RGN_GetCanvasInfo(CVI_OSD_HANDLE, &g_stCanvasInfo) != CVI_SUCCESS) {
            printf("CVI_RGN_GetCanvasInfo failed \n");
            return -1;
        }
        g_CanvasBuf[i] = g_stCanvasInfo.pu8VirtAddr;
        CVI_RGN_UpdateCanvas(CVI_OSD_HANDLE);
        g_CanvasUpdate ++;
    }
#if (CVI_DISPLAY_CANVAS_NUM == 1)
    g_CanvstmpBuf = (unsigned char *)malloc(PANEL_WIDTH * PANEL_HEIGHT * 4);
#endif
    return s32Ret;
}

static int osd_destory_overlayex()
{
    int s32Ret;
    MMF_CHN_S stMMFChn = {0};
    stMMFChn.enModId = CVI_ID_VO;
    stMMFChn.s32DevId = CVI_VO_DEV;
    stMMFChn.s32ChnId = CVI_VO_CHN;
    CVI_RGN_DetachFromChn(CVI_OSD_HANDLE, &stMMFChn);
    s32Ret = CVI_RGN_Destroy(CVI_OSD_HANDLE);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_Destroy failed with %#x!\n", s32Ret);
    } else {
        g_stCanvasInfo.pu8VirtAddr = NULL;
    }
    for (int i =0 ; i < CVI_DISPLAY_CANVAS_NUM; i++) {
        g_CanvasBuf[i] = NULL;
    }
    #if (CVI_DISPLAY_CANVAS_NUM == 1)
    if (g_CanvstmpBuf) {
        free(g_CanvstmpBuf);
        g_CanvstmpBuf = NULL;
    }
    #endif
    return s32Ret;
}

void _display_async_cb(void)
{
    if (_aysn_event) {
        _aysn_event(_event_dev, DISPLAY_EVENT_WRITE_DONE ,_event_priv);
        _aysn_event = NULL;
    }
    if (_event) {
        _event(_event_dev, DISPLAY_EVENT_FLUSH_DONE ,_event_priv);
        _event = NULL;
    }
}

rvm_dev_t *st7701s_devops_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t * dev = rvm_hal_device_new(drv, sizeof(display_driver_t), id);
    csi_pwm_t pwm;
    if(csi_pwm_init(&pwm, g_st7701_pin.blank_pwm_bank) == 0 ){
        csi_pwm_out_stop(&pwm, g_st7701_pin.blank_pwm_chn);
        csi_pwm_out_config(&pwm, g_st7701_pin.blank_pwm_chn, 255, 255, PWM_POLARITY_HIGH);
        csi_pwm_out_start(&pwm, g_st7701_pin.blank_pwm_chn);
    }
    CVI_VO_SetFlushEventCb(&_display_async_cb);
    osd_create_overlayex();//注册OSD
    return dev;
}

static void st7701s_devops_uninit(rvm_dev_t *dev)
{
    aos_check_param(dev);
    osd_destory_overlayex();
    rvm_hal_device_free(dev);
}

static int st7701s_devops_open(rvm_dev_t *dev)
{
    return 0;
}

static int st7701s_devops_close(rvm_dev_t *dev)
{
    return 0;
}

static int st7701s_devops_lpm(rvm_dev_t *dev, int state)
{
    return 0;
}

static display_driver_t _display_ops = {
    .drv = {
        .name = "disp",
        .open = st7701s_devops_open,
        .close = st7701s_devops_close,
        .init = st7701s_devops_init,
        .uninit = st7701s_devops_uninit,
        .lpm = st7701s_devops_lpm,
    },
    .set_event = _set_event,
    .get_info = _get_info,
    .get_framebuffer = _get_framebuffer,
    .set_brightness = _set_brightness,
    .get_brightness = _get_brightness,
    .write_area = _write_area,
    .write_area_async = _write_area_async,
    .read_area = _read_area,
    .pan_display = _pan_display,
    .blank_on_off = _blank_on_off,
};

void drv_st7701s_display_register(st7701s_gpio_pin *config)
{
    if (config) {
        g_st7701_pin.blank_pwm_bank = config->blank_pwm_bank;
        g_st7701_pin.blank_pwm_chn = config->blank_pwm_chn;
    }
    rvm_driver_register(&_display_ops.drv, NULL, 0);
}

void drv_st7701s_display_unregister()
{
    rvm_driver_unregister("disp");
}