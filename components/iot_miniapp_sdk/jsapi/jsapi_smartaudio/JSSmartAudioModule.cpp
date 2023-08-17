#include "jqutil_v2/jqutil.h"
#include "jsmodules/JSCModuleExtension.h"
/**
 * 音频播放 模块
 */

using namespace jqutil_v2;
namespace aiot {
// 模块被使用(加载)时调用
extern JSValue init_aiot_smta(JQModuleEnv* env);
static int module_init(JSContext *ctx, JSModuleDef *m) {
    JQuick::sp<JQModuleEnv> env = JQModuleEnv::CreateModule(ctx, m, "smta");
    JSValue module = init_aiot_smta(env.get());
    env->setModuleExportDone(module);
    return 0;
}
DEF_MODULE_LOAD_FUNC(smta, module_init)
}
// 系统启动时调用，决定是否将该模块注册到 JS 空间
extern "C" void register_aiot_smta_jsapi()
{
    registerCModuleLoader("smta", &aiot::smta_module_load);
}
//
// Created by houge on 2022/12/13.
//
