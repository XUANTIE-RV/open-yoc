/*
Copyright (C) 2022 Alibaba Group Holding Limited
*/

#ifndef _LY_KWS_H
#define _LY_KWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "ly_kws_reg.h"

// 算法可能返回的错误
typedef enum kws_error {
     AIE_OK = 0,
     AIE_NO_REGISTER = -1,
     AIE_NO_INIT = -2,
     AIE_WRONG_PARAMS = -3,     
}kws_error_t; 

// 用户可调用相应API得到相应模型的配置信息，检查应用参数是否满足要求，后续可添加其他信息
typedef struct kws_info {
	int16_t sample_rate;
} kws_info_t;

/**
 * 功能：获取模型支持的一些参数信息
 * 参数 handle_t：模型句柄
 * 参数 info：该模型支持的相关信息
 * 返回值：错误码
*/
kws_error_t aie_get_kws_info(kws_handle_t handle, kws_info_t *info);

/**
 * 功能：对模型的资源进行初始化
 * 参数 handle：模型句柄
 * 参数 channels：采集的语音数据通道数
 * 参数 points_per_frame：一帧语音数据采样的总点数
 * 参数 model_cal_res_dim：模型输出数据的维度
 * 返回值：错误码
*/
kws_error_t aie_kws_init(kws_handle_t handle, int channels, size_t * points_per_frame, size_t *model_cal_res_dim);

/**
 * 功能：特征提取、前向计算
 * 参数 handle：模型句柄
 * 参数 pcm_data：语音数据，非交织
 * 参数 points_per_frame：一帧语音数据采样的总点数
 * 参数 model_cal_res：模型处理语音数据的输出结果
 * 参数 model_cal_res_dim：模型输出数据的维度
 * 返回值：错误码
*/
kws_error_t aie_kws_run(kws_handle_t handle, int16_t *pcm_data, size_t points_per_frame, 
                        void *model_cal_res, size_t model_cal_res_dim);

/**
 * 功能：后处理，选择最可能的唤醒词/命令词
 * 参数 handle：模型句柄
 * 参数 model_cal_res：模型处理语音数据的输出结果
 * 参数 output_data：唤醒结果，下标0为唤醒词id，下标1为唤醒分数
 * 返回值：错误码
*/
kws_error_t aie_kws_postprocess(kws_handle_t handle, void *model_cal_res,  int (*output_data)[2]);

/**
 * 功能：获取唤醒后的唤醒词/命令词拼音
 * 参数 handle：模型句柄
 * 参数 word_id：唤醒词/命令词id
 * 参数 kws_word：根据id得到的唤醒词/命令词
 * 返回值：错误码
*/
kws_error_t aie_kws_get_word(void *handle, int word_id, char **kws_word);

#ifdef __cplusplus
}
#endif

#endif