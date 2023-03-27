/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_QJSMODULEXTENSION_H___
#define ___JQUICK_QJSMODULEXTENSION_H___

#include "jsmodules/JSCModuleExtension.h"
#include <string>
#include <map>

namespace falcon
{
class QJSModuleExtension
{
public:
    typedef JSModuleDef* (*LoadModuleFunction)(JSContext *context, const std::string &moduleName);
    static QJSModuleExtension* singleton();

    /**
     * 注册qjs扩展模块，应用代码在import该模块的时候，会调用loader函数动态加载
     * loader函数返回的是一个quickjs的模块定义
     * @param moduleName
     * @param loader
     */
    void registerModuleLoader(const std::string &moduleName, LoadModuleFunction loader);
    /**
     * 注册qjs扩展模块，应用代码在import该模块的时候，会调用loader函数动态加载
     * loader函数返回的是一个quickjs的模块定义
     * @param moduleName
     * @param loader
     */
    void registerModuleLoader(const std::string &moduleName, LoadCModuleFunction loader);
    /**
     * 注册qjs扩展模块，应用代码在import该模块的时候，会调用loader函数动态加载
     * loader函数返回的是一个quickjs的模块定义
     * @param appid
     * @param moduleName
     * @param loader
     */
    void registerModuleLoader(const std::string& appid, const std::string &moduleName, LoadModuleFunction loader);
    /**
     * 往js loop中发送消息，主要用于其他线程往js线程执行callback
     * (因为js函数不能跨线程调用用，所以一些后台任务的js回调，需要post到js线程来执行)
     * @param func
     * @param context
     * @param argc
     * @param argv
     * @return
     */
    bool postMessage(CJSMessageFunction func, void* context, int argc, char* argv[]);

private:
    QJSModuleExtension();
};
}

#endif  //___JQUICK_QJSMODULEXTENSION_H___
