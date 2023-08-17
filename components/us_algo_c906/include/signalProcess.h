/**
 * Copyright (C) 2023 Unisound.inc, All rights reserved.
 *
 * @file signalProcess.h
 * @brief 信号处理算法头文件
 * @version 0.1
 */

#ifndef __UNISOUND__SIGNALPROCESS_API_H__
#define __UNISOUND__SIGNALPROCESS_API_H__

#if (!(defined SGP_CALL) || !(defined SGP_IMPORT_OR_EXPORT))
#    if defined __WIN32__ || defined _WIN32 || defined _WIN64
#       define SGP_CALL __stdcall
#       ifdef  SGP_IMPLEMENTION
#           define SGP_IMPORT_OR_EXPORT __declspec(dllexport)
#       else
#           define SGP_IMPORT_OR_EXPORT __declspec(dllimport)
#       endif
#    elif defined __ANDROID__
#       define SGP_CALL
#       define SGP_IMPORT_OR_EXPORT __attribute ((visibility("default")))
#    else
#       define SGP_CALL
#       define SGP_IMPORT_OR_EXPORT __attribute ((visibility("default")))
#    endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 函数描述：创建信号处理引擎
 * 参数：
 * ccfg_fn: 扩展使用，目前传NULL。
 * 是否同步：同步接口
 * 返回值：成功：返回创建的引擎指针，后续函数已该值为第一个参数；失败：返回NULL 
 */
SGP_IMPORT_OR_EXPORT void *SGP_CALL sgp_new(const char *cfg_fn);

/*
 * 函数描述：销毁信号处理引擎
 * 参数：
 *     engine：信号引擎句柄
 * 
 * 是否同步：同步接口
 * 返回值：成功：0； 失败：非0
 */
SGP_IMPORT_OR_EXPORT int SGP_CALL sgp_delete(void *engine);

/*
 * 函数描述：开始信号处理
 * 参数：
     engine：信号引擎句柄
 * 是否同步：同步接口
 * 返回值：成功：0； 失败：非0
 */
SGP_IMPORT_OR_EXPORT int SGP_CALL sgp_start(void *engine);

/*
 * 函数描述：信号处理引擎feed数据
 * 参数：
 *    engine：信号引擎句柄
 * 	  data：音频数据
 *    size：音频数据大小
 * 是否同步：同步接口
 *返回值：成功：0；失败：非0
 */
SGP_IMPORT_OR_EXPORT int SGP_CALL sgp_feed(void *engine, const char *data, int size);

/*
 * 函数描述：结束信号处理
 * 参数描述：
     engine：信号引擎句柄
 * 是否同步：同步接口
 * 返回值：成功：0； 失败：非0
 */
SGP_IMPORT_OR_EXPORT int SGP_CALL sgp_stop(void *engine);

/*
 * 获取信号处理版本信息
 * engine：信号处理句柄
 * 同步接口
 */
SGP_IMPORT_OR_EXPORT const char *SGP_CALL sgp_version();

/**基础接口 - 必须实现
 * 信号处理回调函数
 * usr_ptr：回调函数数据
 * data：信号处理后的数据
 * size： 信号处理后的数据大小
 */
typedef int (*SyncSgpResultHandler)(void *usr_ptr, const char *data, int size);

/**基础接口 - 必须实现
 * 说明：注册信号处理数据回调函数，通过该回调函数获取信号处理函数。
 * engine：信号处理引擎。
 * usr_data：回调函数的参数。
 * func: 外部传入的回调处理函数。
 * 同步接口
 */
SGP_IMPORT_OR_EXPORT int SGP_CALL sgp_register_processed_voice(void *engine, void *usr_data, SyncSgpResultHandler func);


#ifdef __cplusplus
}
#endif
#endif

