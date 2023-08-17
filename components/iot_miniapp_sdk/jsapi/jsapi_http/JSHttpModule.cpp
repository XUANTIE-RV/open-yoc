#include "jqutil_v2/jqutil.h"
#include "jsmodules/JSCModuleExtension.h"
#include "jquick_config.h"

using namespace jqutil_v2;
namespace aiot {
// 模块被使用(加载)时调用
extern JSValue init_aiot_http(JQModuleEnv* env);
static int module_init(JSContext *ctx, JSModuleDef *m) {
    JQuick::sp<JQModuleEnv> env = JQModuleEnv::CreateModule(ctx, m, "http");
    JSValue module = init_aiot_http(env.get());
    env->setModuleExportDone(module);
    return 0;
}
DEF_MODULE_LOAD_FUNC(http, module_init)
}
// 系统启动时调用，决定是否将该模块注册到 JS 空间
extern "C" void register_aiot_http_jsapi()
{
    registerCModuleLoader("http", &aiot::http_module_load);
}
