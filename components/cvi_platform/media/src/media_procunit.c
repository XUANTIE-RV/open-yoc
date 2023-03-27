#include <aos/kernel.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <sys/prctl.h>
#include "debug/dbg.h"
#include "media_procunit.h"

CVI_S32 MEDIA_ListPopFront(MEDIAPROCUNIN_CTX * ctx, void **pstream);

int cvi_MeidaCommonListInit(MEDIAUNITPROCNODE_S *Head)
{
    //初始化链表头
    if (!Head)
    {
        return -1;
    }
    Head->front = 0;
    Head->rear = 0;
    return 0;
}

static int _media_list_isFull(MEDIAUNITPROCNODE_S * head)
{
    if (head == NULL) {
        return CVI_FAILURE;
    }
    if (head->front == (head->rear + 1) % MEDIA_PROCUNIT_DATAMAXNUM) {
        return CVI_SUCCESS;
    }
    return CVI_FAILURE;
}

static int _media_list_isEmpty(MEDIAUNITPROCNODE_S * head)
{
    if (head == NULL) {
        return CVI_FAILURE;
    }
    if (head->front == head->rear) {
        return CVI_SUCCESS;
    }
    return CVI_FAILURE;
}

static int _media_list_insert(MEDIAUNITPROCNODE_S * Head, void *data)
{
    Head->HeadNode[Head->rear].data = data;
    Head->rear = (Head->rear + 1) % MEDIA_PROCUNIT_DATAMAXNUM;
    return CVI_SUCCESS;
}

static int _media_list_popfront(MEDIAUNITPROCNODE_S * Head, void **data)
{
    *data = Head->HeadNode[Head->front].data; 
    Head->front = (Head->front + 1) % MEDIA_PROCUNIT_DATAMAXNUM;
    return CVI_SUCCESS;
}

CVI_S32 MEDIA_ListPushBack(MEDIAPROCUNIN_CTX * ctx, void *pstream)
{
    //list插入队列
    if (!pstream) {
        return CVI_FAILURE;
    }
    if (ctx == NULL) {
        return CVI_FAILURE;
    }
    if( ctx->m_runStatus == 0) {
        return CVI_FAILURE;
    }
    MEDIAUNITPROCNODE_S * Head = &ctx->m_head;
    void *data = NULL;
    if (ctx->m_mallocFunc) {
        if (ctx->m_mallocFunc(&data,pstream) != CVI_SUCCESS) {
            goto exit;
        }
    }
    if (_media_list_isFull(Head) == CVI_SUCCESS) {
        //aos_debug_printf("_media_list_isFull \r\n");
        void *psteam = NULL;
        //printf("MEDIA_ListPushBack %d max \n", ctx->m_listSize);
        MEDIA_ListPopFront(ctx, &psteam);
        if (psteam) {
            if (ctx->m_relaseFunc) {
                ctx->m_relaseFunc(&psteam);
            }
            psteam = NULL;
        }
    }
    pthread_mutex_lock(&ctx->m_mutex);
    if (_media_list_isFull(Head) != CVI_SUCCESS) {
        _media_list_insert(Head, data);
    }
    pthread_mutex_unlock(&ctx->m_mutex);
    return CVI_SUCCESS;
exit:
    if(data) {
        if (ctx->m_relaseFunc) {
            ctx->m_relaseFunc(&data);
        }
    }
    return CVI_FAILURE;
}

CVI_S32 MEDIA_ListPopFront(MEDIAPROCUNIN_CTX * ctx, void **pstream)
{
    //VENC_STREAM_S list弹出队列
    if(!pstream) {
        return CVI_FAILURE;
    }
    if(ctx == NULL) {
        return CVI_FAILURE;
    }
    MEDIAUNITPROCNODE_S * Head = &ctx->m_head;
    void * data = NULL;
    if(_media_list_isEmpty(Head) == CVI_SUCCESS) {
        //aos_debug_printf("_media_list_isEmpty \r\n");
        return CVI_FAILURE;
    }
    pthread_mutex_lock(&ctx->m_mutex);
    _media_list_popfront(Head, &data);
    if (data != NULL) {
        *pstream = data;
    }
    pthread_mutex_unlock(&ctx->m_mutex);
    return CVI_SUCCESS;
}

