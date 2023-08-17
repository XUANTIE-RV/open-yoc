/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <string>
#include "cvi_common.h"
#include "cvi_comm_sys.h"
#include "cvi_vb.h"
#include "sys_uapi.h"
#include "media_video.h"
#include "vi_snsr_i2c.h"

#include "cvi_buffer.h"
#include "cvi_math.h"
#include "vi_isp.h"
#include "vi_uapi.h"
#include "common_vi.h"
#include "vpss_uapi.h"
#include "vo_uapi.h"
#include "rgn_uapi.h"
#include "ldc_platform.h"
#include "cvi_sys.h"
#include "cvi_vi.h"

#include "dsi_panels.h"
#include "cvi_vo.h"
#include "cvi_venc.h"
#include "vi_snsr_i2c.h"
#include "cvi_region.h"

#include "board_config.h"
// #if CONFIG_SENSOE_IR0
// #include "fc_custom_media.h"
// #include "ir_custom_param.h"
// #endif
#include "media_config.h"
#include "cx/preview/preview_service.h"
#include <ulog/ulog.h>

#define TAG "MediaConfig"

using namespace std;

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
static int _meida_sensor_init(PARAM_VI_CFG_S * pstViCtx,CVI_U8 *devNum)
{
    //Sensor
    SNS_COMBO_DEV_ATTR_S devAttr;
    memset(&devAttr, 0, sizeof(devAttr));
    CVI_S32 snsr_type[2];
    ISP_SNS_OBJ_S *pSnsObj[2];
    ISP_CMOS_SENSOR_IMAGE_MODE_S stSnsrMode[2];
    CVI_U8 dev_num;
    ISP_SNS_COMMBUS_U unSnsrBusInfo = {0};
    ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc[2] = {0};
    ISP_INIT_ATTR_S InitAttr = {0};
    ALG_LIB_S stAeLib = {0};
    ALG_LIB_S stAwbLib = {0};
    CVI_S32 s32Ret;
    struct snsr_rst_gpio_s snsr_gpio;

    if(pstViCtx == NULL) {
        LOGE(TAG, "%s pstViCtx NULL err \n",__func__);
        return CVI_FAILURE;
    }
    MEDIA_CHECK_RET(getSnsType(snsr_type, &dev_num), "getSnsType fail");
    for (CVI_U8 i = 0; i < *devNum; i++) {
        pSnsObj[i] = getSnsObj((SNS_TYPE_E)snsr_type[i]);
        MEDIA_CHECK_RET(getSnsMode(i, &stSnsrMode[i]), "stSnsrMode fail");
    }
    /* clock enable */
    // vip_clk_en();
    /************************************************
     * start sensor
     ************************************************/
    InitAttr.enGainMode = SNS_GAIN_MODE_SHARE;

    for (CVI_U8  i = 0; i < *devNum; ++i) {
        if (!pSnsObj[i]) {
            LOGE(TAG, "sns obj[%d] is null.\n", i);
            return CVI_FAILURE;
        }
        unSnsrBusInfo.s8I2cDev = pstViCtx->pstSensorCfg[i].s8I2cDev;
        snsr_gpio.snsr_rst_port_idx = pstViCtx->pstSensorCfg[i].u32Rst_port_idx;
        snsr_gpio.snsr_rst_pin = pstViCtx->pstSensorCfg[i].u32Rst_pin;
        snsr_gpio.snsr_rst_pol = pstViCtx->pstSensorCfg[i].u32Rst_pol;
        cvi_cif_reset_snsr_gpio_init(i, &snsr_gpio);
		InitAttr.u16UseHwSync = pstViCtx->pstSensorCfg[i].bHwSync;
        if(pstViCtx->pstSensorCfg[i].s32I2cAddr != -1) {
            pSnsObj[i]->pfnPatchI2cAddr(pstViCtx->pstSensorCfg[i].s32I2cAddr);
        }
        pSnsObj[i]->pfnSetInit(i, &InitAttr);
        LOGD(TAG, "bus info:%d\n", unSnsrBusInfo.s8I2cDev);
        pSnsObj[i]->pfnSetBusInfo(i, unSnsrBusInfo);
        pSnsObj[i]->pfnRegisterCallback(i, &stAeLib, &stAwbLib);
        pSnsObj[i]->pfnExpSensorCb(&stSnsrSensorFunc[i]);
        stSnsrSensorFunc[i].pfn_cmos_sensor_global_init(i);
        s32Ret = stSnsrSensorFunc[i].pfn_cmos_set_image_mode(i, &stSnsrMode[i]);
        if (s32Ret != CVI_SUCCESS) {
            LOGE(TAG, "sensor set image mode failed!\n");
            return CVI_FAILURE;
        }
        s32Ret = stSnsrSensorFunc[i].pfn_cmos_set_wdr_mode(i, stSnsrMode[i].u8SnsMode);
        if (s32Ret != CVI_SUCCESS) {
            LOGE(TAG, "sensor set wdr mode failed!\n");
            return CVI_FAILURE;
        }
        if(i==0) {
            cif_reset_snsr_gpio(i, 1);
        }
        cif_reset_mipi(i);
        usleep(100);
        pSnsObj[i]->pfnGetRxAttr(i, &devAttr);
        //devAttr.mipi_attr.lane_id = pstViCtx->pstSensorCfg[i].as16LaneId;
        cif_set_dev_attr(&devAttr);
        cif_enable_snsr_clk(i, 1);
        usleep(100);
        cif_reset_snsr_gpio(i, 0);
        usleep(1000);
        if (pSnsObj[i]->pfnSnsProbe) {
            s32Ret = pSnsObj[i]->pfnSnsProbe(i);
            if (s32Ret) {
                LOGE(TAG, "sensor probe failed!\n");
                return CVI_FAILURE;
            }
        }
    }
    return CVI_SUCCESS;
}

