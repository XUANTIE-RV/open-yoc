#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <aos/kernel.h>
#include <lvgl.h>
#include "gui_display.h"
#include "aos/cli.h"
#include "cvi_mipi_tx.h"
#include "cvi_region.h"
#include "fatfs_vfs.h"
#include "cvi_math.h"
#include "cvi_vb.h"
#include "cvi_vo.h"
#include "cvi_buffer.h"
#include "vfs.h"
#include "cvi_param.h"
#include "lv_event.h"
#include "lv_obj.h"
#include "lv_color.h"
#include "debug/dbg.h"
#define OverlayMinHandle 10

static lv_obj_t * g_GuiScreen = NULL;
static CVI_S32 g_GuiRunStatus = 0;
static pthread_t g_GuiTaskId;
static RGN_CANVAS_INFO_S g_stCanvasInfo = {0};
static int g_GuiDisplayCloseStatus = 0;
lv_obj_t *g_GuiLableObj = NULL;
static pthread_mutex_t g_GuiDisplayMutex;

void lv_event_cb(lv_event_t * event)
{
    char *data = (char *)lv_event_get_param(event);
    if(data) {
        //printf("lv_event_cb data is %s \n",data);
    }
    if(event->code == LV_EVENT_PRESSED) {
        lv_label_set_text(g_GuiLableObj, data);
    } else if (event->code == LV_EVENT_PRESS_LOST) {
        lv_label_set_text(g_GuiLableObj, " ");
    }
}

void Gui_Label_SendEvent(CVI_U8 EventType,void *Data)
{
    if(g_GuiRunStatus == 1) {
        pthread_mutex_lock(&g_GuiDisplayMutex);
        lv_event_send(g_GuiLableObj, EventType, Data);
        pthread_mutex_unlock(&g_GuiDisplayMutex);
    }
}

