#include "board.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <posix/timer.h>
#include <sys/time.h>
#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_vpss.h"
#include "cvi_buffer.h"
#include "cvi_venc.h"
//#include "test_rtsp.h"
#include "sys/prctl.h"
#include "fatfs_vfs.h"
#include "vfs.h"
#include "media_video.h"
#include "cvi_param.h"
#include "rtsp_func.h"

#define cvi_printf printf

#define TAG "app"
#define DUMP_BS 0
#define VENC_TEST_MAX_CHN 2

static int g_runstart[VENC_TEST_MAX_CHN];
static pthread_t VencTask[VENC_MAX_CHN_NUM];

#define APP_CHECK_RET(actual, fmt, arg...)                                   \
    do {																		 \
        if ((actual) != 0) {													 \
            aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
            return;																\
        }																		 \
    } while (0)

#define APP_CHECK_RET2(actual, fmt, arg...)                                   \
    do {																		 \
        if ((actual) != 0) {													 \
            aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
            return actual;														 \
        }																		 \
    } while (0)

#define APP_CHECK_RET3(actual, fmt, arg...)                                   \
    do {																		 \
        if ((actual) != 0) {													 \
            aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
            return (void *)CVI_SUCCESS;															\
        }																		 \
    } while (0)


static CVI_VOID* venc_streamproc(CVI_VOID *pvData)
{
    VENC_STREAM_S stStream = {0};
    CVI_U8 *pVeChn = (CVI_U8 *)pvData;
    CVI_CHAR cName[32] = {0};

    #if DUMP_BS
    int  i = 0;
    int  fd = -1;
    VENC_PACK_S *ppack;
    snprintf(cName, sizeof(cName), CONFIG_SD_FATFS_MOUNTPOINT"/video%d", *pVeChn);
    printf("venc_streamproc pVeChn is %d \n",*pVeChn);
    fd = aos_open(cName, O_CREAT | O_TRUNC | O_RDWR);
    if (fd < 0)
        cvi_printf("open %s fail \n", cName);
    #endif
    snprintf(cName, sizeof(cName), "venc_stmproc%d", *pVeChn);
    prctl(PR_SET_NAME, cName);

    while (!g_runstart[*pVeChn]) {
        if(MEDIA_VIDEO_VencGetStream(*pVeChn,&stStream,2000) == CVI_SUCCESS)
        {
            #if DUMP_BS
            for (i = 0; i < stStream.u32PackCount; i++) {
                    ppack = &stStream.pstPack[i];
                    aos_write(fd, ppack->pu8Addr + ppack->u32Offset, ppack->u32Len - ppack->u32Offset);
            }
            #endif
            #if (CONFIG_APP_RTSP_SUPPORT == 1)
            SendToRtsp(*pVeChn,&stStream);
            #endif
            MEDIA_VIDEO_VencReleaseStream(*pVeChn,&stStream);
        } else {
            usleep(20*1000);
        }

    }
    #if DUMP_BS
    aos_sync(fd);
    aos_close(fd);
    #endif

    return (void *)CVI_SUCCESS;
}

static void _start_venc(void)
{
    int ret = 0;
    int VeChn = 0;
    PARAM_VENC_CFG_S *pstVencCfg = PARAM_getVencCtx();
    PARAM_VENC_CHN_CFG_S *pstVecncChnCtx = NULL;

    if(!pstVencCfg) {
        return ;
    }

    struct sched_param param;
    pthread_attr_t pthread_attr;

    for (VeChn = 0; VeChn < pstVencCfg->s32VencChnCnt; VeChn++) {
        pstVecncChnCtx = &pstVencCfg->pstVencChnCfg[VeChn];
        if(pstVecncChnCtx->stChnParam.u16EnType != PT_H265 && pstVecncChnCtx->stChnParam.u16EnType != PT_H264) {
            continue;
        }
        param.sched_priority = 40;
        pthread_attr_init(&pthread_attr);
        pthread_attr_setschedpolicy(&pthread_attr, SCHED_RR);
        pthread_attr_setschedparam(&pthread_attr, &param);
        pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setstacksize(&pthread_attr, 8192);
        ret = pthread_create(&VencTask[VeChn], &pthread_attr, venc_streamproc, &pstVecncChnCtx->stChnParam.u8VencChn);
        if (ret != 0) {
            cvi_printf("[Chn %d]pthread_create failed, ret %d\n", VeChn, ret);
            return ;
        }
    }
    return ;
}

static void _stop_venc(void)
{
    PARAM_VENC_CFG_S *pstVencCfg = PARAM_getVencCtx();
    int VeChn = 0;

    if(!pstVencCfg) {
        return ;
    }
    for (VeChn = 0; VeChn < pstVencCfg->s32VencChnCnt; VeChn++) {
        g_runstart[VeChn] = 1;
        if (VencTask[VeChn])
            pthread_join(VencTask[VeChn], NULL);
        cvi_printf("venc stop chn:%d\n", VeChn);
    }
    return ;
}

void media_start_venc(int32_t argc, char **argv)
{
    _start_venc();
}

void media_stop_venc(int32_t argc, char **argv)
{
    _stop_venc();
}

ALIOS_CLI_CMD_REGISTER(media_start_venc, media_start_venc, media start venc);
ALIOS_CLI_CMD_REGISTER(media_stop_venc, media_stop_venc, media stop venc);
