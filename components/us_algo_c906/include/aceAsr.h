/**
 * Copyright (C) 2023 Unisound, All rights reserved.
 *
 * @file   aceAsr.h
 * @brief  
 * @author jiangchuang@unisound.com
 * @date   2023/03/18
 */

#ifndef __UNISOUND__API__TGENGINEAPI_H__
#define __UNISOUND__API__TGENGINEAPI_H__

#if (!(defined TGENGINE_CALL) || !(defined TGENGINE_IMPORT_OR_EXPORT))
#if defined __WIN32__ || defined _WIN32 || defined _WIN64
#define TGENGINE_CALL __stdcall
#ifdef TGENGINE_IMPLEMENTION
#define TGENGINE_IMPORT_OR_EXPORT __declspec(dllexport)
#else
#define TGENGINE_IMPORT_OR_EXPORT __declspec(dllimport)
#endif
#elif defined __ANDROID__
#define TGENGINE_CALL
#define TGENGINE_IMPORT_OR_EXPORT __attribute((visibility("default")))
#else
#define TGENGINE_CALL
#define TGENGINE_IMPORT_OR_EXPORT __attribute((visibility("default")))
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

typedef void* TGEngine;


/**
 * 识别结果回调函数
 * usr_ptr：回调函数数据
 * json：识别结果json字符串, {"cmd" : "天猫精灵","score" : "2.0"}
 * bytes： 识别结果字符串数据大小
 */
typedef int (*SyncAsrResultHandler)(void *usr_ptr, char *json, int bytes);


/**
 * 创建引擎
 * 返回算法引擎指针(如果local算法和唤醒等算法是一个引擎，可以返回对应的引擎指针)
 */
TGENGINE_IMPORT_OR_EXPORT TGEngine TGENGINE_CALL TGEngineCreate(void);

/**
 * 销毁引擎
 * engine：初始化的引擎指针
 * 同步接口
 */
TGENGINE_IMPORT_OR_EXPORT void TGENGINE_CALL TGEngineDestroy(TGEngine engine);



/** 
 * 启动引擎，开始识别, sgp处理完数据后会自动往识别引擎送数据
 * engine：初始化的识别引擎
 * 返回值: 0成功, 非0失败
 */
TGENGINE_IMPORT_OR_EXPORT int TGENGINE_CALL TGEngineStart(TGEngine engine, SyncAsrResultHandler rst_hdl, void *rst_usr_ptr);

/** 
 * 停止引擎
 * engine：初始化的识别引擎
 * 返回值: 0成功, 非0失败
 */
TGENGINE_IMPORT_OR_EXPORT int TGENGINE_CALL TGEngineStop(TGEngine engine);


#if defined(__cplusplus)
};
#endif // defined(__cplusplus)

#endif // __UNISOUND__API__TGENGINEAPI_H__