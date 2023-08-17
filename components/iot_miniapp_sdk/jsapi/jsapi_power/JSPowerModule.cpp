#include "jqutil_v2/jqutil.h"
#include "jsmodules/JSCModuleExtension.h"
/**
 * 电源 模块
 */

using namespace jqutil_v2;
namespace aiot {
// 模块被使用(加载)时调用
extern JSValue init_aiot_power(JQModuleEnv* env);
static int module_init(JSContext *ctx, JSModuleDef *m) {
    JQuick::sp<JQModuleEnv> env = JQModuleEnv::CreateModule(ctx, m, "power");
    JSValue module = init_aiot_power(env.get());
    env->setModuleExportDone(module);
    return 0;
}
DEF_MODULE_LOAD_FUNC(power, module_init)
}
// 系统启动时调用，决定是否将该模块注册到 JS 空间
extern "C" void register_aiot_power_jsapi()
{
    registerCModuleLoader("power", &aiot::power_module_load);
}