static void gui_label_create(void)
{
    //static lv_style_t style_btn;
    static lv_style_t style_btn;
    lv_style_init(&style_btn);
    lv_style_set_opa(&style_btn, 100);
    lv_style_set_text_align(&style_btn, LV_TEXT_ALIGN_LEFT);
    lv_style_set_opa(&style_btn, 100);
	lv_style_set_text_color(&style_btn, lv_color_make(0, 0, 0xff));
    g_GuiLableObj = lv_label_create(g_GuiScreen);
    lv_obj_add_style(g_GuiLableObj, &style_btn, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(g_GuiLableObj, lv_color_make(0, 0, 0xff), 0);
    lv_label_set_long_mode(g_GuiLableObj, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(g_GuiLableObj, 0, 0);
    lv_obj_set_size(g_GuiLableObj, PANEL_WIDTH, PANEL_HEIGHT);
    lv_label_set_text(g_GuiLableObj, " ");
    lv_obj_add_event_cb(g_GuiLableObj, lv_event_cb, LV_EVENT_ALL, NULL);
}

static void *_gui_display_task(void *arg)
{
    char szThreadName[20]="gui_display_task";
    prctl(PR_SET_NAME, szThreadName, 0, 0, 0);
    g_GuiScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_GuiScreen, lv_color_make(0, 0, 0xff), 0);
    lv_obj_set_style_bg_opa(g_GuiScreen, LV_OPA_TRANSP, 0);
    lv_obj_invalidate(g_GuiScreen);
    gui_label_create();
    lv_scr_load(g_GuiScreen);
    while(g_GuiRunStatus) {
        pthread_mutex_lock(&g_GuiDisplayMutex);
        lv_task_handler();
        pthread_mutex_unlock(&g_GuiDisplayMutex);
        aos_msleep(3);
        lv_tick_inc(1);
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
#if (CONFIG_PANEL_ILI9488 == 1)
    stRegion.unAttr.stOverlay.stSize.u32Width = PANEL_WIDTH;
    stRegion.unAttr.stOverlay.stSize.u32Height = PANEL_HEIGHT;
#else
    stRegion.unAttr.stOverlay.stSize.u32Width = PANEL_HEIGHT;
    stRegion.unAttr.stOverlay.stSize.u32Height = PANEL_WIDTH;
#endif
    stRegion.unAttr.stOverlay.u32BgColor = 0x00000000; // transparent
    stRegion.unAttr.stOverlay.u32CanvasNum = 1;

    s32Ret = CVI_RGN_Create(OverlayMinHandle, &stRegion);
    if (s32Ret != CVI_SUCCESS) {
        LOGE(__FILE__,"CVI_RGN_Create failed with %#x!\n", s32Ret);
        return CVI_FAILURE;
    }

    stMMFChn.enModId = CVI_ID_VPSS;
#ifdef CONFIG_GUI_RGN_DEVID
    stMMFChn.s32DevId = CONFIG_GUI_RGN_DEVID;
#else
    stMMFChn.s32DevId = 0;
#endif
#ifdef CONFIG_GUI_RGN_CHNID
    stMMFChn.s32ChnId = CONFIG_GUI_RGN_CHNID;
#else
    stMMFChn.s32ChnId = 1;
#endif
    stRgnChnAttr.bShow = CVI_TRUE;
    stRgnChnAttr.enType = OVERLAY_RGN;
    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 0;
    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 0;
    stRgnChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;

    s32Ret = CVI_RGN_AttachToChn(OverlayMinHandle, &stMMFChn, &stRgnChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        LOGE(__FILE__,"CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        return CVI_FAILURE;
    }

    return s32Ret;
}

static int osd_destory_overlayex()
{
    int s32Ret;
    MMF_CHN_S stMMFChn = {0};
    stMMFChn.enModId = CVI_ID_VPSS;
#ifdef CONFIG_GUI_RGN_DEVID
    stMMFChn.s32DevId = CONFIG_GUI_RGN_DEVID;
#else
    stMMFChn.s32DevId = 0;
#endif
#ifdef CONFIG_GUI_RGN_CHNID
    stMMFChn.s32ChnId = CONFIG_GUI_RGN_CHNID;
#else
    stMMFChn.s32ChnId = 1;
#endif
    CVI_RGN_DetachFromChn(OverlayMinHandle, &stMMFChn);
    s32Ret = CVI_RGN_Destroy(OverlayMinHandle);
    if (s32Ret != CVI_SUCCESS) {
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
        lv_init();
        static lv_disp_draw_buf_t guiDispBuf = {0};
        static lv_color_t *flushbuf = NULL;
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv); /*Basic initialization*/
        if(flushbuf == NULL) {
            flushbuf = (lv_color_t *)aos_malloc(g_stCanvasInfo.stSize.u32Height * g_stCanvasInfo.stSize.u32Width* sizeof(lv_color_t));
        }
        if(flushbuf) {
            aos_debug_printf("g_stCanvasInfo.stSize.u32Height %d u32Width %d\r\n",g_stCanvasInfo.stSize.u32Height, g_stCanvasInfo.stSize.u32Width);
            lv_disp_draw_buf_init(&guiDispBuf, flushbuf, NULL,  g_stCanvasInfo.stSize.u32Height * g_stCanvasInfo.stSize.u32Width);
        }
        disp_drv.full_refresh = 0;//是否启动全刷新
        disp_drv.flush_cb = GUI_DisplayFlush;
        disp_drv.hor_res = g_stCanvasInfo.stSize.u32Width;
        disp_drv.ver_res = g_stCanvasInfo.stSize.u32Height;
        disp_drv.draw_buf = &guiDispBuf;
        lv_disp_drv_register(&disp_drv);
    }
    return CVI_SUCCESS;
}

CVI_S32 GUI_Display_Start()
{
    CVI_S32 s32Ret = 0;
    if(g_stCanvasInfo.pu8VirtAddr == NULL) {
        osd_create_overlayex();
        s32Ret = CVI_RGN_GetCanvasInfo(OverlayMinHandle, &g_stCanvasInfo);
        if (s32Ret != CVI_SUCCESS) {
            LOGE(__FILE__,"CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
            return CVI_FAILURE;
        }
    }
    LOGD(__FILE__,"stCanvasInfo.pu8VirtAddr:%p,width:%d height:%d\n",
        g_stCanvasInfo.pu8VirtAddr, g_stCanvasInfo.stSize.u32Width, g_stCanvasInfo.stSize.u32Height);
    GUI_Display_LVGLInit();
    pthread_mutex_init(&g_GuiDisplayMutex, NULL);
    g_GuiRunStatus = 1;
    pthread_attr_t threadAttr = {0};
    pthread_attr_init(&threadAttr);
    pthread_attr_setstacksize(&threadAttr, 8192);
    if (pthread_create(&g_GuiTaskId, &threadAttr, _gui_display_task, NULL) == -1) {
        g_GuiRunStatus = 0;
        LOGE(__FILE__,"create main ui task fail\n");
        return CVI_FAILURE;
    }
    LOGD(__FILE__,"GUI_Display_Start success \n");
    return CVI_SUCCESS;
}

CVI_S32 GUI_Display_Stop()
{
    g_GuiRunStatus = 0;
    pthread_join(g_GuiTaskId,NULL);
    //lv_deinit();
    lv_obj_del(g_GuiLableObj);
    osd_destory_overlayex();
    pthread_mutex_destroy(&g_GuiDisplayMutex);
    return CVI_SUCCESS;
}

//Gui_Label_SendEvent(LV_EVENT_PRESSED,"12345");
void Send_FaceRecognition_event(int argc, char **argv)
{
    Gui_Label_SendEvent(LV_EVENT_PRESSED,"recognition success");
}

void Send_DisFaceRecognition_event(int argc, char **argv)
{
    Gui_Label_SendEvent(LV_EVENT_PRESS_LOST," ");
}

void GUI_Dispaly_Switch(int argc, char **argv)
{
    if(argc < 2) {
        printf("Input 0 close gui\n");
        printf("Input 1 open gui\n");
        return ;
    }
    lv_obj_t *ptmp = lv_scr_act();
    int display_switch = atoi(argv[1]);
    if(display_switch == 0) {
        printf("close display \n");
        g_GuiDisplayCloseStatus = 1;
        lv_obj_add_flag(ptmp, LV_OBJ_FLAG_HIDDEN);
    } else {
        printf("open display \n");
        g_GuiDisplayCloseStatus = 0;
        lv_obj_clear_flag(ptmp, LV_OBJ_FLAG_HIDDEN);
    }
}

ALIOS_CLI_CMD_REGISTER(Send_FaceRecognition_event,test_gui_FaceRecognition,test_gui_press)
ALIOS_CLI_CMD_REGISTER(Send_DisFaceRecognition_event,test_gui_DisFaceRecognition,test_gui_dispress)
ALIOS_CLI_CMD_REGISTER(GUI_Dispaly_Switch,gui_display_switch,gui_display_switch)

