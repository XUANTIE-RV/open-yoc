#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include "media_nightvision.h"
#include "cvi_param.h"
#include "cvi_isp.h"
#include "cvi_bin.h"
#include "platform.h"
#include "cvi_ae.h"
#include <aos/kernel.h>
#include <aos/cli.h>

static ISP_IR_AUTO_ATTR_S g_stIrAttr[VI_MAX_DEV_NUM] = {0};
int g_runStatus = 0;
pthread_t g_taskThread = 0;

__attribute__((weak)) void MEDIA_NightVision_GetAutoAttr(ISP_DEV IspDev, ISP_IR_AUTO_ATTR_S *pstIrAttr)
{
    //默认参数，如果需要实现自己得定制化参数请在custom中对应实现进行覆盖
    pstIrAttr->bEnable = 1;
    pstIrAttr->u32Normal2IrIsoThr = 1400; //dark threshold
    pstIrAttr->u32Ir2NormalIsoThr = 300;  //normal threshold
    pstIrAttr->u32RGMin = 150;
    pstIrAttr->u32RGMax = 170;
    pstIrAttr->u32BGMin = 155;
    pstIrAttr->u32BGMax = 170;
}

__attribute__((weak)) int MEDIA_NightVision_IsRGBMode()
{
    if(PARAM_GET_MANAGER_CFG_PIPE() == 0) {
        return 0;
    } else {
        return -1;
    }
}

int MEDIA_NigthVision_RunAutoIr(int DevId)
{
    PARAM_VI_CFG_S * pstViCfg = PARAM_getViCtx();
    unsigned int frameID = 0;
    if(pstViCfg == NULL || DevId >= VI_MAX_DEV_NUM) {
        return -1;
    }
    CVI_ISP_GetFrameID(DevId, &frameID);
    if (frameID <= 0) {
        return -1;
    }
    if(MEDIA_NightVision_IsRGBMode() == 0) {
        CVI_ISP_IrAutoRunOnce(DevId, &g_stIrAttr[DevId]);
        if(g_stIrAttr[DevId].enIrSwitch == ISP_IR_SWITCH_TO_IR) {
            //load rgb mono param
            if(pstViCfg->u32IspSceneNum >= 2 && pstViCfg->pstIspCfg[1].stPQBinDes.
            pIspBinData) {
                CVI_BIN_ImportBinData(pstViCfg->pstIspCfg[1].stPQBinDes.pIspBinData,
                pstViCfg->pstIspCfg[1].stPQBinDes.u32IspBinDataLen);
            } else {
                //path路径
                //CVI_BIN_LoadParamFromBin
            }
            g_stIrAttr[DevId].enIrStatus = 1;
            //ircut open led open
            PLATFORM_IrCutCtl(10000);
            usleep(100 * 1000);
        } else if(g_stIrAttr[DevId].enIrSwitch == ISP_IR_SWITCH_TO_NORMAL) {
            // load rgb param
            if(pstViCfg->u32IspSceneNum >= 1 && pstViCfg->pstIspCfg[0].
            stPQBinDes.pIspBinData) {
                CVI_BIN_ImportBinData(pstViCfg->pstIspCfg[0].stPQBinDes.pIspBinData,
                pstViCfg->pstIspCfg[0].stPQBinDes.u32IspBinDataLen);
            } else {
                //path路径
                //CVI_BIN_LoadParamFromBin
            }
            // close ircut close ir led
            g_stIrAttr[DevId].enIrStatus = 0;
            PLATFORM_IrCutCtl(0);
            usleep(100 * 1000);
        }
    } else {
        return -1;
    }
    return 0;
}

void * _media_nightvision_handle(void *args)
{
    prctl(PR_SET_NAME, "nightvision", 0, 0, 0);
    while(g_runStatus) {
        PARAM_VI_CFG_S * pstViCfg = PARAM_getViCtx();
        for (int i = 0; i < pstViCfg->u32WorkSnsCnt; i++) {
            MEDIA_NigthVision_RunAutoIr(i);
        }
        usleep(5 * 1000);
    }
    return 0;
}

int MEDIA_NightVisionInit()
{
    if(g_runStatus == 0) {
        for (int i = 0; i < VI_MAX_DEV_NUM; i++) {
            MEDIA_NightVision_GetAutoAttr(i, &g_stIrAttr[i]);
        }
        g_runStatus = 1;
        pthread_attr_t TaskAttr = {0};
        pthread_attr_init(&TaskAttr);
        pthread_attr_setstacksize(&TaskAttr, 8192);
        pthread_create(&g_taskThread, &TaskAttr, _media_nightvision_handle, NULL);
    }
    return 0;
}

int MEDIA_NightVisionDeInit()
{
    if(g_runStatus == 1) {
        g_runStatus = 0;
        pthread_join(&g_taskThread, NULL);
        memset(g_stIrAttr, 0, sizeof(ISP_IR_AUTO_ATTR_S) * VI_MAX_DEV_NUM);
    }
    return 0;
}
#if 0
void testMedia_NightVisionInit(int32_t argc, char **argv)
{
    MEDIA_NightVisionInit();
}

void testMedia_NightVisionDeInit(int32_t argc, char **argv)
{
    MEDIA_NightVisionDeInit();
}

void testMedia_NightVisionAutoRun(int32_t argc, char **argv)
{
    if(argc < 2) {
        printf("Please input testMedia_NightVisionAutoRun 0/1 \n");
        return ;
    }
    if(atoi(argv[1]) == 0 || atoi(argv[1]) == 1) {
        MEDIA_NigthVision_RunAutoIr(atoi(argv[1]));
    }
}
ALIOS_CLI_CMD_REGISTER(testMedia_NightVisionInit, testMedia_NightVisionInit, testMedia_NightVisionInit);
ALIOS_CLI_CMD_REGISTER(testMedia_NightVisionDeInit, testMedia_NightVisionDeInit, testMedia_NightVisionDeInit);
ALIOS_CLI_CMD_REGISTER(testMedia_NightVisionAutoRun, testMedia_NightVisionAutoRun, testMedia_NightVisionAutoRun);
#endif


void cli_switch_led(int argc,char **argv)
{
	if (2 == argc)
	{
		if (0 == atoi(argv[1]))
		{
			PLATFORM_IrCutCtl(0);
		}
		else if(1 == atoi(argv[1]))
		{
			PLATFORM_IrCutCtl(10000);
		}
	} 
    else 
    {
        printf("usage: cli_switch_led 0/1\r\n");
    }
}
ALIOS_CLI_CMD_REGISTER(cli_switch_led, cli_switch_led, cli_switch_led);