#pragma once
#include "jqutil_v2/jqmisc.h"
#include "jqutil_v2/JQDefs.h"
#include "jqutil_v2/JQAsyncExecutor.h"
#include "jqutil_v2/bson.h"
#include "jqutil_v2/JQObjectTemplate.h"
#include "jqutil_v2/JQRefCpp.h"
#include <string>
#include <functional>
#include "utils/REF.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace jqutil_v2 {

class JQReturnValue {
public:
    inline JQReturnValue(const JQReturnValue& that)
            : _ctx(that._ctx), _value(that._value) {
    }
    // Local setters
    inline void Set(JSValue value) { jq_set_value(_ctx, &_value, value); }
    // Fast primitive setters
    inline void Set(bool value) { jq_set_value(_ctx, &_value, JS_NewBool(_ctx, value)); }
    inline void Set(double i) { jq_set_value(_ctx, &_value, JS_NewFloat64(_ctx, i)); }
    inline void Set(int32_t i) { jq_set_value(_ctx, &_value, JS_NewInt32(_ctx, i)); }
    inline void Set(uint32_t i) { jq_set_value(_ctx, &_value, JS_NewUint32(_ctx, i)); }
    inline void Set(const char* msg) { jq_set_value(_ctx, &_value, JS_NewString(_ctx, msg)); }
    inline void Set(const std::string &msg) { jq_set_value(_ctx, &_value, JS_NewStringLen(_ctx, msg.c_str(), msg.size())); }
    // Fast JS primitive setter
    inline void SetNull() { jq_set_value(_ctx, &_value, JS_NULL); }
    inline void SetUndefined() { jq_set_value(_ctx, &_value, JS_UNDEFINED); }
    inline void SetException() { jq_set_value(_ctx, &_value, JS_EXCEPTION); }
    inline void ThrowSyntaxError(const char* fmt, ...)
    { va_list ap; va_start(ap, fmt); jq_set_value(_ctx, &_value, JS_ThrowSyntaxError(_ctx, fmt, ap)); va_end(ap); }
    inline void ThrowTypeError(const char* fmt, ...)
    { va_list ap; va_start(ap, fmt); jq_set_value(_ctx, &_value, JS_ThrowTypeError(_ctx, fmt, ap)); va_end(ap); }
    inline void ThrowReferenceError(const char* fmt, ...)
    { va_list ap; va_start(ap, fmt); jq_set_value(_ctx, &_value, JS_ThrowReferenceError(_ctx, fmt, ap)); va_end(ap); }
    inline void ThrowRangeError(const char* fmt, ...)
    { va_list ap; va_start(ap, fmt); jq_set_value(_ctx, &_value, JS_ThrowRangeError(_ctx, fmt, ap)); va_end(ap); }
    inline void ThrowInternalError(const char* fmt, ...)
    { va_list ap; va_start(ap, fmt); jq_set_value(_ctx, &_value, JS_ThrowInternalError(_ctx, fmt, ap)); va_end(ap); }
    inline void ThrowOutOfMemory()
    { jq_set_value(_ctx, &_value, JS_ThrowOutOfMemory(_ctx)); }

    // Convenience getter for Isolate
    inline JSContext* GetContext() const;

    inline JSValueConst Get() const { return _value; }

private:
    friend class JQFunctionInfo;
    friend class JQJSThreadInfo;
    JQReturnValue(JSContext *ctx, JSValue value=JS_UNDEFINED)
            :_ctx(ctx), _value(value) {}

    JSContext *_ctx;
    JSValue _value;
};

using JQParams = std::vector<Bson>;

class JQParamsHolder {
    Bson param_null;
public:
    /** The number of available arguments. */
    inline size_t Length() const { return _params.size(); }

    /** The arguments. */
    inline const JQParams& Params() const { return _params; }
    inline void SetParams(const JQParams& params) { _params = params; }
    std::string ParamsToString() const;

    /**
     * Accessor for the available arguments. Returns `null` if the index
     * is out of bounds.
     */
    inline const Bson& operator[](uint32_t i) const
    { if (i >= Length()) { return param_null; }
        else { return _params[i]; } }

protected:
    JQParams _params;
};

class JQFunctionInfo
{
public:
    /** The number of available arguments. */
    inline size_t Length() const { return _length; }
    /** The arguments. */
    inline JSValueConst* Argv() const { return _argv; }
    /**
     * Accessor for the available arguments. Returns `undefined` if the index
     * is out of bounds.
     */
    inline JSValueConst operator[](uint32_t i) const
    { if (i >= Length()) { return JS_UNDEFINED; }
        else { return _argv[i]; } }

