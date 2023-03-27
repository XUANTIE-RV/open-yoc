/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_JQUICKCONTEXT_H___
#define ___JQUICK_JQUICKCONTEXT_H___

#include "quickjs/quickjs.h"
#include "looper/Handler.h"
#include <string>
#include <map>

namespace JQuick
{
class JQuickJSRuntime
{
public:
    JSRuntime* _runtime;
    JQuick::sp<JQuick::Handler> _handler;
    bool _is_worker;
    void* _opaque;
};
class JQuickJSContext
{
public:
    JSContext* _context;
    std::string _appid;
    void* _opaque;
};
bool is_main_jsthread(JSRuntime*);
/**
 * 获取当前运行的JS线程Looper
 */
JQuick::sp<JQuick::Looper> getJSLooper(JSRuntime*);
JQuickJSRuntime* getJQuickJSRuntime(JSRuntime*);

bool is_main_jsthread(JSContext*);
/**
 * 获取当前运行的JS线程Looper
 */
JQuick::sp<JQuick::Looper> getJSLooper(JSContext*);
JQuickJSContext* getJQuickJSContext(JSContext*);

/**
 * 获取当前运行的应用工作路径
 */
std::string getWorkspace(JSContext*);
/**
 * 获取当前运行的应用id
 */
std::string getAppid(JSContext*);
/**
 * 获取当前运行的应用数据缓存路径
 */
std::string getDataDir(JSContext*);
/**
 * 获取应用数据缓存路径
 */
std::string getDataDir(const std::string& appid);

/**
 * 根据appId启动对应的app.
 * 如果app已经启动，会被拉到前台，并传递新的options
 */
void startApp(const std::string& appId,
              const std::string& pageName,
              const std::map< std::string, std::string >& options);
/**
 * 根据appId启动对应的app服务.
 */
void startAppService(const std::string& appId,
              const std::string& serviceName,
              const std::map< std::string, std::string >& options);
/**
 * 退出应用
 * @param appId
 */
void closeApp(const std::string& appId);

/**
 * 获取当前运行于前台的app（软键盘、下拉栏等全局app除外）
 * @return
 */
std::string getTopApp();

/**
 * 当前是否运行在main线程
 * @return
 */
bool is_main_process();

/**
 * 翻转屏幕（旋转180度）
 * (注意: 内部实现没有持久化功能，需要外部模块做持久化)
 * @param flip
 */
void setScreenFlip(bool flip);

/**
 * App生命周期回调
 */
enum LifecycleState
{
    LC_STATE_CREATED = 1,      // 应用创建
    LC_STATE_RESUMED = 2,      // 应用前台可见
    LC_STATE_PAUSED = 3,       // 应用不可见
    LC_STATE_DESTROYED = 4,    // 应用销毁
};
/**
 * 需注意回调中不应包含耗时操作（如有耗时操作，可自行创建线程执行）
 */
typedef void (*AppLifecycleCallback)(LifecycleState state, const std::string& appid);
void registerAppLifecycle(AppLifecycleCallback cb);
void unregisterAppLifecycle(AppLifecycleCallback cb);

std::string exchangeAppidIfNeeded(const std::string& appidOrCategory);
}
#endif  //___JQUICK_JQUICKCONTEXT_H___
