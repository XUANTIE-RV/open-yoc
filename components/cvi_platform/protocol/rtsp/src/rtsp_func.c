#include <unistd.h>
#include "rtsp_func.h"
#include "board.h"
#include "cvi_venc.h"
#include "rtsp.h"
#include "media_video.h"
#include "cvi_param.h"
#include "media_procunit.h"
#define RTSP_MAX_LIVE 4

static CVI_RTSP_CTX *ctx = NULL;
static CVI_RTSP_SESSION *pSession[RTSP_MAX_LIVE] = {NULL};
static int rtsp_init_flag = 0;
static int rtsp_session_init_flag[RTSP_MAX_LIVE] = {0};
static int rtsp_session_chn_flag[RTSP_MAX_LIVE] = {0};
static MEDIAPROCUNIN_CTX * rtsp_mediaprocunit_ctx[RTSP_MAX_LIVE] = {0};
static CVI_VOID rtsp_connect(const char *ip, CVI_VOID *arg)
{
    printf("rtsp connect: %s\n", ip);
}

static CVI_VOID disconnect(const char *ip, CVI_VOID *arg)
{
    printf("rtsp disconnect: %s\n", ip);
}

CVI_S32 SendToRtsp(int live, VENC_STREAM_S *pstStream)
{
    CVI_S32 s32Ret = CVI_SUCCESS;
    VENC_PACK_S *ppack;
    CVI_RTSP_DATA data = {0};

    if (0 == rtsp_init_flag || 0 == rtsp_session_init_flag[live]) {
        return -1;
    }

    data.blockCnt = pstStream->u32PackCount;
    for (CVI_U32 i = 0; i < pstStream->u32PackCount; i++) {
        ppack = &pstStream->pstPack[i];
        data.dataPtr[i] = ppack->pu8Addr + ppack->u32Offset;
        data.dataLen[i] = ppack->u32Len - ppack->u32Offset;
    }

    s32Ret = CVI_RTSP_WriteFrame(ctx, pSession[live]->video, &data);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RTSP_WriteFrame failed\n");
        return -1;
    }

    return s32Ret;
}

static void *rtsp_sendProc(void *args)
{
    int chn = *((int *)args);
    VENC_STREAM_S stStream = {0};
    while(rtsp_session_init_flag[chn]) {
        if(MEDIA_VIDEO_VencGetStream(chn,&stStream,2000) == CVI_SUCCESS) {
            MEDIA_ListPushBack(rtsp_mediaprocunit_ctx[chn], (void *)&stStream);
            MEDIA_VIDEO_VencReleaseStream(chn,&stStream);
        } else {
            usleep(1*1000);
        }
    }
    return 0;
}

static CVI_S32 _media_malloc_func(void **dst, void *src)
{
    //申请 VENC_STREAM_S 成员空间
    if(!dst || !src) {
        return CVI_FAILURE;
    }
    CVI_U32 i = 0;
    VENC_STREAM_S *pdst = NULL;
    VENC_STREAM_S *psrc = (VENC_STREAM_S *) src;
    pdst = (VENC_STREAM_S *)malloc(sizeof(VENC_STREAM_S));
    if(pdst == NULL) {
        return CVI_FAILURE;
    }
    memcpy(pdst,psrc,sizeof(VENC_STREAM_S));
    pdst->pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * psrc->u32PackCount);
    if(!pdst->pstPack) {
        return CVI_FAILURE;
    }
    memset(pdst->pstPack,0,sizeof(VENC_PACK_S) * psrc->u32PackCount);
    for(i = 0;i < psrc->u32PackCount; i++) {
        memcpy(&pdst->pstPack[i],&psrc->pstPack[i],sizeof(VENC_PACK_S));
        pdst->pstPack[i].pu8Addr = (CVI_U8 *)malloc(psrc->pstPack[i].u32Len);
        if(!pdst->pstPack[i].pu8Addr) {
            goto exit;
        }
        memcpy(pdst->pstPack[i].pu8Addr,psrc->pstPack[i].pu8Addr,psrc->pstPack[i].u32Len);
    }
    *dst = (void *) pdst;
    return CVI_SUCCESS;
exit:
    if(pdst->pstPack) {
        for(i = 0;i < psrc->u32PackCount; i++) {
            if(pdst->pstPack[i].pu8Addr) {
                free(pdst->pstPack[i].pu8Addr);
            }
        }
        free(pdst->pstPack);
    }
    return CVI_FAILURE;
}

static CVI_S32 _media_relase_func(void **src)
{
    //释放VENC_STREAM_S成员空间
    VENC_STREAM_S * psrc = NULL;

    if(src == NULL) {
        return CVI_FAILURE;
    }
    psrc = (VENC_STREAM_S *) *src;
    if(!psrc) {
        return CVI_FAILURE;
    }
    if(psrc->pstPack) {
        for(CVI_U32 i = 0;i < psrc->u32PackCount; i++) {
            if(psrc->pstPack[i].pu8Addr) {
                free(psrc->pstPack[i].pu8Addr);
            }
        }
        free(psrc->pstPack);
    }
    if(psrc) {
        free(psrc);
    }
    return CVI_SUCCESS;
}

