#pragma once
#include "jqutil_v2/JQBaseObject.h"
#include "jqutil_v2/bson.h"
#include "quickjs/quickjs.h"
#include "utils/REF.h"
#include <map>
#include <string>

namespace jqutil_v2 {

typedef enum {
    JQCallbackType_CustomParams,  // custom params
    JQCallbackType_Simple,  // success -> (res), failed -> (error)
    JQCallbackType_Std,  // success -> (null, res), failed -> (error, ?)
    JQCallbackType_Resolve,  // -> (res)
    JQCallbackType_Reject, // -> (err) -> err.res = res
} JQCallbackType;

typedef struct {
    JQCallbackType type;
    JSValue func;
} JQCallbackDesc;

class JQErrorDesc {
public:
    JQErrorDesc(const std::string &name, const std::string &message, int code=0)
            :name(name), message(message), code(code) {}
    JQErrorDesc() {}
    std::string name;
    std::string message;
    int code = 0;
    Bson customValue;
public:
    JSValue createJSValue(JSContext *ctx) const;
};

class JQAsyncExecutor: public JQuick::REF_BASE {
public:
    JQAsyncExecutor(JQBaseObject *object, JQuick::sp<JQuick::Handler> jsHandler=NULL);
    ~JQAsyncExecutor();

    // function callback, return callbackid
    uint32_t addCallback(JSValueConst callback, JQCallbackType type=JQCallbackType_Std);
    // delete callback registered, not guarantee to fullfill
    void removeCallback(uint32_t callbackid);
    // create promise resolve/reject functions, and return promiseId as callbackid
    uint32_t createPromiseId(JSContext *ctx, JSValue *outPromiseOrException);

    // can be called not on JSThread
    void removeCallbackAsync(uint32_t callbackid);
    void onCallbackJSONAsync(uint32_t callbackid, const std::string &json, const JQErrorDesc * errDesc=nullptr, bool autoDel=true);
    void onCallbackAsync(uint32_t callbackid, const Bson &bson, const JQErrorDesc * errDesc=nullptr, bool autoDel=true);
    void onErrorAsync(uint32_t callbackid, const std::string &message, int code=0, const std::string &name="", bool autoDel=true);

    // should be called on js thread
    void onCallbackJSON(uint32_t callbackid, const std::string &json, const JQErrorDesc * errDesc=nullptr, bool autoDel=true);
    void onCallback(uint32_t callbackid, const Bson &bson, const JQErrorDesc * errDesc=nullptr, bool autoDel=true);
    void onCallbackJSValue(uint32_t callbackid, int argc, JSValueConst *argv, const JQErrorDesc * errDesc=nullptr, bool autoDel=true);
    void onError(int callbackid, const std::string &message, int code=0, const std::string &name="", bool autoDel=true);

    bool hasCallbackid(uint32_t callbackid) const;

    JQuick::sp<JQuick::Handler> jsHandler();

    void onGCMark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func);
    void onGCCollect(JSContext *ctx);

protected:
    // resolve reject funcs
    uint32_t _addResolvingFuncs(JSContext *ctx, JSValue resolve, JSValue reject);

protected:
    inline uint32_t _genId() { return ++_tokenId; }
    void _freeDescVec(JSContext *ctx, std::vector<JQCallbackDesc> &descVec);

    // auxiliary functions
    void _onCallbackJSONJSThread(uint32_t callbackid, const std::string &json, const JQErrorDesc &errDesc, bool autoDel, bool hasErrDesc);
    void _onCallbackJSThread(uint32_t callbackid, const Bson &bson, const JQErrorDesc &errDesc, bool autoDel, bool hasErrDesc);

protected:
    uint32_t _tokenId = 0;

    // for async method callback
    std::map<uint32_t/*callbackid*/, std::vector<JQCallbackDesc>/*callback list*/> _callbackMap;

    JQuick::wp<JQBaseObject> _object;

    JQuick::sp<JQuick::Handler> _jsHandler;
};

}  // namespace jqutil_v2