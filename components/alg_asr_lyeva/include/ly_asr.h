/**
 * @file ly_asr.h
 * @author Liang Shui (Gao Hao)
 * @brief ASR header file
 * @date 2022-12-12
 *
 * Copyright (C) 2022 Alibaba Group Holding Limited
 *
 */

#ifndef _LY_ASR_H
#define _LY_ASR_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdint.h>
#include <stddef.h>
#include "alg_register.h"

// 用户可调用相应API得到相应模型的配置信息，检查应用参数是否满足要求，后续可添加其他信息
typedef struct asr_info
{
     int16_t sample_rate; // 采样率，系统应满足16k采样率
     int decoding_window; // 窗长
     int stride;          // 窗移
} asr_info_t;

// 算法可能返回的错误
typedef enum asr_error
{
     ASR_OK = 0,
     ASR_NO_REGISTER = 1,
     ASR_NO_INIT = 2,
     ASR_WRONG_PARAMS = 3,
     ASR_TEXT_LEN_OUT = 4,
     ASR_MEM_NO_INIT = 5,
} asr_error_t;

// asr初始化的输入参数
typedef struct asr_init_params {
     int channels;
} asr_init_params_t;

/**
 * @brief 获取asr算法需要的配置信息
 *
 * @param handle asr句柄
 * @param info asr配置信息结构体
 * @return asr_error_t asr错误码
 */
asr_error_t aie_get_asr_info(asr_wenet_handle *handle, asr_info_t *info);

/**
 * @brief asr初始化函数
 *
 * @param handle asr句柄
 * @param init_params 初始化参数
 * @param points_per_frame 一帧采样数据的总点数。16k采样频率下，一帧数据为10ms，共160个采样点
 * @return asr_error_t asr错误码
 */
asr_error_t aie_asr_init(asr_wenet_handle *handle, asr_init_params_t init_params,
                         size_t *points_per_frame);

/**
 * @brief asr语音特征提取，特征存储在handle的缓存中
 *
 * @param handle asr句柄
 * @param pcm_data 语音数据，长度应为10ms
 * @param points_per_frame 一帧采样数据的总点数。16k采样频率下，一帧数据为10ms，共160个采样点
 * @return asr_error_t asr错误码
 */
asr_error_t aie_asr_feature_compute(void *handle, int16_t *pcm_data, size_t points_per_frame, float** feature);

/**
 * @brief asr执行函数，模型对语音数据的处理结果存储在句柄中
 *
 * @param handle asr句柄
 * @return asr_error_t asr错误码
 */
asr_error_t aie_asr_run(void *handle);

/**
 * @brief asr执行函数，该函数包含vad功能，可用于判断vad结束
 *
 * @param handle asr句柄
 * @param offset encoder部分位置编码的偏移
 * @param top_index ctc的部分输出结果
 * @return asr_error_t
 */
asr_error_t aie_asr_run_as_vad(void *handle, int *offset, int **top_index);

#ifndef DNO_DECODER
/**
 * @brief asr重打分函数
 *
 * @param handle asr句柄
 * @param asr_out asr识别结果
 * @param sentence_len asr识别结果中的汉字数量
 * @return asr_error_t  asr错误码
 */
asr_error_t aie_asr_search_and_rescoring(asr_wenet_handle *handle, char **asr_out,
                                             size_t *sentence_len);
#endif

// 该函数用于没有重打分模型的情况，可选
asr_error_t aie_asr_ctc_scoring(asr_wenet_handle *handle, char **asr_out, size_t *sentence_len);

/**
 * @brief asr复位函数，清空asr模型中所有的数据缓存
 *
 * @param handle asr句柄
 * @return asr_error_t asr错误码
 */
asr_error_t aie_asr_reset(asr_wenet_handle *handle);

/**
 * @brief asr复位函数，仅配合aie_asr_run_as_vad使用
 *        encoder_conf->offset = 0;
 *        encoder_conf->dynamic0 = 0;
 *        ctc_conf->encoder_out_tensor->dim[1] = 0;
 *        ctc_conf->ctc_out_tensor->dim[1] = 0;
 *
 * @param handle asr句柄
 * @return asr_error_t asr错误码
 */
asr_error_t aie_asr_reset_remain_caches(void *handle);

#ifdef __cplusplus
}
#endif // _cpluscplus

#endif // _LY_ASR_H