    /** Returns the receiver. This corresponds to the "this" value. */
    inline JSValueConst This() const { return _this_value; }
    /**
     * If the callback was created without a Signature, this is the same
     * value as This(). If there is a signature, and the signature didn't match
     * This() but one of its hidden prototypes, this will be the respective
     * hidden prototype.
     *
     * Note that this is not the prototype of This() on which the accessor
     * referencing this callback was found (which in V8 internally is often
     * referred to as holder [sic]).
     */
    inline JSValueConst Holder() const { return _holder; }
    /** For construct calls, this returns the "new.target" value. */
    inline JSValueConst NewTarget() const { return _new_target; }
    /** Indicates whether this is a regular call or a construct call. */
    inline bool IsConstructCall() const { return _isConstructorCall; }
    /** The current Isolate. */
    inline JSContext* GetContext() const { return _ctx; }
    /** The JQReturnValue for the call. */
    inline JQReturnValue& GetReturnValue() const { return _returnValue; }

    template<typename T>
    inline T* UnwrapHolder() const
    {
        return JQUnwrapRefSimple<T>(Holder());
    }

    template<typename T>
    inline T* UnwrapThis() const
    {
        return JQUnwrap<T>(This());
    }

    inline JQRef<JQObjectTemplate> objTpl() const { return _objTpl; }
    JQRef<JQModuleEnv> moduleEnv() const;

    JQParamsHolder toParamsHolder() const;

protected:
    friend class JQFunctionTemplate;
    friend class JQObjectTemplate;
    inline JQFunctionInfo(JSContext *ctx):
            _ctx(ctx), _length(0), _argv(nullptr), _this_value(JS_UNDEFINED),
            _holder(JS_UNDEFINED), _new_target(JS_UNDEFINED), _isConstructorCall(false),
            _returnValue(ctx)
    {}

    JSContext *_ctx;
    size_t _length;
    JSValueConst *_argv;
    JSValueConst _this_value;
    // the js object corresponding to cpp object
    JSValueConst _holder;
    JSValueConst _new_target;
    bool _isConstructorCall;
    mutable JQReturnValue _returnValue;
    JQRef<JQObjectTemplate> _objTpl;
    JQWRef<JQFunctionTemplate> _funcTpl;
};

class JQJSThreadInfo {
public:
    /** The number of available arguments. */
    inline size_t Length() const { return _params.size(); }
    /** The arguments. */
    inline const JQParams& Params() const { return _params; }
    /**
     * Accessor for the available arguments. Returns `undefined` if the index
     * is out of bounds.
     */
    inline const Bson operator[](uint32_t i) const
    { if (i >= Length()) { return Bson(); }
        else { return _params[i]; } }

    /** The current Isolate. */
    inline JSContext* GetContext() const { return _ctx; }
    /** The JQReturnValue for the call. */
    inline JQReturnValue& GetReturnValue() const { return _returnValue; }

    inline JQRef<JQObjectTemplate> objTpl() const { return _objTpl; }
    JQRef<JQModuleEnv> moduleEnv() const;

    template<typename T>
    JQuick::sp<T> DataAs() const { return static_cast<T*>(_data); }

protected:
    friend class JQFunctionTemplate;
    friend class JQObjectTemplate;
    friend class JQAsyncInfo;
    inline JQJSThreadInfo(JSContext *ctx):
        _ctx(ctx), _returnValue(ctx) {}

    JSContext *_ctx;
    JQParams _params;
    mutable JQReturnValue _returnValue;
    JQuick::wp<JQBaseObject> _thisCppObj;
    JQuick::wp<JQBaseObject> _holderCppObj;
    JQRef<JQObjectTemplate> _objTpl;
    JQuick::sp<JQuick::REF_BASE> _data;
};

using JQJSThreadCallback = std::function<void(JQJSThreadInfo&)>;

class JQAsyncInfo: public JQParamsHolder {
public:
    void post(const Bson &json, const JQErrorDesc *errDesc=NULL) const;
    void postJSON(const std::string& json, const JQErrorDesc *errDesc=NULL) const;
    // send error
    void postError(const JQErrorDesc &errDesc) const;
    void postError(const std::string &message) const;
    void postError(const char* fmt, ...) const;
    void postError(const Bson &bson) const;
    // dispatch function in js thread
    void postJSThread(JQJSThreadCallback func) const;
    bool isSettled() const;
    inline JQRef<JQObjectTemplate> objTpl() const { return _objTpl; }
    JQRef<JQModuleEnv> moduleEnv() const;

    void SetData(JQuick::REF_BASE *data);
    template<typename T>
    JQuick::sp<T> DataAs() const { return static_cast<T*>(_data.get()); }

private:
    static void Caller_postJSThread(JQJSThreadCallback func, JQAsyncInfo self);

protected:
    friend class JQFunctionTemplate;
    friend class JQObjectTemplate;
    uint32_t _callbackId = 0;
    JQuick::wp<JQBaseObject> _thisCppObj;
    JQuick::wp<JQBaseObject> _holderCppObj;
    JQRef<JQObjectTemplate> _objTpl;
    JQuick::sp<JQuick::REF_BASE> _data;
};

typedef void JQFunctionCallback (JQFunctionInfo &);
typedef void JQAsyncCallback (JQAsyncInfo &);

using JQFunctionCallbackType = std::function<JQFunctionCallback >;
using JQAsyncCallbackType = std::function<JQAsyncCallback>;

}  // namespace jqutil_v2