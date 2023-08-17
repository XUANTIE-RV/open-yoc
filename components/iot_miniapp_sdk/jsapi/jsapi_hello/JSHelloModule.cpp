#include "jqutil_v2/jqutil.h"
#include "jsmodules/JSCModuleExtension.h"
#include "jquick_config.h"

using namespace jqutil_v2;

namespace hello {

extern int foo_init(JQModuleEnv* env);

static std::vector<std::string> exportList = {
    "foo", "fooWifi"
};

static int module_init(JSContext *ctx, JSModuleDef *m) {
    JQuick::sp<JQModuleEnv> env = JQModuleEnv::CreateModule(ctx, m, "hello");

    foo_init(env.get());

    env->setModuleExportDone(JS_UNDEFINED, exportList);
    return 0;
}

DEF_MODULE_LOAD_FUNC_EXPORT(hello, module_init, exportList)

}  // namespace hello

void register_hello_jsapi()
{
    registerCModuleLoader("hello", &hello::hello_module_load);
}

