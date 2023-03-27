#pragma once
#include "jqutil_v2/JQBaseObject.h"
#include <map>
#include <string>

namespace jqutil_v2 {

class JQModuleEnv: public JQBaseObject {
public:
    JQModuleEnv(JSContext *ctx, JSModuleDef* def);
    virtual void OnGCCollect();
    virtual void OnGCMark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func);

    JSValueConst getFunction(const std::string &funcName);
    void registerFunction(const std::string &funcName, JSValue func);
    void unregisterFunction(const std::string &funcName);
    std::string appid() const;
    std::string dataDir() const;
    std::string name() const;
    JSContext* context() const;
    JSModuleDef* moduleDef() const;
    inline JQuick::sp<JQuick::Handler> jsHandler() const { return _handler; }

    void setModuleField(const std::string &name, JSValue obj);
    void setModuleExport(const std::string &name, JSValue obj, bool withFieldSet=true);
    void setModuleExportDone(JSValue module=JS_UNDEFINED, const std::vector<std::string> &exportList={});

    static JQuick::sp<JQModuleEnv> CreateModule(JSContext *ctx, JSModuleDef* def, const std::string &moduleName);

    template<typename T>
    JQuick::sp<T> as() { return static_cast<T*>(this); }

    void postJSThread(std::function<void()> func);
    void postNamedThread(const std::string &name, std::function<void()> func);

protected:
    std::map<std::string, JSValue> _nameFuncMap;
    std::string _appid;
    std::string _dataDir;
    JSContext *_ctx;
    JSModuleDef *_def;
    JQuick::sp<JQuick::Handler> _handler;

    // will be released when call setModuleExportDone, so do not need to mark gc
    std::map<std::string, JSValue> _collectModuleExportEntries;
};

}  // namespace jqutil_v2