static void _media_handler_func(struct MEDIAPROCUNIN_CTX_S * ctx, void *parm, void *data)
{
    if (ctx == NULL) {
        return ;
    }
    int chn = (*(int *)(parm));
    VENC_STREAM_S * pstStream = (VENC_STREAM_S *)data;
    SendToRtsp(chn, pstStream);
}

CVI_VOID rtsp_setup(int live, int type)
{
    CVI_RTSP_SESSION_ATTR attr = {0};
    CVI_RTSP_CONFIG config = {0};
    pthread_t pthreadId = 0;
    char threadname[64] = {0};
    struct sched_param param;
    pthread_attr_t pthread_attr;

    if (live >= RTSP_MAX_LIVE) {
        printf("live too max:%d \n", live);
        return ;
    }

    if (type != RTSP_VIDEO_H264 && type != RTSP_VIDEO_H265 && type != RTSP_VIDEO_JPEG) {
        printf("type:%d not support \n", type);
        return ;
    }

    if (0 == rtsp_init_flag) {
        config.port = 544;
        if (CVI_RTSP_Create(&ctx, &config) < 0) {
            printf("fail to create rtsp\n");
            return ;
        }
    }
    rtsp_init_flag = 1;

    attr.video.codec = type;
    sprintf(attr.name, "live%d", live);

    int s32Ret = CVI_RTSP_CreateSession(ctx, &attr, &pSession[live]);
    if (s32Ret != 0) {
        printf("CVI_RTSP_CreateSession failed with %#x\n", s32Ret);
        return ;
    }

    CVI_RTSP_STATE_LISTENER listener = {0};

    listener.onConnect = rtsp_connect;
    listener.argConn = ctx;
    listener.onDisconnect = disconnect;
    CVI_RTSP_SetListener(ctx, &listener);

    if (CVI_RTSP_Start(ctx) < 0) {
        printf("fail to rtsp start\n");
    }

    if(rtsp_session_init_flag[live] == 0 ) {
        rtsp_session_init_flag[live] = 1;
        rtsp_session_chn_flag[live] = live;
        MEDIAPROCUNIT_ARGS _args = {0};
        _args.m_signalName = "rtsp_procunit";
        _args.m_Pram = (void *)&rtsp_session_chn_flag[live];
        _args.m_handlerFunc = _media_handler_func;
        MEDIA_ProcUnitInit(&rtsp_mediaprocunit_ctx[live], _media_malloc_func, _media_relase_func);
        MEDIA_RegisterArgs(rtsp_mediaprocunit_ctx[live], _args);
        param.sched_priority = 30;
        pthread_attr_init(&pthread_attr);
        pthread_attr_setschedpolicy(&pthread_attr, SCHED_RR);
        pthread_attr_setschedparam(&pthread_attr, &param);
        pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
        pthread_create(&pthreadId,&pthread_attr,rtsp_sendProc,&rtsp_session_chn_flag[live]);
        snprintf(threadname,sizeof(threadname),"rtsp_sendProc%d",live);
        pthread_setname_np(pthreadId, threadname);
    }
}

static void *_rtsp_handler(void *args)
{
    PARAM_VENC_CFG_S *pstVencCfg = PARAM_getVencCtx();
    PARAM_VENC_CHN_CFG_S *pstVencChnCfg = NULL;
    int s32rtspType = 0;

    if(!pstVencCfg) {
        return 0;
    }
#if (CONFIG_APP_ETHERNET_SUPPORT == 1 && CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    extern uint8_t g_wifi_got_ip;
    extern uint8_t g_eth_got_ip;
    while(g_wifi_got_ip == 0 && g_eth_got_ip == 0 ) {
        usleep(10*1000);
    }
#elif (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    extern uint8_t g_wifi_got_ip;
    while(g_wifi_got_ip == 0) {
        usleep(10*1000);
    }
#elif (CONFIG_APP_ETHERNET_SUPPORT == 1)
    //extern uint8_t g_eth_got_ip;
    //while(g_eth_got_ip == 0 ) {
    //    usleep(10*1000);
    //}
#endif
    sleep(2);//确保网络做异步初始化完成 已经调用tcpip_init 注册了lwip
    //确保LWIP初始化并且可以拿到IP 才可以启动
    for(int i = 0; i < pstVencCfg->s32VencChnCnt ; i++) {
        pstVencChnCfg = &pstVencCfg->pstVencChnCfg[i];
        if(pstVencChnCfg->stChnParam.u16EnType == PT_H265) {
            s32rtspType = RTSP_VIDEO_H265;
        } else if (pstVencChnCfg->stChnParam.u16EnType == PT_H264) {
            s32rtspType = RTSP_VIDEO_H264;
        } else if (pstVencChnCfg->stChnParam.u16EnType == PT_MJPEG) {
            s32rtspType = RTSP_VIDEO_JPEG;
        } else  {
            continue;
        }
        rtsp_setup(i,s32rtspType);
    }
    return 0;
}

void cvi_rtsp_init()
{
    pthread_t pthreadId;
    pthread_create(&pthreadId,NULL,_rtsp_handler,NULL);
}
