#ifndef __CVI_MEDIALIST_H__
#define __CVI_MEDIALIST_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "cvi_type.h"

#define MEDIA_PROCUNIT_DATAMAXNUM 60
#define SIGNALE_MAXNUM 5 //信号槽最大成员数目
struct MEDIAPROCUNIN_CTX_S;
typedef CVI_S32 (*media_malloc_func)(void **dst, void *src);
typedef CVI_S32 (*media_relase_func)(void **src);
typedef void (*media_handler_func)(struct MEDIAPROCUNIN_CTX_S * ctx, void *parm, void *data);

typedef struct _MediaListHead_S
{
    void *data;
}MEIDALISTHEAD_S;

typedef struct _MEDIAUNITPROCNODE_S
{
    int front;
    int rear;
    MEIDALISTHEAD_S HeadNode[MEDIA_PROCUNIT_DATAMAXNUM];
}MEDIAUNITPROCNODE_S;

typedef struct {
    char * m_signalName;
    void * m_Pram;
    media_handler_func m_handlerFunc;
}MEDIAPROCUNIT_ARGS;

typedef struct MEDIAPROCUNIN_CTX_S {
    MEDIAUNITPROCNODE_S m_head;
    CVI_U8 m_runStatus;
    pthread_mutex_t m_mutex;
    pthread_t m_pthreadId;
    MEDIAPROCUNIT_ARGS m_slotArgs[SIGNALE_MAXNUM];//信号槽最多支持注册5只回调
    media_malloc_func m_mallocFunc;
    media_relase_func m_relaseFunc;
}MEDIAPROCUNIN_CTX;

CVI_S32 MEDIA_ListPushBack(MEDIAPROCUNIN_CTX * ctx, void *pstream);
//CVI_S32 MEDIA_ListPopFront(MEDIAPROCUNIN_CTX * ctx, void **pstream);
CVI_S32 MEDIA_RegisterArgs(MEDIAPROCUNIN_CTX * ctx, MEDIAPROCUNIT_ARGS args);
CVI_S32 MEDIA_UnRegisterArgs(MEDIAPROCUNIN_CTX * ctx, const char * signalName);
CVI_S32 MEDIA_ProcUnitInit(MEDIAPROCUNIN_CTX ** ctx, media_malloc_func malloc_func, media_relase_func release_func);
CVI_S32 MEDIA_ProcUnitDeInit(MEDIAPROCUNIN_CTX * ctx);

#endif