static int MediaVideoRGBVbInit()
{
    CVI_U32 	u32BlkSize;
    CVI_U32 	u32RotBlkSize;
    VB_CONFIG_S stVbConfig;

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));

    int i=0;
	u32BlkSize = COMMON_GetPicBufferSize(320, 480, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32RotBlkSize = COMMON_GetPicBufferSize(480, 320, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX2(u32BlkSize, u32RotBlkSize);

	stVbConfig.astCommPool[i].u32BlkSize	= u32BlkSize;
	stVbConfig.astCommPool[i].u32BlkCnt	= 3;
	LOGD(TAG, "common pool[%d] BlkSize %d count %d\n", i, u32BlkSize, stVbConfig.astCommPool[i].u32BlkCnt);

    i++;
    u32BlkSize = COMMON_GetPicBufferSize(480, 640, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32RotBlkSize = COMMON_GetPicBufferSize(640, 480, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX2(u32BlkSize, u32RotBlkSize);

	stVbConfig.astCommPool[i].u32BlkSize	= u32BlkSize;
	stVbConfig.astCommPool[i].u32BlkCnt	= 2;
	LOGD(TAG, "common pool[%d] BlkSize %d count %d\n", i, u32BlkSize, stVbConfig.astCommPool[i].u32BlkCnt);

    i++;
	u32BlkSize = COMMON_GetPicBufferSize(960, 720, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32RotBlkSize = COMMON_GetPicBufferSize(720, 960, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX2(u32BlkSize, u32RotBlkSize);

	stVbConfig.astCommPool[i].u32BlkSize	= u32BlkSize;
	stVbConfig.astCommPool[i].u32BlkCnt	= 5;
	LOGD(TAG, "common pool[%d] BlkSize %d count %d\n", i, u32BlkSize, stVbConfig.astCommPool[i].u32BlkCnt);

    stVbConfig.u32MaxPoolCnt = i+1;

    MEDIA_CHECK_RET(CVI_VB_SetConfig(&stVbConfig), "CVI_VB_SetConfig failed\n");
    MEDIA_CHECK_RET(CVI_VB_Init(), "CVI_VB_Init failed\n");
    return 0;
}

int sensor_init_pre_hook(string sensor_name)
{
    extern PARAM_VI_CFG_S g_stViCtx;
    extern PARAM_VI_CFG_S g_stFcViCtx;
    if(sensor_name == "rgb0") {
        CustomEvent_IRGpioSet(0);
        CVI_U8 devNum = 0;

        devNum = g_stViCtx.u32WorkSnsCnt;
        MEDIA_CHECK_RET(_meida_sensor_init(&g_stViCtx, &devNum),"_meida_sensor_init fail");
        LOGD(TAG, "VIDEO The devNum is %d \n",devNum);
        CVI_VI_SetDevNum(devNum);
    } else if(sensor_name == "ir0") {
        CustomEvent_IRGpioSet(1);
        CVI_U8 devNum = 0;

        devNum = g_stFcViCtx.u32WorkSnsCnt;
        MEDIA_CHECK_RET(_meida_sensor_init(&g_stFcViCtx, &devNum),"_meida_sensor_init fail");
        LOGD(TAG, "VIDEO The devNum is %d \n",devNum);
        CVI_VI_SetDevNum(devNum);
    }
    return 0;
}

extern cx_preview_hdl_t preview_handle;
extern cx_dvr_hdl_t record_handle;
extern cxlv_hdl_t g_cloud_lv_handle;
extern cx_srv_hdl_t g_faceai_handler;
extern "C" void dvr_record_callback(cx_dvr_event_t event, void *data);
extern "C" void cloud_lv_callback_func(cx_lv_event_e event, void *data);

static int RgbSensorInit = 0;
static int IrSensorInit = 0;
int MediaVideoRGBInit()
{
    if(IrSensorInit) {
        MediaVideoIRDeInit();
    }
    if(RgbSensorInit) {
        return 0;
    }

    LOGD(TAG, "################## RGB enter\n");
    RgbSensorInit = 1;
    MEDIA_CHECK_RET(CVI_SYS_Init(), "CVI_SYS_Init failed\n");
    VPSS_MODE_S stVPSSMode;
    memset(&stVPSSMode, 0, sizeof(stVPSSMode));
    VI_VPSS_MODE_S stVIVPSSMode;
    memset(&stVIVPSSMode, 0, sizeof(stVIVPSSMode));
    stVIVPSSMode.aenMode[0] = VI_OFFLINE_VPSS_ONLINE;
    stVPSSMode.enMode = VPSS_MODE_DUAL;
    stVPSSMode.aenInput[0] = VPSS_INPUT_ISP;
    stVPSSMode.ViPipe[0] = 0;
    stVPSSMode.aenInput[1] = VPSS_INPUT_MEM;
    stVPSSMode.ViPipe[1] = 0;
    MEDIA_CHECK_RET(CVI_SYS_SetVIVPSSMode(&stVIVPSSMode),"CVI_SYS_SetVIVPSSMode failed\n");
    CVI_SYS_SetVPSSModeEx(&stVPSSMode);

    MediaVideoRGBVbInit();

    /* start all RGB service*/
    record_handle = cx_dvr_start("recorder", dvr_record_callback);
    preview_handle = cx_preview_start("preview", PREVIEW_MODE_RGB);
    g_cloud_lv_handle = cx_lv_start("linkvisual", cloud_lv_callback_func);

    return 0;
}

int MediaVideoRGBDeInit() 
{
    if(!RgbSensorInit) {
        return 0;
    }

    /* close all RGB service*/
    cx_preview_stop(preview_handle);
    cx_dvr_stop(record_handle);
    cx_lv_stop(g_cloud_lv_handle);

    RgbSensorInit = 0;
    LOGD(TAG, "################## RGB exit\n");
    CVI_VB_Exit();
    CVI_SYS_Exit();

    return 0;
}

static int MediaVideoIrVbInit()
{
    CVI_U32 	u32BlkSize;
    CVI_U32 	u32RotBlkSize;
    VB_CONFIG_S stVbConfig;

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt = 2;

    u32BlkSize = COMMON_GetPicBufferSize(480, 640, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8
					, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32RotBlkSize = COMMON_GetPicBufferSize(640, 480, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8
					, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX2(u32BlkSize, u32RotBlkSize);

    stVbConfig.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConfig.astCommPool[0].u32BlkCnt	= 8;
    LOGD(TAG, "common pool[0] BlkSize %d count %d\n", u32BlkSize, stVbConfig.astCommPool[0].u32BlkCnt);

    u32BlkSize = COMMON_GetPicBufferSize(320, 480, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8
					, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32RotBlkSize = COMMON_GetPicBufferSize(480, 320, PIXEL_FORMAT_NV21, DATA_BITWIDTH_8
					, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX2(u32BlkSize, u32RotBlkSize);

    stVbConfig.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConfig.astCommPool[1].u32BlkCnt	= 3;
    LOGD(TAG, "common pool[1] BlkSize %d count %d\n", u32BlkSize, stVbConfig.astCommPool[1].u32BlkCnt);
    MEDIA_CHECK_RET(CVI_VB_SetConfig(&stVbConfig), "CVI_VB_SetConfig failed\n");
    MEDIA_CHECK_RET(CVI_VB_Init(), "CVI_VB_Init failed\n");
    return 0;
}

int MediaVideoIRInit()
{
    if(RgbSensorInit) {
        MediaVideoRGBDeInit();
    }
    if(IrSensorInit) {
        return 0;
    }

    LOGD(TAG, "################## IR enter\n");
    IrSensorInit = 1;
    MEDIA_CHECK_RET(CVI_SYS_Init(), "CVI_SYS_Init failed\n");
    VPSS_MODE_S stVPSSMode;
    memset(&stVPSSMode, 0, sizeof(stVPSSMode));
    VI_VPSS_MODE_S stVIVPSSMode;
    memset(&stVIVPSSMode, 0, sizeof(stVIVPSSMode));
    stVIVPSSMode.aenMode[0] = VI_ONLINE_VPSS_ONLINE;
    stVIVPSSMode.aenMode[1] = VI_ONLINE_VPSS_ONLINE;
    stVPSSMode.enMode = VPSS_MODE_DUAL;
    stVPSSMode.aenInput[0] = VPSS_INPUT_MEM;
    stVPSSMode.ViPipe[0] = 0;
    stVPSSMode.aenInput[1] = VPSS_INPUT_ISP;
    stVPSSMode.ViPipe[1] = 0;
    MEDIA_CHECK_RET(CVI_SYS_SetVIVPSSMode(&stVIVPSSMode),"CVI_SYS_SetVIVPSSMode failed\n");
    CVI_SYS_SetVPSSModeEx(&stVPSSMode);

    MediaVideoIrVbInit();
    /* start ir service*/
    app_faceai_start();

    return 0;
}

int MediaVideoIRDeInit() 
{
    if(!IrSensorInit) {
        return 0;
    }

    /* stop ir service*/
#if defined(CONFIG_IR_GUI_EN) && CONFIG_IR_GUI_EN
    GUI_IR_Display_Stop();
    cx_preview_stop(preview_handle);
#endif
    app_faceai_stop();

    IrSensorInit = 0;
    LOGD(TAG, "################## IR exit\n");
    CVI_VB_Exit();
    CVI_SYS_Exit();

    return 0;
}
#else

int sensor_init_pre_hook(string sensor_name)
{
    return 0;
}

static int _meida_sensor_init(PARAM_VI_CFG_S * pstViCtx,CVI_U8 *devNum)
{
    //Sensor
    SNS_COMBO_DEV_ATTR_S devAttr = {(input_mode_e)0};
    CVI_S32 snsr_type[2];
    ISP_SNS_OBJ_S *pSnsObj[2];
    ISP_CMOS_SENSOR_IMAGE_MODE_S stSnsrMode[2];
    CVI_U8 dev_num;
    ISP_SNS_COMMBUS_U unSnsrBusInfo = {0};
    ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc[2] = {0};
    ISP_INIT_ATTR_S InitAttr = {0};
    ALG_LIB_S stAeLib = {0};
    ALG_LIB_S stAwbLib = {0};
    CVI_S32 s32Ret;
    struct snsr_rst_gpio_s snsr_gpio;

    if(pstViCtx == NULL) {
        MEDIABUG_PRINTF("%s pstViCtx NULL err \n",__func__);
        return CVI_FAILURE;
    }
    MEDIA_CHECK_RET(getSnsType(snsr_type, &dev_num), "getSnsType fail");
    for (CVI_U8 i = 0; i < *devNum; i++) {
        pSnsObj[i] = getSnsObj((SNS_TYPE_E)snsr_type[i]);
        MEDIA_CHECK_RET(getSnsMode(i, &stSnsrMode[i]), "stSnsrMode fail");
    }
    /* clock enable */
    // vip_clk_en();
    /************************************************
     * start sensor
     ************************************************/
    InitAttr.enGainMode = SNS_GAIN_MODE_SHARE;

    for (CVI_U8  i = 0; i < *devNum; ++i) {
        if (!pSnsObj[i]) {
            MEDIABUG_PRINTF("sns obj[%d] is null.\n", i);
            return CVI_FAILURE;
        }
        unSnsrBusInfo.s8I2cDev = pstViCtx->pstSensorCfg[i].s8I2cDev;
        snsr_gpio.snsr_rst_port_idx = pstViCtx->pstSensorCfg[i].u32Rst_port_idx;
        snsr_gpio.snsr_rst_pin = pstViCtx->pstSensorCfg[i].u32Rst_pin;
        snsr_gpio.snsr_rst_pol = pstViCtx->pstSensorCfg[i].u32Rst_pol;
        cvi_cif_reset_snsr_gpio_init(i, &snsr_gpio);
		InitAttr.u16UseHwSync = pstViCtx->pstSensorCfg[i].bHwSync;
        if(pstViCtx->pstSensorCfg[i].s32I2cAddr != -1) {
            pSnsObj[i]->pfnPatchI2cAddr(pstViCtx->pstSensorCfg[i].s32I2cAddr);
        }
        pSnsObj[i]->pfnSetInit(i, &InitAttr);
        MEDIABUG_PRINTF("bus info:%d\n", unSnsrBusInfo.s8I2cDev);
        pSnsObj[i]->pfnSetBusInfo(i, unSnsrBusInfo);
        pSnsObj[i]->pfnRegisterCallback(i, &stAeLib, &stAwbLib);
        pSnsObj[i]->pfnExpSensorCb(&stSnsrSensorFunc[i]);
        stSnsrSensorFunc[i].pfn_cmos_sensor_global_init(i);
        s32Ret = stSnsrSensorFunc[i].pfn_cmos_set_image_mode(i, &stSnsrMode[i]);
        if (s32Ret != CVI_SUCCESS) {
            MEDIABUG_PRINTF("sensor set image mode failed!\n");
            return CVI_FAILURE;
        }
        s32Ret = stSnsrSensorFunc[i].pfn_cmos_set_wdr_mode(i, stSnsrMode[i].u8SnsMode);
        if (s32Ret != CVI_SUCCESS) {
            MEDIABUG_PRINTF("sensor set wdr mode failed!\n");
            return CVI_FAILURE;
        }
        if(pstViCtx->pstSensorCfg[i].u8DisableRst != CVI_TRUE) {
            cif_reset_snsr_gpio(i, 1);
        }
        cif_reset_mipi(i);
        udelay(100);
        pSnsObj[i]->pfnGetRxAttr(i, &devAttr);
        if (pstViCtx->pstSensorCfg[i].bSetDevAttrMipi != 0) {
            for (CVI_U8 j = 0; j < MIPI_LANE_NUM+1; j++) {
                devAttr.mipi_attr.lane_id[j] = pstViCtx->pstSensorCfg[i].as16LaneId[j];
                devAttr.mipi_attr.pn_swap[j] = pstViCtx->pstSensorCfg[i].as8PNSwap[j];
            }
            devAttr.mipi_attr.wdr_mode = (enum mipi_wdr_mode_e)pstViCtx->pstSensorCfg[i].s32WDRMode;
        }
        if(pstViCtx->pstSensorCfg[i].bSetDevAttr != 0) {
            devAttr.devno = i;
            devAttr.mac_clk = (enum rx_mac_clk_e)pstViCtx->pstSensorCfg[i].s16MacClk;
            devAttr.mclk.cam = pstViCtx->pstSensorCfg[i].u8MclkCam;
            devAttr.mclk.freq = (enum cam_pll_freq_e)pstViCtx->pstSensorCfg[i].u8MclkFreq;
        }
        cif_set_dev_attr(&devAttr);
    }
    for (CVI_U8  i = 0; i < *devNum; ++i) {
        if (!pSnsObj[i]) {
            continue;
        }
        if(pstViCtx->pstSensorCfg[i].u8DisableRst != CVI_TRUE) {
            pSnsObj[i]->pfnGetRxAttr(i, &devAttr);
            cif_enable_snsr_clk(i, 1);
            usleep(100);
            cif_reset_snsr_gpio(i, 0);
            udelay(100);
        }
        if (pSnsObj[i]->pfnSnsProbe) {
            s32Ret = pSnsObj[i]->pfnSnsProbe(i);
            if (s32Ret) {
                MEDIABUG_PRINTF("sensor probe failed!\n");
                return CVI_FAILURE;
            }
        }
    }
    return CVI_SUCCESS;
}

int _MEDIA_VIDEO_DSIInit(int devno, const struct dsc_instr *cmds, int size)
{
    CVI_S32 s32Ret = CVI_SUCCESS;

    for (int i = 0; i < size; i++) {
        const struct dsc_instr *instr = &cmds[i];
        struct cmd_info_s cmd_info = {
            .devno = (unsigned int)devno,
            .data_type = instr->data_type,
            .cmd_size = instr->size,
            .cmd = (unsigned char *)instr->data
        };

        s32Ret = mipi_tx_send_cmd(0, &cmd_info);
        if (s32Ret != CVI_SUCCESS) {
            MEDIABUG_PRINTF("dsi init failed at %d instr.\n", i);
            return CVI_FAILURE;
        }
        if (instr->delay) {
            udelay(instr->delay * 1000);
        }
    }

    return 0;
}

void *_MEDIA_VIDEO_PanelInit(void *data)
{
    int fd = 0;
    CVI_S32 s32Ret = CVI_SUCCESS;
    struct panel_desc_s *panel_desc = (struct panel_desc_s *)data;

    s32Ret = mipi_tx_rstpin(0);
    if (s32Ret != CVI_SUCCESS) {
        MEDIABUG_PRINTF("mipi_tx_rstpin failed with %#x\n", s32Ret);
        return NULL;
    }

#if (CONFIG_PANEL_READID == 1)
    CVI_U32 panelid[3] = {0}, param[3] = {0xDA, 0xDB, 0xDC};
    CVI_U8 buf[4], i;

    for (i = 0; i < 3; ++i) {
        struct get_cmd_info_s get_cmd_info = {
            .devno = 0,
            .data_type = 0x06,
            .data_param = param[i],
            .get_data_size = 0x01,
            .get_data = buf
        };

        memset(buf, 0, sizeof(buf));
        if (mipi_tx_recv_cmd(0, &get_cmd_info)) {
            MEDIABUG_PRINTF("%s get panel id fialed!\n", __func__);
            return NULL;
        }
        panelid[i] = buf[0];
    }
    printf("%s panel id (0x%02x 0x%02x 0x%02x)!\n", __func__, panelid[0], panelid[1], panelid[2]);

    // modify panel_desc accord to panel id.
    s32Ret = mipi_tx_cfg(0, panel_desc->dev_cfg);
    if (s32Ret != CVI_SUCCESS) {
        MEDIABUG_PRINTF("mipi_tx_cfg failed with %#x\n", s32Ret);
        return NULL;
    }
#endif

    s32Ret = _MEDIA_VIDEO_DSIInit(0, panel_desc->dsi_init_cmds, panel_desc->dsi_init_cmds_size);
    if (s32Ret != CVI_SUCCESS) {
        MEDIABUG_PRINTF("dsi_init failed with %#x\n", s32Ret);
        return NULL;
    }
    s32Ret = mipi_tx_set_hs_settle(fd, panel_desc->hs_timing_cfg);
    if (s32Ret != CVI_SUCCESS) {
        MEDIABUG_PRINTF("mipi_tx_set_hs_settle failed with %#x\n", s32Ret);
        return NULL;
    }
    s32Ret = mipi_tx_enable(fd);
    if (s32Ret != CVI_SUCCESS) {
        MEDIABUG_PRINTF("mipi_tx_enable failed with %#x\n", s32Ret);
        return NULL;
    }
    MEDIABUG_PRINTF("Init for MIPI-Driver-%s\n", panel_desc->panel_name);

    pthread_exit(NULL);
    return NULL;
}

static int PanelInit(void)
{
    static int initstatus = 0;
    CVI_S32 s32Ret = CVI_SUCCESS;

    if (initstatus == 0) {
        initstatus = 1;
        s32Ret = mipi_tx_init(panel_desc.dev_cfg);
        if (s32Ret != CVI_SUCCESS) {
            MEDIABUG_PRINTF("mipi_tx_init failed with %#x\n", s32Ret);
            return s32Ret;
        }

        s32Ret = mipi_tx_disable(0);
        if (s32Ret != CVI_SUCCESS) {
            MEDIABUG_PRINTF("mipi_tx_disable failed with %#x\n", s32Ret);
            return s32Ret;
        }
        s32Ret = mipi_tx_cfg(0, panel_desc.dev_cfg);
        if (s32Ret != CVI_SUCCESS) {
            MEDIABUG_PRINTF("mipi_tx_cfg failed with %#x\n", s32Ret);
            return s32Ret;
        }

        struct sched_param param;
        pthread_attr_t attr;
        pthread_condattr_t cattr;
        pthread_t thread;

        param.sched_priority = MIPI_TX_RT_PRIO;
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &param);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_condattr_init(&cattr);
        pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
        pthread_create(&thread, &attr, _MEDIA_VIDEO_PanelInit, (void *)&panel_desc);
        pthread_setname_np(thread, "cvi_mipi_tx");
    }
    return 0;
}

static int VoInit(PARAM_VO_CFG_S * pstVoCtx)
{
    if(pstVoCtx == NULL) {
        return CVI_FAILURE;
    }

    if (pstVoCtx->u8VoCnt == 0) {
        return CVI_SUCCESS;
    }
    if (pstVoCtx->pstPanelTxCfg != NULL && panel_desc.dev_cfg != NULL) {
        if (pstVoCtx->pstPanelTxCfg->bset_attr == 1) {
            for(int i = 0; i < 5; i++) {
                panel_desc.dev_cfg->lane_id[i] = (mipi_tx_lane_id)pstVoCtx->pstPanelTxCfg->lane_id[i];
                panel_desc.dev_cfg->lane_pn_swap[i] = pstVoCtx->pstPanelTxCfg->lane_pn_swap[i];
            }
            panel_desc.dev_cfg->output_mode = (output_mode_e)pstVoCtx->pstPanelTxCfg->output_mode;
            panel_desc.dev_cfg->video_mode = (video_mode_e)pstVoCtx->pstPanelTxCfg->video_mode;
            panel_desc.dev_cfg->output_format = (output_format_e)pstVoCtx->pstPanelTxCfg->output_format;
        }
        if (pstVoCtx->pstPanelTxCfg->bset_pin == 1) {
            panel_desc.dev_cfg->reset_pin = pstVoCtx->pstPanelTxCfg->reset_pin;
            panel_desc.dev_cfg->pwm_pin = pstVoCtx->pstPanelTxCfg->pwm_pin;
            panel_desc.dev_cfg->power_ct_pin = pstVoCtx->pstPanelTxCfg->power_ct_pin;
        }
    }
    PanelInit();

    MEDIABUG_PRINTF("******start vo******\n");
    return CVI_SUCCESS;
}

int MediaVideoInit()
{
    CVI_U8 devNum = 0;

    PARAM_LoadCfg();
    
    PARAM_SYS_CFG_S * pstSysCtx = PARAM_getSysCtx();
    MEDIA_CHECK_RET(MEDIA_VIDEO_SysVbInit(pstSysCtx),"MEDIA_VIDEO_SysVbInit failed");

    PARAM_VI_CFG_S * pstViCfg = PARAM_getViCtx();
    if(pstViCfg->u32WorkSnsCnt != 0) {
        devNum = pstViCfg->u32WorkSnsCnt;
        MEDIA_CHECK_RET(_meida_sensor_init(pstViCfg,&devNum),"_meida_sensor_init fail");

        CVI_VI_SetDevNum(devNum);
    }

    PARAM_VO_CFG_S * pstVoCtx = PARAM_getVoCtx();
    VoInit(pstVoCtx);

    return 0;
}
#endif