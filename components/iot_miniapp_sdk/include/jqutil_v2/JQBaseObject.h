#pragma once
#include "jqutil_v2/JQDefs.h"
#include "looper/Handler.h"
#include "quickjs/quickjs.h"
#include <functional>
#include <vector>
#include <map>

// #define ENABLE_JQBASEOBJECT_FLAG

namespace jqutil_v2 {

/*
 * 注意：JQBaseObject 基本所有方法都需要在 JSThread 上调用，其他线程想要调用相关方法需要先抛送到 JS 线程再调用
 */
class JQBaseObject: public JQuick::REF_BASE {
private:
#ifdef ENABLE_JQBASEOBJECT_FLAG
    enum {
        FLAG_ATTACHED = 1 << 0,
        FLAG_GC_REGISTER = 1 << 1,
        FLAG_INTERNAL_HOLDER = 1 << 2,
        FLAG_ASYNC_EXECUTOR_STAGE0 = 1 << 3,
        FLAG_ASYNC_EXECUTOR_STAGE1 = 1 << 4,
        FLAG_SIGNAL_REGISTER_STAGE0 = 1 << 5,
        FLAG_SIGNAL_REGISTER_STAGE1 = 1 << 6
    };
    inline void _setAttachedFlag(bool f)
    { f ? _flags |= FLAG_ATTACHED : _flags &= ~FLAG_ATTACHED; }
    inline void _setGcRegisterFlag(bool f)
    { f ? _flags |= FLAG_GC_REGISTER : _flags &= ~FLAG_GC_REGISTER; }
    inline void _setInternalHolderFlag(bool f)
    { f ? _flags |= FLAG_INTERNAL_HOLDER : _flags &= ~FLAG_INTERNAL_HOLDER; }
    inline void _setAsyncExecutorStage0Flag(bool f)
    { f ? _flags |= FLAG_ASYNC_EXECUTOR_STAGE0 : _flags &= ~FLAG_ASYNC_EXECUTOR_STAGE0; }
    inline void _setAsyncExecutorStage1Flag(bool f)
    { f ? _flags |= FLAG_ASYNC_EXECUTOR_STAGE1 : _flags &= ~FLAG_ASYNC_EXECUTOR_STAGE1; }
    inline void _setSignalRegisterStage0Flag(bool f)
    { f ? _flags |= FLAG_SIGNAL_REGISTER_STAGE0 : _flags &= ~FLAG_SIGNAL_REGISTER_STAGE0; }
    inline void _setSignalRegisterStage1Flag(bool f)
    { f ? _flags |= FLAG_SIGNAL_REGISTER_STAGE1 : _flags &= ~FLAG_SIGNAL_REGISTER_STAGE1; }

    inline bool _attachedFlag() const { return _flags & FLAG_ATTACHED; }
    inline bool _gcRegisterFlag() const { return _flags & FLAG_GC_REGISTER; }
    inline bool _internalHolderFlag() const { return _flags & FLAG_INTERNAL_HOLDER; }
    inline bool _asyncExecutorStage0Flag() const { return _flags & FLAG_ASYNC_EXECUTOR_STAGE0; }
    inline bool _asyncExecutorStage1Flag() const { return _flags & FLAG_ASYNC_EXECUTOR_STAGE1; }
    inline bool _asyncExecutorFlag() const
    { return (_flags & (FLAG_ASYNC_EXECUTOR_STAGE0 | FLAG_ASYNC_EXECUTOR_STAGE1)) ==
               (FLAG_ASYNC_EXECUTOR_STAGE0 | FLAG_ASYNC_EXECUTOR_STAGE1); }

    inline bool _signalRegisterStage0Flag() const { return _flags & FLAG_SIGNAL_REGISTER_STAGE0; }
    inline bool _signalRegisterStage1Flag() const { return _flags & FLAG_SIGNAL_REGISTER_STAGE1; }
    inline bool _signalRegisterFlag() const
    { return (_flags & (FLAG_SIGNAL_REGISTER_STAGE0 | FLAG_SIGNAL_REGISTER_STAGE1)) ==
               (FLAG_SIGNAL_REGISTER_STAGE0 | FLAG_SIGNAL_REGISTER_STAGE1); }
#endif

public:
    using CreatorType = std::function<JQBaseObject*()>;
    using GCMarkHook = std::function<void(JQBaseObject*, JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func)>;
    using GCCollectHook = std::function<void(JQBaseObject*, JSContext *ctx)>;

protected:
    JQBaseObject();
    virtual ~JQBaseObject();

