#include "jqutil_v2/jqutil.h"
#include "jsmodules/JSCModuleExtension.h"
/**
 * KV 模块
 */

using namespace jqutil_v2;
namespace aiot {
// 模块被使用(加载)时调用
extern JSValue init_aiot_system_kv(JQModuleEnv* env);
static int module_init(JSContext *ctx, JSModuleDef *m) {
    JQuick::sp<JQModuleEnv> env = JQModuleEnv::CreateModule(ctx, m, "system_kv");
    JSValue module = init_aiot_system_kv(env.get());
    env->setModuleExportDone(module);
    return 0;
}
DEF_MODULE_LOAD_FUNC(system_kv, module_init)
}
// 系统启动时调用，决定是否将该模块注册到 JS 空间
extern "C" void register_aiot_system_kv_jsapi()
{
    registerCModuleLoader("system_kv", &aiot::system_kv_module_load);
}
//
// Created by zypbelie on 2022/12/6.
//
