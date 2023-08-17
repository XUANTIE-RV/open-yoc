/**
 * Copyright (C) 2023 Unisound.inc, All rights reserved.
 *
 * @file wakeup_sync_api_new.h
 * @brief 唤醒算法头文件
 */

#ifndef __UNISOUND__WAKEUP_API_WAKEUP_SYNC_API_H__
#define __UNISOUND__WAKEUP_API_WAKEUP_SYNC_API_H__


#if (!(defined XENGINE_CALL) || !(defined XENGINE_IMPORT_OR_EXPORT))
#    if defined __WIN32__ || defined _WIN32 || defined _WIN64
#       define XENGINE_CALL __stdcall
#       ifdef  XENGINE_IMPLEMENTION
#           define XENGINE_IMPORT_OR_EXPORT __declspec(dllexport)
#       else
#           define XENGINE_IMPORT_OR_EXPORT __declspec(dllimport)
#       endif
#    elif defined __ANDROID__
#       define XENGINE_CALL
#       define XENGINE_IMPORT_OR_EXPORT __attribute ((visibility("default")))
#    else
#       define XENGINE_CALL
#       define XENGINE_IMPORT_OR_EXPORT __attribute ((visibility("default")))
#    endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
 * 函数描述：创建唤醒引擎
 * 返回值：成功：指向引擎的指针；失败：NULL
 */
XENGINE_IMPORT_OR_EXPORT void *XENGINE_CALL wku_new(void);


/**基础接口 - 必须实现
 * 函数描述：销毁唤醒引擎
 * 参数描述：
 *		engine：唤醒句柄
 * 是否同步：同步接口
 * 返回值：成功：0；失败: 非0
 */
XENGINE_IMPORT_OR_EXPORT int XENGINE_CALL wku_delete(void *engine);



/*
 *函数描述：唤醒回调函数声明，算法回调该接口回传唤醒结果。
 *参数描述：
 * 		usr_ptr：对应wku_start API中的第四个参数wku_usr
 * 		json：唤醒json串，具体key-value值见下面
 * 		bytes： 唤醒数据大小
 * 第二个参数Json格式：
 * {"wakeup_words" : "天猫精灵","score" : "2.0"}
 *
 * score : 唤醒score
 * wakeup_words : 唤醒词
*/
typedef int (*SyncWkuResultHandler)(void *usr_ptr, const char *json, int bytes);


/*
 * 函数描述：开始唤醒引擎(sgp处理完原始语音数据后会自动往唤醒引擎送数据)
 * 参数描述：
 * 		engine：唤醒句柄
 * 		wku_usr： 调用传入的一个参数指针，引擎回调函数SyncWkuResultHandler时作为其第一个参数(usr_ptr)
 * 		wku_func： 唤醒回调函数
 * 是否同步：同步接口
 * 返回值：成功：0；失败：非0
 */
XENGINE_IMPORT_OR_EXPORT int XENGINE_CALL wku_start(void *engine, SyncWkuResultHandler wku_func, void *wku_usr);


/*
 * 函数描述：结束唤醒引擎
 * 函数参数：
 *		engine：唤醒引擎句柄
 * 是否同步：同步接口
 * 返回值：成功：0；失败：非0
 */
XENGINE_IMPORT_OR_EXPORT int XENGINE_CALL wku_stop(void *engine);


/*
 * 函数描述：获取唤醒引擎版本号
 * 参数描述：
 *		engine：唤醒引擎句柄
 * 是否同步：同步接口
 * 返回值：版本信息
 */
XENGINE_IMPORT_OR_EXPORT const char *XENGINE_CALL wku_version();


#ifdef __cplusplus
}
#endif
#endif