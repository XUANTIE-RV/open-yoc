#pragma once
#include "quickjs/quickjs.h"
#include "utils/REF.h"

namespace jqutil_v2 {

// == JQRefCppSimple START ==
// only with REF UNREF feature
class JQRefCppSimple {
public:
    static JSValue NewJSValue(JSContext *ctx, JQuick::REF_BASE* cppObj);
    // Get a JQuick::REF_BASE* pointer, or subclass pointer, for the JS object
    static void* FromJSObject(JSValueConst object);
    template <typename T>
    static inline T* FromJSObject(JSValueConst object);
};
template <typename T>
T* JQRefCppSimple::FromJSObject(JSValueConst object) {
    return static_cast<T*>(FromJSObject(object));
}

// Global alias for FromJSObject() to avoid churn.
template <typename T>
inline T* JQUnwrapRefSimple(JSValueConst obj) {
    return JQRefCppSimple::FromJSObject<T>(obj);
}
// == JQRefCppSimple END ==

#if 0
// == JQRefCpp START ==
// with OnInit OnGCCollect OnGCMark lifecycle feature
class JQRefCpp: public JQuick::REF_BASE
{
public:
    JQRefCpp();
    virtual ~JQRefCpp();

    // when _value is attached
    virtual void OnInit(JSContext *ctx, JSValueConst v) {}
    // when _value is detached, _value will be gc collected
    virtual void OnGCCollect(JSRuntime *rt, JSValue v) {}
    // when quickjs is mark objects to try resolve circle dependency or calculate all memory usage,
    // especialy for solitary js objects.
    virtual void OnGCMark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func) {}

    static JSValue NewJSValue(JSContext *ctx, JQRefCpp* cppObj);

    template <typename T>
    inline T* UnwrapRef(JSValueConst obj) {
        return JQRefCpp::FromJSObject<T>(obj);
    }
    // Get a JQRefCpp* pointer, or subclass pointer, for the JS object
    static void* FromJSObject(JSValueConst object);
    template <typename T>
    static inline T* FromJSObject(JSValueConst object);

private:
    static void __js_init_ref_class(JSContext *ctx);
    static void __js_ref_finalizer(JSRuntime *rt, JSValue v);
    static void __js_ref_gc_mark(JSRuntime *rt, JSValueConst v, JS_MarkFunc *mark_func);
};

template <typename T>
T* JQRefCpp::FromJSObject(JSValueConst object) {
    return static_cast<T*>(FromJSObject(object));
}

// Global alias for FromJSObject() to avoid churn.
template <typename T>
inline T* JQUnwrapRef(JSValueConst obj) {
    return JQRefCpp::FromJSObject<T>(obj);
}
// == JQRefCpp END ==
#endif

}  // namespace jqutil_v2