    // when _value is attached
    virtual void OnInit() {}
    // when JQFunctionTemplate new instance
    virtual void OnCtor(JQFunctionInfo &info) {}
    // when _value is detached, _value will be gc collected
    virtual void OnGCCollect() {}
    // when quickjs is mark objects to try resolve circle dependency or calculate all memory usage,
    // especialy for solitary js objects.
    virtual void OnGCMark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func) {}

public:
    uint32_t internalFieldCount() const;
    bool setInternalField(const std::string &key, JSValueConst field);
    JSValueConst getInternalField(const std::string &key) const;
    bool hasInternalField(const std::string &key) const;
    bool removeInternalField(const std::string &key);
    void clearInternalFields();
    const std::map<std::string, JSValue>* getInternalFields() const;
    std::map<std::string, JSValue>* getInternalFields();
    std::map<std::string, JSValue>* getOrCreateInternalFields();

    // Global alias for FromJSObject() to avoid churn.
    template <typename T>
    inline T* Unwrap(JSValueConst obj) {
        return JQBaseObject::FromJSObject<T>(obj);
    }

    inline JSValueConst getJSValue() const
    { return _valuePtr ? JS_MKPTR(JS_TAG_OBJECT, _valuePtr) : JS_UNDEFINED; }

    inline JSContext* getContext() const { return (JSContext*)((intptr_t)_ctx & ~0x01); }

    inline void disableJSCall() { _ctx = (JSContext*)((intptr_t)_ctx & ~0x01); }
    inline bool isJSCallDisabled() const { return !((intptr_t)_ctx & 0x01); }

    // wrap JQBaseObject into a JSValue
    static JSValue NewObject(JSContext *ctx, JQBaseObject* cppObj);
    // Get a JQBaseObject* pointer, or subclass pointer, for the JS object
    static JQBaseObject* FromJSObject(JSValueConst object);
    template <typename T>
    static inline T* FromJSObject(JSValueConst object);

    void hookGCCollect(GCCollectHook hook);
    void hookGCMark(GCMarkHook hook);

    JQuick::sp<JQObjectTemplate> objTpl() const;
    JQuick::sp<JQModuleEnv> moduleEnv() const;
    JQuick::sp<JQuick::Handler> jsHandler() const;
    bool inSameThread() const;

    inline bool isAttached() const
    {
#ifdef ENABLE_JQBASEOBJECT_FLAG
        return _attachedFlag();
#else
        return getContext() != NULL;
#endif
    }

    template<typename T>
    JQuick::sp<T> as() { return static_cast<T*>(this); }

private:
    friend class JQObjectTemplate;
    friend class JQFunctionTemplate;
    void _on_js_detach();
    void _on_js_attach(JSContext *ctx, JSValueConst value);
    void _on_js_gc_mark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func);

    static void __js_init_object_class(JSContext *ctx);
    static void __js_object_finalizer(JSRuntime *rt, JSValue v);
    static void __js_object_gc_mark(JSRuntime *rt, JSValueConst v, JS_MarkFunc *mark_func);

    friend class JQObjectSignalRegister;
    JQObjectSignalRegister* getOrCreateSignalRegister();
    JQObjectSignalRegister* getSignalRegister() const;

protected:
    // call on js thread
    JQAsyncExecutor* getOrCreateAsyncExecutor();
    // from another thread
    JQuick::sp<JQAsyncExecutor> getAsyncExecutor() const;

private:
    // will call into _OnCallbackJSON
    friend class JQAsyncInfo;
    // run js thread
    template <typename... Args>
    void _OnSignalMagic(intptr_t magic, Args... args);

private:
    // currently js value is not referenced by JQBaseObject,
    // it will be set to JS_UNDEFINED when js value is freed
    // NOTE: we provide isAttached to check whether can run js logic in c++ callback function,
    // these must be dispatched from C++ to JSThread function calls.
    void*                               _valuePtr = NULL;
    JSContext*                          _ctx = NULL;
    JQuick::sp<JQObjectTemplate>        _objTpl;
#ifdef ENABLE_JQBASEOBJECT_FLAG
    uint32_t                            _flags = 0;
#endif
};

template <typename T>
T* JQBaseObject::FromJSObject(JSValueConst object) {
    return static_cast<T*>(FromJSObject(object));
}

// Global alias for FromJSObject() to avoid churn.
template <typename T>
inline T* JQUnwrap(JSValueConst obj) {
    return JQBaseObject::FromJSObject<T>(obj);
}

}  // namespace jqutil_v2