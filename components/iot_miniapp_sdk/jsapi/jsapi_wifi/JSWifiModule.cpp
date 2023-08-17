#include "jqutil_v2/jqutil.h"
#include "jsmodules/JSCModuleExtension.h"
#include "jquick_config.h"

using namespace jqutil_v2;
namespace aiot {
// 模块被使用(加载)时调用
extern JSValue init_aiot_wifi(JQModuleEnv* env);
static int module_init(JSContext *ctx, JSModuleDef *m) {
    JQuick::sp<JQModuleEnv> env = JQModuleEnv::CreateModule(ctx, m, "wifi");
    JSValue module = init_aiot_wifi(env.get());
    env->setModuleExportDone(module);
    return 0;
}
DEF_MODULE_LOAD_FUNC(wifi, module_init)
}

// 系统启动时调用，决定是否将该模块注册到 JS 空间
extern "C" void register_aiot_wifi_jsapi()
{
    registerCModuleLoader("wifi", &aiot::wifi_module_load);
}