void *CVI_MEIDA_EventHander(void *args)
{
    //CVI_MEIDA_EventHander 出队处理线程
    MEDIAPROCUNIN_CTX *pstMediaProcCtx = (MEDIAPROCUNIN_CTX *)args;
    void *psteam = NULL;
    prctl(PR_SET_NAME, "MediaEventHander", 0, 0, 0);
    while (pstMediaProcCtx->m_runStatus) {
        //弹出链表
        if (MEDIA_ListPopFront(pstMediaProcCtx, &psteam) == CVI_SUCCESS) {
            if (psteam) {
                for (int i = 0; i < SIGNALE_MAXNUM; i++) {
                    if (pstMediaProcCtx->m_slotArgs[i].m_handlerFunc) {//下放数据到消费者
                        pstMediaProcCtx->m_slotArgs[i].m_handlerFunc(pstMediaProcCtx, pstMediaProcCtx->m_slotArgs[i].m_Pram, psteam);
                    }
                }
                if (pstMediaProcCtx->m_relaseFunc) {
                    pstMediaProcCtx->m_relaseFunc(&psteam);
                }
                psteam = NULL;
            }
        }
        aos_msleep(2);
    }
    return 0;
}

CVI_S32 MEDIA_RegisterArgs(MEDIAPROCUNIN_CTX * ctx, MEDIAPROCUNIT_ARGS args)
{
    if(ctx == NULL) {
        return CVI_FAILURE;
    }
    for (int i = 0 ; i < SIGNALE_MAXNUM; i++) {
        if (ctx->m_slotArgs[i].m_handlerFunc == NULL &&
            ctx->m_slotArgs[i].m_signalName == NULL &&
            ctx->m_slotArgs[i].m_Pram == NULL) {
                ctx->m_slotArgs[i].m_Pram = args.m_Pram;
                ctx->m_slotArgs[i].m_signalName = args.m_signalName;
                ctx->m_slotArgs[i].m_handlerFunc = args.m_handlerFunc;
                break;
        }
    }
    return CVI_SUCCESS;
}

CVI_S32 MEDIA_UnRegisterArgs(MEDIAPROCUNIN_CTX * ctx, const char * signalName)
{
    if (ctx == NULL) {
        return CVI_FAILURE;
    }
    for( int i = 0 ; i < SIGNALE_MAXNUM; i++) {
        if (strcmp(ctx->m_slotArgs[i].m_signalName, signalName) == 0) {
            ctx->m_slotArgs[i].m_handlerFunc = NULL;
            ctx->m_slotArgs[i].m_signalName = NULL;
            ctx->m_slotArgs[i].m_Pram = NULL;
        }
    }
    return CVI_SUCCESS;
}

CVI_S32 MEDIA_ProcUnitInit(MEDIAPROCUNIN_CTX ** ctx, media_malloc_func malloc_func, media_relase_func release_func)
{
    if ( ctx == NULL) {
        return CVI_FAILURE;
    }
    *ctx = (MEDIAPROCUNIN_CTX *)malloc(sizeof(MEDIAPROCUNIN_CTX));
    if (*ctx == NULL) {
        return CVI_FAILURE;
    }
    memset(*ctx, 0, sizeof(MEDIAPROCUNIN_CTX));
    //链表初始化 线程初始化
    pthread_mutex_init(&((*ctx)->m_mutex), NULL);
    cvi_MeidaCommonListInit(&(*ctx)->m_head);
    (*ctx)->m_mallocFunc = malloc_func;
    (*ctx)->m_relaseFunc = release_func;
    (*ctx)->m_runStatus = 1;
    if (pthread_create(&(*ctx)->m_pthreadId, NULL, CVI_MEIDA_EventHander, (void *)(*ctx)) != 0) {
        pthread_mutex_destroy(&(*ctx)->m_mutex);
        (*ctx)->m_runStatus = 0;
        goto exit;
    }
    return CVI_SUCCESS;
exit:
    if ((*ctx) != NULL) {
        free(*ctx);
        *ctx = NULL;
    }
    return CVI_FAILURE;
}

CVI_S32 MEDIA_ProcUnitDeInit(MEDIAPROCUNIN_CTX * ctx)
{
    //销毁EXIT
    void *psteam = NULL;

    if (ctx == NULL) {
        return CVI_FAILURE;
    }
    if(ctx->m_runStatus == 0) {
        return CVI_FAILURE;
    }
    ctx->m_runStatus = 0;
    pthread_join(ctx->m_pthreadId, NULL);
    while (MEDIA_ListPopFront(ctx, &psteam) == CVI_SUCCESS) {
        if (psteam) {
            if (ctx->m_relaseFunc) {
                ctx->m_relaseFunc(&psteam);
            }
            psteam = NULL;
        }
    }
    pthread_mutex_destroy(&ctx->m_mutex);
    free(ctx);
    return CVI_SUCCESS;
}
