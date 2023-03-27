#pragma once
#include "jqutil_v2/bson.h"
#include "looper/Task.h"
#include "quickjs/quickjs.h"
#include <functional>
#include <vector>

#ifndef offsetof
#define offsetof(type, field) ((size_t) &((type *)0)->field)
#endif
#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

namespace jqutil_v2 {

void jq_dump_error(JSContext* ctx);
int jq_eval_binary_with_exc(JSContext* ctx, const uint8_t* buf, size_t buf_len);

/* set the new value and free the old value after (freeing the value
   can reallocate the object data) */
inline void jq_set_value(JSContext *ctx, JSValue *pval, JSValue new_val)
{
    JSValue old_val;
    old_val = *pval;
    *pval = new_val;
    JS_FreeValue(ctx, old_val);
}

JSValue jq_call_return(JSContext* ctx, JSValueConst func, const std::vector<JSValueConst> &args={}, JSValueConst this_val=JS_UNDEFINED);
void jq_call_void(JSContext *ctx, JSValueConst func, const std::vector<JSValueConst> &args={}, JSValueConst this_val=JS_UNDEFINED);
JSValue jq_bind_obj_func(JSContext *ctx, JSValueConst obj, const std::string &funcName);

JSValue bsonToJSValue(JSContext *ctx, const Bson& bson);
Bson JSValueToBson(JSContext *ctx, JSValueConst val);

int jq_vsnprintf(char *outBuf, size_t size, const char* format, va_list ap);
void jq_printf_va(std::string &buf, const char* fmt, va_list ap);
std::string jq_printf(const char* fmt, ...);

class JQStdFuncTask : public JQuick::Task
{
public:
    JQStdFuncTask(std::function<void()> func);
    virtual ~JQStdFuncTask();
    virtual void run();

private:
    std::function<void()> _func;
};

#define DEF_MODULE_LOAD_FUNC(NAME, INIT_FUNC) \
static JSModuleDef* NAME##_module_load(JSContext *ctx, const char *moduleName) { \
    if (strcmp(moduleName, #NAME) == 0) { \
        JSModuleDef *m = JS_NewCModule(ctx, moduleName, INIT_FUNC); \
        if (!m) return NULL; \
        JS_AddModuleExport(ctx, m, "default"); \
        return m; \
    } \
    return NULL; \
}

#define DEF_MODULE_LOAD_FUNC_EXPORT(NAME, INIT_FUNC, EXPORT_LIST) \
static JSModuleDef* NAME##_module_load(JSContext *ctx, const char *moduleName) { \
    if (strcmp(moduleName, #NAME) == 0) { \
        JSModuleDef *m = JS_NewCModule(ctx, moduleName, INIT_FUNC); \
        if (!m) return NULL; \
        JS_AddModuleExport(ctx, m, "default"); \
        for (auto &item: EXPORT_LIST) JS_AddModuleExport(ctx, m, item.c_str()); \
        return m; \
    } \
    return NULL; \
}

}  // namespace jqutil_v2

#define DEF_MODULE_EXPORT_FUNC(NAME) \
extern "C" JQUICK_EXPORT void custom_init_jsapis() { \
    registerCModuleLoader(#NAME, &NAME##_module_load); \
}

#define DEF_MODULE(NAME, INIT_FUNC) \
    DEF_MODULE_LOAD_FUNC(NAME, INIT_FUNC) \
    DEF_MODULE_EXPORT_FUNC(NAME)
