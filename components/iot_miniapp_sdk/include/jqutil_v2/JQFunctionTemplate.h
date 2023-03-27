#pragma once
#include "jqutil_v2/JQObjectTemplate.h"
#include "jqutil_v2/jqmisc.h"
#include "quickjs/quickjs.h"

namespace jqutil_v2 {

class JQFunctionTemplate;
using JQFunctionTemplateRef = JQRef< JQFunctionTemplate >;

class JQFunctionTemplate : public JQTemplate {
private:
    // bind to TgtClass.constructor, using "new TgtClass" to create instance or "TgtClass()" to call function
    static JSValue InstanceCtor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv,
                                int magic, JSValueConst *func_data);

    JQFunctionTemplate(JSContext *ctx, const std::string &name);
    JQFunctionTemplate(JQModuleEnv *env, const std::string &name);

public:
    static JQFunctionTemplateRef New(JQModuleEnv *env, const std::string &name);

    // create Function, which can be a TgtClass(Ctor) or Function
    JSValue GetFunction();

    JSValue CallConstructor(int argc=0, JSValueConst *argv=NULL);

    inline void setFunc(JQFunctionCallbackType func) { _func = func; }

    JQObjectTemplateRef InstanceTemplate();
    JQObjectTemplateRef PrototypeTemplate();

    // proto accessor
    template <typename T>
    inline JQObjectProperty& SetProtoAccessor(const char* prop,
                                 void(T::* getter)(JQFunctionInfo&),
                                 void(T::* setter)(JQFunctionInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->SetAccessor(prop1, [getter](JQFunctionInfo& info) {
          T* obj = info.UnwrapThis<T>();
          assert(obj != NULL);
          (obj->*getter)(info);
        }, [setter](JQFunctionInfo& info) {
          T* obj = info.UnwrapThis<T>();
          assert(obj != NULL);
          (obj->*setter)(info);
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    template <typename T>
    inline JQObjectProperty& SetProtoGetter(const char* prop,
                                 void(T::* getter)(JQFunctionInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->SetAccessor(prop1, [getter](JQFunctionInfo& info) {
            T* obj = info.UnwrapThis<T>();
            assert(obj != NULL);
            (obj->*getter)(info);
        }, [](JQFunctionInfo& info) {
            info.GetReturnValue().ThrowInternalError("getter only can not be set");
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    template <typename T>
    inline JQObjectProperty& SetProtoSetter(const char* prop,
                                 void(T::* setter)(JQFunctionInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->SetAccessor(prop1, [](JQFunctionInfo& info) {
            info.GetReturnValue().ThrowInternalError("setter only can not be get");
        }, [setter](JQFunctionInfo& info) {
            T* obj = info.UnwrapThis<T>();
            assert(obj != NULL);
            (obj->*setter)(info);
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    // support JQFunctionCallback of some class member functions
    template <typename T>
    inline JQObjectProperty& SetProtoMethod(const char* prop, void(T::* func)(JQFunctionInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_Set(prop1, [func](JQFunctionInfo& info) {
          T* obj = info.UnwrapThis<T>();
          assert(obj != NULL);
          (obj->*func)(info);
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    // async scheduled class member method
    template <typename T>
    inline JQObjectProperty& SetProtoMethodAsync(const char* prop, void(T::* func)(JQAsyncInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_SetAsync(prop1, [func](JQAsyncInfo& info) {
          // NOTE: run in module thread pool
          JQuick::sp<JQBaseObject> spRef = info._thisCppObj.promote();
          T* obj = static_cast<T*>(spRef.get());
          if (obj) {
              (obj->*func)(info);
          }
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    // promise scheduled class member method
    template <typename T>
    inline JQObjectProperty& SetProtoMethodPromise(const char* prop, void(T::* func)(JQAsyncInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_SetPromise(prop1, [func](JQAsyncInfo& info) {
          // NOTE: run in module thread pool
          JQuick::sp<JQBaseObject> spRef = info._thisCppObj.promote();
          T* obj = static_cast<T*>(spRef.get());
          if (obj) {
            (obj->*func)(info);
          }
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    // promise scheduled class member method
    template <typename T>
    inline JQObjectProperty& SetProtoMethodAsyncStd(const char* prop, void(T::* func)(JQAsyncInfo&))
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_SetAsyncStd(prop1, [func](JQAsyncInfo& info) {
          // NOTE: run in module thread pool
          JQuick::sp<JQBaseObject> spRef = info._thisCppObj.promote();
          T* obj = static_cast<T*>(spRef.get());
          if (obj) {
            (obj->*func)(info);
          }
        });
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    inline JQObjectProperty& SetProtoMethodAsync(const char* prop, JQAsyncCallbackType func)
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_SetAsync(prop1, func);
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    inline JQObjectProperty& SetProtoMethodPromise(const char* prop, JQAsyncCallbackType func)
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_SetPromise(prop1, func);
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    inline JQObjectProperty& SetProtoMethodAsyncStd(const char* prop, JQAsyncCallbackType func)
    {
        JSAtom prop1 = JS_NewAtom(_tplCtx, prop);
        JQObjectProperty& p = PrototypeTemplate()->_SetAsyncStd(prop1, func);
        JS_FreeAtom(_tplCtx, prop1);
        return p;
    }

    inline std::string name() const { return _name; }

    void setAsyncScheduleMode(int mode);
    void setAsyncScheduleHook(JQAsyncScheduleHook hook);

private:
    JQFunctionCallbackType _func;
    JQObjectTemplateRef _instanceTpl;
    JQObjectTemplateRef _prototypeTpl;
    std::string _name;
};

}  // namespace jqutil_v2