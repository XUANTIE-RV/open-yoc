/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include "cvi_region.h"
#include "pthread.h"
#include <sys/prctl.h>
#include <lvgl.h>
#include <ulog/ulog.h>
#include "aos/kernel.h"
#include "board_config.h"
#include "app_gui.h"
#include "lv_port_disp.h"
#include "board_display.h"

#define TAG "ui"

static aos_mutex_t gui_lock;
lv_obj_t *g_GuiLableObj = NULL;


static void gui_label_create(void)
{
    static lv_style_t style;

    aos_mutex_lock(&gui_lock, AOS_WAIT_FOREVER);
    lv_style_init(&style);
    lv_style_set_bg_opa(&style, LV_OPA_TRANSP);
    // lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_text_font(&style,  &lv_font_montserrat_24);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED));

    g_GuiLableObj = lv_label_create(lv_scr_act());
    lv_obj_add_style(g_GuiLableObj, &style, LV_STATE_DEFAULT);
    lv_label_set_long_mode(g_GuiLableObj, LV_LABEL_LONG_WRAP);
    // lv_obj_set_align(g_GuiLableObj, LV_ALIGN_LEFT_MID);
    // lv_obj_set_size(g_GuiLableObj, 480, 100);
    lv_obj_set_pos(g_GuiLableObj, 0, 10);
    // lv_obj_align(g_GuiLableObj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_width(g_GuiLableObj, PANEL_WIDTH);
    lv_obj_set_style_text_align(g_GuiLableObj, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_add_event_cb(g_GuiLableObj, lv_event_cb, LV_EVENT_ALL, NULL);
    lv_label_set_text(g_GuiLableObj, "");
    aos_mutex_unlock(&gui_lock);
}

#ifdef CONFIG_RGBIR_SENSOR_SWITCH

#define CONFIG_GUI_RGN_DEVID 5
#define CONFIG_GUI_RGN_CHNID 0
#define OverlayMinHandle 10

static pthread_t g_GuiTaskId;
static CVI_S32 g_GuiRunStatus = 0;
static RGN_CANVAS_INFO_S g_stCanvasInfo = {0};
static int g_GuiDisplayCloseStatus = 0;
static lv_disp_drv_t disp_drv = {0};

static void lv_event_cb(lv_event_t *event)
{
    char *data = (char *)lv_event_get_param(event);
    if(data) {
        //printf("lv_event_cb data is %s \n",data);
    }

    lv_obj_t *obj = lv_event_get_target(event);
    aos_mutex_lock(&gui_lock, AOS_WAIT_FOREVER);
    if(event->code == LV_EVENT_PRESSED) {
        lv_label_set_text(obj,data);
    } else {
        lv_label_set_text(obj," ");
    }
    aos_mutex_unlock(&gui_lock);
}

static void *_gui_display_task(void *arg)
{
    char szThreadName[20]="gui_display_task";
    prctl(PR_SET_NAME, szThreadName, 0, 0, 0);
    // lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);//设置背景透明
    lv_obj_invalidate(lv_scr_act());
    gui_label_create();
    while(g_GuiRunStatus) {
        aos_mutex_lock(&gui_lock, AOS_WAIT_FOREVER);
        lv_task_handler();
        aos_mutex_unlock(&gui_lock);

        aos_msleep(3);

        // aos_mutex_lock(&gui_lock, AOS_WAIT_FOREVER);
        // lv_tick_inc(1);
        // aos_mutex_unlock(&gui_lock);
    }
    return NULL;
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
    stRegion.unAttr.stOverlay.u32CanvasNum = 1;

    s32Ret = CVI_RGN_Create(OverlayMinHandle, &stRegion);
    if (s32Ret != 0) {
        printf("CVI_RGN_Create failed with %#x!\n", s32Ret);
        return -1;
    }

    stMMFChn.enModId = CVI_ID_VPSS;
    stMMFChn.s32DevId = CONFIG_GUI_RGN_DEVID;
    stMMFChn.s32ChnId = CONFIG_GUI_RGN_CHNID;
    stRgnChnAttr.bShow = CVI_TRUE;
    stRgnChnAttr.enType = OVERLAY_RGN;
    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 0;
    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 0;
    stRgnChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;

    s32Ret = CVI_RGN_AttachToChn(OverlayMinHandle, &stMMFChn, &stRgnChnAttr);
    if (s32Ret != 0) {
        printf("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        return -1;
    }

    return s32Ret;
}

static int osd_destory_overlayex()
{
    int s32Ret;
    MMF_CHN_S stMMFChn = {0};
    stMMFChn.enModId = CVI_ID_VPSS;

    stMMFChn.s32DevId = CONFIG_GUI_RGN_DEVID;
    stMMFChn.s32ChnId = CONFIG_GUI_RGN_CHNID;

    CVI_RGN_DetachFromChn(OverlayMinHandle, &stMMFChn);
    s32Ret = CVI_RGN_Destroy(OverlayMinHandle);
    if (s32Ret != 0) {
        printf("CVI_RGN_Destroy failed with %#x!\n", s32Ret);
    } else {
        g_stCanvasInfo.pu8VirtAddr = NULL;
    }
    return s32Ret;
}

static void GUI_DisplayFlush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    lv_coord_t i = 0,j = 0;
    uint16_t area_hor = area->x2 - area->x1 + 1;
    unsigned int offset = 0;
    for(j = area->y1; j <= area->y2; j++) {
        offset = (area->x1 + (j * disp_drv->hor_res)) * sizeof(lv_color_t);
        memcpy(g_stCanvasInfo.pu8VirtAddr + offset, color_p + area_hor * i, area_hor * sizeof(lv_color_t));
        i++;
    }
#if 0
    if(g_GuiDisplayCloseStatus == 1) {
        if(g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
            uint16_t * tmpbuf = (uint16_t *) g_stCanvasInfo.pu8VirtAddr;
            for(i = 0; i < g_stCanvasInfo.stSize.u32Width; i++) {
                for(j = 0; j < g_stCanvasInfo.stSize.u32Height; j++) {
                    tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] &= 0x7FFF;
                }
            }
        }
        if(g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
            uint32_t * tmpbuf = (uint32_t *) g_stCanvasInfo.pu8VirtAddr;
            for(i = 0; i < g_stCanvasInfo.stSize.u32Width; i++) {
                for(j = 0; j < g_stCanvasInfo.stSize.u32Height; j++) {
                    tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] &= 0x00FFFFFF;
                }
            }
        }
    }
