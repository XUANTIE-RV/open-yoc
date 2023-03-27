/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#pragma once
#include <string>
#include <stdio.h>
#include <map>

namespace ariver
{
namespace iot
{
enum APIStatus {
    API_Success = 0,
    API_NotSupport = 1,  // 无效的api调用
    API_ParamError = 2,  // 无效的参数
    API_Failed = 3,
    API_NoPermission = 4,
    API_Timeout,
    API_UserDenied,
    API_UserCancelled = 11,
};

class ExtensionProxyBase
{
public:
    typedef void (*InvokeFunction)(const std::string& callbackid, const std::string& params);

    /**
     * @deprecated
     * 后续会废弃，使用REGISTER_MODULE_JSAPI替代
     *
     * 业务方扩展JSAPI使用该接口注册
     * func: jsapi具体的实现函数
     * label_name: 映射到js端的函数名称，如$falcon.jsapi.alert label_name=alert
     */
#define REGISTER_STD_JSAPI(func, label_name) \
    registerJSApi(func, #label_name);

    /**
     * 业务方扩展JSAPI使用该接口注册
     * func: jsapi具体的实现函数
     * module: 模块名，映射到js端的模块名称，如$falcon.jsapi.module
     * label_name: 映射到js端的函数名称，如$falcon.jsapi.module.alert label_name=alert
     */
#define REGISTER_JSAPI(func, module, label_name) \
    registerJSApi(func, #module, #label_name);

    /**
     * @deprecated
     */
    virtual void registerJSApi(InvokeFunction func, const std::string& labelName) = 0;
    /**
     * 业务方扩展JSAPI使用该接口注册
     * func: jsapi具体的实现函数
     * module: 模块名，映射到js端的模块名称，如$falcon.jsapi.module
     * label_name: 映射到js端的函数名称，如$falcon.jsapi.module.alert label_name=alert
     */
    virtual void registerJSApi(InvokeFunction func, const std::string& module, const std::string& labelName) = 0;
    /**
     *  Jsapi回调函数
     *  callbackId: 如容器侧未下传callbackid，需要在jsapi extension实现侧自行生成
     */
    virtual void postCallback(const std::string& callbackid, APIStatus status, const std::string& params) = 0;
    /**
     *  Jsapi回调函数，支持传递buf数据到js
     *  callbackId: 如容器侧未下传callbackid，需要在jsapi extension实现侧自行生成
     *  buf: 传给前端的buf数据
     *  buf_size: buf数据长度
     *  shared: 是否复制(如果是false，容器会fork一份buf使用，外部自己管理buf的释放
     *      否则由容器内部管理buf的释放，外部不要重复释放)
     */
    virtual void postCallback(const std::string& callbackid, APIStatus status, const std::string& params, const char* buf, size_t buf_size, bool shared) = 0;

    /**
     *  容器上行发送自定义事件的接口
     *  js端使用 $falcon.on("customEvent", (param)=>{}) 注册
     *  customEvent: 事件名称
     *  param: 参数
     */
    virtual void sendCustomEvent(const std::string& customEvent, const std::string& param) = 0;

    /**
     *  容器上行发送自定义事件的接口
     *  js端使用方法
     *  import module from "$jsapi/moduleName"
     *  module.on("customEvent", (param)=>{})
     *  进行注册
     *
     *  module: 模块名称
     *  customEvent: 事件名称
     *  param: 参数
     */
    virtual void sendCustomEvent(const std::string& module, const std::string& customEvent, const std::string& param) = 0;

    /**
     * 向某个app发送全局event事件
     */
    virtual void sendAppEvent(const std::string& appId, const std::string& customEvent, const std::string& param) = 0;
    /**
     * 向某个app发送全局event事件
     */
    virtual void sendAppEvent(const std::string& appId, const std::string& module, const std::string& customEvent, const std::string& param) = 0;
};

ExtensionProxyBase* getJSApiExtensionProxy();
}  // namespace iot
}  // namespace ariver
