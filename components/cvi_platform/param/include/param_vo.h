#ifndef __PARAM_VO_H__
#define __PARAM_VO_H__
#include <stdio.h>
#include "cvi_defines.h"
#include "cvi_comm_vo.h"

//多级别结构 确定使用的多级数组加number作标定 (多级限定场景 串行结构 不允许存在同一级别有多个多级存在) 大结构必须精简尽量使用指针

typedef struct _PARAM_VODEV_S {
    VO_CONFIG_S stVoConfig;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_LAYER VoLayer;
    CVI_U8 u8ChnCnt;
    CVI_U8 u8Bindmode;
    MMF_CHN_S stSrcChn;
    MMF_CHN_S stDestChn;
    VO_CHN_ATTR_S *pstVOChnAttr;
    CVI_U8 u8ChnRotation;
    CVI_BOOL bSetPin;
    struct gpio_desc_s stResetPin;
    struct gpio_desc_s stPwmPin;
    struct gpio_desc_s stPowerctPin;
}PARAM_VODEV_S;

typedef struct _PARAM_MIPIDEV_S {
    CVI_BOOL                    bset_attr;
    CVI_S32                     lane_id[5];
    CVI_S32                     output_mode;
    CVI_S32                     video_mode;
    CVI_S32                     output_format;
    CVI_U32                     phy_data_rate;
    CVI_U32                     pixel_clk;
    CVI_BOOL                    lane_pn_swap[5];
    CVI_BOOL                    bset_pin;
    struct gpio_desc_s          reset_pin;
    struct gpio_desc_s          pwm_pin;
    struct gpio_desc_s          power_ct_pin;
}PARAM_PANELDEV_S;


typedef struct _PARAM_VO_CFG_S {
    CVI_U8 u8VoCnt;
    PARAM_VODEV_S *pstVoCfg;
    PARAM_PANELDEV_S *pstPanelTxCfg;
} PARAM_VO_CFG_S;



#endif