#endif
#if 0
    if(g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
        uint16_t * tmpbuf = (uint16_t *) g_stCanvasInfo.pu8VirtAddr;
        uint8_t _r5 = 0;
        uint8_t _g6 = 0;
        uint8_t _b5 = 0;
        for(i = 0; i < g_stCanvasInfo.stSize.u32Width; i++) {
            for(j = 0; j < g_stCanvasInfo.stSize.u32Height; j++) {
                _r5 = (tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] >> 11) & 0x1F;
                _g6 = (tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] >> 5) & 0x3F;
                _b5 = (tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] ) & 0x1F;
                //printf("_r5 %d _g6 %d _b5 %d \n",_r5,_g6,_b5);
                if(_r5 >= 28 && _g6 >= 57 && _b5 >= 28) {
                    tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] &= 0x7FFF;//ARGB  A set 0
                } else {
                    tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] |= 0x8000;//ARGB  A set 1
                }
            }
        }
    }
    if(g_stCanvasInfo.enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
        uint32_t * tmpbuf = (uint32_t *) g_stCanvasInfo.pu8VirtAddr;
        uint8_t _r8 = 0;
        uint8_t _g8 = 0;
        uint8_t _b8 = 0;
        for(i = 0; i < g_stCanvasInfo.stSize.u32Width; i++) {
            for(j = 0; j < g_stCanvasInfo.stSize.u32Height; j++) {
                _r8 = (tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] >> 16) &0xFF;
                _g8 = (tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] >> 8) &0xFF;
                _b8 = (tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i]) &0xFF;
                if((_r8 > 230 && _b8 > 230 && _g8 > 230)) {
                    tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] &= 0x00FFFFFF;
                } else {
                    tmpbuf[j * g_stCanvasInfo.stSize.u32Width + i] |= 0xFF000000;
                }
            }
        }
    }
#endif
    lv_disp_flush_ready(disp_drv);
}

