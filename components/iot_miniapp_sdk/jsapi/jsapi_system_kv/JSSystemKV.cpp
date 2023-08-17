#include "jqutil_v2/jqutil.h"
#include "JSSystemKVImpl.h"
#include  <ulog/ulog.h>

#define TAG "JSKV"

using namespace jqutil_v2;
namespace aiot
{
class JSSystemKV : public JQPublishObject
{
public:
    void setItem(JQAsyncInfo &info);
    void removeItem(JQAsyncInfo &info);
    void clear(JQAsyncInfo &info);

    void getItem(JQFunctionInfo &info);
private:
    JSSystemKVImpl _impl;
};

void JSSystemKV::setItem(JQAsyncInfo &info){
    std::string key = info[0].string_value();
    std::string value = info[1].string_value();
    if(key.empty()){
        info.postError("key should not be empty");
        return ;
    }
    if(value.empty()){
        info.postError("value should not be empty");
        return ;
    }
    _impl.setItem(key, value);
    info.post(key + ": " + value);
}

void JSSystemKV::getItem(JQFunctionInfo &info){
    JSContext *ctx = NULL;
    std::string key = JQString(ctx,info[0]).getString();
    std::string defaultValue="undefined";
    if(info.Length()>1){
        std::string defaultValue = JQString(ctx,info[1]).getString();
    }

    if(key.empty()){
        info.GetReturnValue().Set("undefined");
    }
    else{
        std::string value = _impl.getItem(key, defaultValue);
        info.GetReturnValue().Set(value);
    }
}

void JSSystemKV::removeItem(JQAsyncInfo &info){
    std::string key = info[0].string_value();
    if(key.empty()){
        info.postError("key should not be empty");
        return ;
    }
    _impl.removeItem(key);
    info.post(0);
}

void JSSystemKV::clear(JQAsyncInfo &info){
    _impl.clear();
    info.post(0);
}

static JSValue createJSSystemKV(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "system_kv");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
        return new JSSystemKV();
    });
       tpl->SetProtoMethodPromise("setItem", &JSSystemKV::setItem);
       tpl->SetProtoMethodPromise("removeItem", &JSSystemKV::removeItem);
       tpl->SetProtoMethodPromise("clear", &JSSystemKV::clear);

       tpl->SetProtoMethod("getItem", &JSSystemKV::getItem);

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_system_kv(JQModuleEnv* env)
{
    // 返回模块
    return createJSSystemKV(env);
}
}  // namespace aiot
