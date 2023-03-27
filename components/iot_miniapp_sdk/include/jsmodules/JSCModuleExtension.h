/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_JSCMODULEXTENSION_H___
#define ___JQUICK_JSCMODULEXTENSION_H___

#include "quickjs/quickjs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef JSModuleDef* (*LoadCModuleFunction)(JSContext* context, const char* moduleName);
/* 往js loop中发送消息的回调函数 */
typedef void (*CJSMessageFunction)(void* context, int argc, char* argv[]);

/**
 * 注册qjs扩展模块，应用代码在import该模块的时候，会调用loader函数动态加载
 * loader函数返回的是一个quickjs的模块定义
 * @param moduleName
 * @param loader
 */
void registerCModuleLoader(const char* moduleName, LoadCModuleFunction loader);

/**
 * 往js loop中发送消息，主要用于其他线程往js线程执行callback
 * (因为js函数不能跨线程调用用，所以一些后台任务的js回调，需要post到js线程来执行)
 * @param func
 * @param context
 * @param argc
 * @param argv
 * @return
 */
int postJSMessage(CJSMessageFunction func, void* context, int argc, char* argv[]);

/**
 * 打印quickjs的异常对象
 * @param ctx
 * @param v
 */
void dumpQJSValue(JSContext* ctx, JSValueConst v);
/**
 * 打印quickjs的异常信息
 * @param ctx
 */
void dumpQJSException(JSContext* ctx);

#ifdef __cplusplus
}
#endif

#endif  //___JQUICK_JSCMODULEXTENSION_H___
