/**
 * @file ly_preprocess.h
 * @author Liang Shui (Gao Hao)
 * @brief 前处理算法接口
 * @date 2023-05-16
 * 
 * Copyright (C) 2022 Alibaba Group Holding Limited
 * 
 */


#ifndef _LY_PREPROCESS_H
#define _LY_PREPROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "alg_register.h"

typedef enum {
     PREPROCESS_OK           = 0,
     PREPROCESS_NO_REGISTER  = 1,
     PREPROCESS_NO_INIT      = 2,
     PREPROCESS_WRONG_PARAMS = 3,
     PREPROCESS_MEM_NOT_INIT = 4,
}pre_error_t; 

/**
 * @brief vad函数初始化
 * 
 * @param handle vad句柄
 * @param vad_time vad判断的累计时间，例如50，则vad会累计判断500ms时间内的数据
 * @param vad_mode vad模式。取值范围0-4，取值越小vad越灵敏
 * @param sample_rate 采样频率，系统采样频率必须为16k
 * @return pre_error_t  返回错误码
 */
pre_error_t aie_vad_init(vad_handle *handle, int vad_time, int vad_mode, int sample_rate);
pre_error_t aie_nnvad_init(vad_handle *handle, int vad_time, int vad_mode, int sample_rate);
/**
 * @brief vad执行函数
 * 
 * @param handle vad句柄
 * @param pcm_data 语音数据，要求长度为10ms
 * @param points_per_frame 输入语音数据的采样点数，16k频率，10ms应有160个采样点
 * @param vad_out vad输出值，0表示静音，1表示有声音
 * @return pre_error_t  返回错误码
 */
pre_error_t aie_vad_run(vad_handle *handle, int16_t *pcm_data,  int points_per_frame, int16_t *vad_out);
pre_error_t aie_nnvad_run(vad_handle* handle, float* feature, int points_per_frame, int16_t *vad_out, int is_start);

/**
 * @brief vad复位函数，该函数会清空句柄中的所有数据至初始化状态 
 * 
 * @param handle vad句柄
 * @return pre_error_t 返回错误码
 */
pre_error_t aie_vad_reset(vad_handle *handle);
pre_error_t aie_nnvad_reset(vad_handle *handle);
/**
 * @brief vad释放函数，一般不需要调用
 * 
 * @param handle vad句柄
 * @return pre_error_t 返回错误码
 */
pre_error_t aie_vad_free(vad_handle *handle);

/**
 * @brief agc初始化函数
 * 
 * @param handle agc句柄
 * @param sample_rate 采样频率，系统采样频率必须为16k
 * @param agc_mode agc模式。0 agc自适应模拟增益，
 *                          1 agc自适应数字增益，
 *                          2 agc固定数字增益。
 * @param gain agc预计获得的增益(dB)，自适应增益模式下算法对最终增益会有调整。
 * @return pre_error_t 返回错误码
 */
pre_error_t aie_agc_init(agc_handle *handle, int16_t sample_rate, int16_t agc_mode, int16_t gain);

/**
 * @brief gac执行函数
 * 
 * @param handle agc句柄
 * @param pcm_data 语音数据，要求长度为10ms
 * @param points_per_frame 输入语音数据的采样点数，16k频率，10ms应有160个采样点
 * @param agc_out 增益后的语音数据
 * @return pre_error_t 返回错误码
 */
pre_error_t aie_agc_run(agc_handle *handle, int16_t *pcm_data, int32_t points_per_frame, int16_t *agc_out);

/**
 * @brief agc复位函数，该函数会清空句柄中的所有数据至初始化状态
 *        agc运行过程中一般不需要复位
 * 
 * @param handle agc句柄
 * @return pre_error_t 返回错误码
 */
pre_error_t aie_agc_reset(agc_handle *handle);

/**
 * @brief agc释放函数，一般不需要调用
 * 
 * @param handle agc句柄
 * @return pre_error_t 返回错误码
 */
pre_error_t aie_agc_free(agc_handle *handle);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // _LY_PREPROCESS_H