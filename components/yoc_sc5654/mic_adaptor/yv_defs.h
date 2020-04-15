/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __IPC_COMMON__
#define __IPC_COMMON__

/* 下述定义的修改AP和CP两侧要一致 */
typedef enum {
    YV_AP_CMD_START = 0x00,
    YV_KWS_ENABLE_CMD, /* 使能CP唤醒算法 */
    YV_PCM_ENABLE_CMD, /* 启动和停止PCM传输 */
    YV_ASR_WAKE_CMD,   /* 发送强制唤醒命令到CP */
    YV_PCM_FREE_CMD,   /* 预留 */
    YV_PCM_SET_CMD,
    YV_PCM_GET_CMD,
    YV_EQ_PROC_CMD,
    YV_EQ_CONFIG_CMD,
    YV_EQ_INIT_CMD,
    YV_GET_DSP_VER,     /* 获取DSP版本 */
    YV_DBG_SET_MEMREC_ADDR,
    YV_CONFIG_SHARED_MEM,   /* 共享一块内存区域给DSP */

    YV_OFFLINE_GET_MAP = 0x70,  /* 从DSP获取离线命令词映射表 */

    YV_CP_CMD_START = 0x80, /* 以下是从CP接收到的命令 */
    YV_PCM_DATA_CMD,        /* 从CP接收到PCM数据 */
    YV_ASR_CMD,             /* 从CP接收到唤醒事件 */
    YV_SILENCE_CMD,         /* 从CP接收到交互超时事件 */
    YV_VAD_CMD,             /* VAD检测结果，有变化发送事件 */
    YV_VAD_DATA_CMD,        /* 从CP接收到VAD数据 */
    YV_KWS_DATA_CMD,        /* 从CP接收到KWS数据 */
} YV_IPC_CMD;

/*
 * 均衡器参数
 */

typedef struct {
    int channel_num; /* 采样通道数*/
    int sample_rate; /* 采样频率*/
    int type;        /* 均衡器种类*/
} yv_eq_param_t;

#define MAX_EQ_FILTER_NUM 15
#define MAX_EQ_NUM 5

typedef struct {
    int   filterNum; /* 段数量 */
    int   ch;        /* 通道 */
    int   index;     /* 当前段 */
    int   type;      /* 滤波器种类 */
    int   status;    /* 滤波器状态 */
    float fc;        /* 滤波器中心频率 */
    float gain;      /* 滤波器增益： -24.0 ~ 24.0 dB */
    float qvalue;    /* 宽广度：0.1 ~ 12 */
} filter_param_t;

typedef struct {
    int            eq_num;
    filter_param_t filters[MAX_EQ_NUM][MAX_EQ_FILTER_NUM];
} yv_eq_settings_t;

#endif