CVI_S32 GUI_Display_LVGLInit()
{
    static int runStatus = 0;
    if(runStatus == 0 ) {
        runStatus = 1;
        lvgl_init();
        static lv_disp_draw_buf_t guiDispBuf = {0};
        static lv_color_t *flushbuf = NULL;
        if(flushbuf == NULL) {
            flushbuf = (lv_color_t *)aos_malloc(g_stCanvasInfo.stSize.u32Height * g_stCanvasInfo.stSize.u32Width* sizeof(lv_color_t));
        }
        if(flushbuf) {
            lv_disp_draw_buf_init(&guiDispBuf, flushbuf, NULL, g_stCanvasInfo.stSize.u32Height * g_stCanvasInfo.stSize.u32Width);
        }

        lv_disp_drv_init(&disp_drv); /*Basic initialization*/
        disp_drv.draw_buf   = &guiDispBuf;
        disp_drv.flush_cb = GUI_DisplayFlush;
        disp_drv.rotated  = 0;
        lv_disp_drv_register(&disp_drv);
    }
    return 0;
}

static void Gui_Label_SendEvent(CVI_U8 EventType,void *Data)
{
    if(g_GuiRunStatus == 1) {
        lv_event_send(g_GuiLableObj, EventType, Data);
    }
}

int GUI_IR_Display_Start()
{
    if(g_GuiRunStatus) {
        return 0;
    }
    int s32Ret = 0;

    if(g_stCanvasInfo.pu8VirtAddr == NULL) {
        osd_create_overlayex();
        s32Ret = CVI_RGN_GetCanvasInfo(OverlayMinHandle, &g_stCanvasInfo);
        if (s32Ret != 0) {
            printf("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
            return -1;
        }
    }

    // printf("stCanvasInfo.pu8VirtAddr:%p,width:%d height:%d\n",
    //     g_stCanvasInfo.pu8VirtAddr, g_stCanvasInfo.stSize.u32Width, g_stCanvasInfo.stSize.u32Height);

    s32Ret = aos_mutex_new(&gui_lock);
    if (s32Ret < 0) {
        printf("Create gui lock failed!\n");
        return -1;
    }
    
    GUI_Display_LVGLInit();
    g_GuiRunStatus = 1;
    if (pthread_create(&g_GuiTaskId, NULL, _gui_display_task, NULL) == -1) {
        g_GuiRunStatus = 0;
        printf("create main ui task fail\n");
        return -1;
    }
    // printf("GUI_IR_Display_Start success \n");
    return 0;
}

int GUI_IR_Display_Stop()
{
    if(!g_GuiRunStatus) {
        return 0;
    }
    //LVGL deinit流程存在问题，目前走销毁只停止线程 Mutex 和 RGN
    g_GuiRunStatus = 0;
    pthread_join(g_GuiTaskId,NULL);
    // lv_deinit();
    lv_obj_del(g_GuiLableObj);
    osd_destory_overlayex();
    aos_mutex_free(&gui_lock);
    return 0;
}

void GUI_IR_Display_Enable(int flag)
{
    lv_obj_t *ptmp = lv_scr_act();
    if(flag == 0) {
        printf("close display \n");
        g_GuiDisplayCloseStatus = 1;
        lv_obj_add_flag(ptmp, LV_OBJ_FLAG_HIDDEN);
    } else {
        printf("open display \n");
        g_GuiDisplayCloseStatus = 0;
        lv_obj_clear_flag(ptmp, LV_OBJ_FLAG_HIDDEN);
    }
}

#else
void _display_hander(void * args)
{
    /* handle the tasks of LVGL */
    while(1) {
        aos_mutex_lock(&gui_lock, AOS_WAIT_FOREVER);
        lv_task_handler();
        aos_mutex_unlock(&gui_lock);

        aos_msleep(3);
    }
}

void GUI_Init()
{
    board_display_init();

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    GUI_IR_Display_Start();
    GUI_IR_Display_Enable(1);
#endif

    aos_mutex_new(&gui_lock);

    lvgl_init();
    lv_port_disp_init();

    // set transparent background, otherwise the sensor screen would be coverd
    // by a white canvas
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, 0); 

    aos_task_new("ui", _display_hander, NULL, 8 * 1024);

    gui_label_create();
}

void GUI_Show_Text(const char *text)
{
    LOGD(TAG, "show text: %s", text);
    
    aos_mutex_lock(&gui_lock, AOS_WAIT_FOREVER);
    lv_label_set_text(g_GuiLableObj, text ? text : "");
    aos_mutex_unlock(&gui_lock);
}

void GUI_Show_FaceRecog_Result(bool success)
{
    GUI_Show_Text(success ? "Recognition Success!" : "Recognition Failed!");
}

void GUI_Show_Register_Result(bool success)
{
    GUI_Show_Text(success ? "Register Success!" : "Register Failed!");
}
#endif