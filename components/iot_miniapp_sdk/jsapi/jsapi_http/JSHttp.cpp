#include "jqutil_v2/jqutil.h"
#include "JSHttpImpl.h"

using namespace jqutil_v2;

namespace aiot {

class JSHttp: public JQPublishObject {
public:
    void request(JQAsyncInfo &info);
    void download(JQAsyncInfo &info);

protected:
    JQuick::sp<Request> _createRequest(JQAsyncInfo& info, bool isDownloadRequest);

    JSHttpImpl _http;
};

JQuick::sp<Request> JSHttp::_createRequest(JQAsyncInfo& info, bool isDownloadRequest)
{
    if (!info[0].is_object()) {
        info.postError("first arg should be object of options");
        return NULL;
    }
    Bson::object opts = info[0].object_items();

    JQuick::sp<Request> req = new Request();

    if (opts["url"].is_string()) {
        req->url = opts["url"].string_value();
    }
    if (req->url.empty()) {
        info.postError("Invalid request");
        return NULL;
    }
    if (opts["method"].is_string()) {
        req->method = opts["method"].string_value();
    }
    if (opts["headers"].is_object()) {
        for (auto &iter: opts["headers"].object_items()) {
            req->headers[iter.first] = iter.second.string_value();
        }
    }
    if (opts["data"].is_string()) {
        req->body = opts["data"].string_value();
    }
    if (opts["timeout"].is_number()) {
        req->timeout = opts["timeout"].int_value();
    }
    if (opts["connectTimeout"].is_number()) {
        req->connectTimeout = opts["connectTimeout"].int_value();
    }
    if (opts["decompress"].is_bool()) {
        req->decompress = opts["decompress"].bool_value();
    }
    if (opts["progressInterval"].is_number()) {
        req->progressInterval = opts["progressInterval"].int_value();
    }

    if (isDownloadRequest) {
        if (opts["outPath"].is_string()) {
            req->outPath = opts["outPath"].string_value();
        }
        if (req->outPath.empty()) {
            info.postError("Invalid request");
            return NULL;
        }
        if (opts["progress"].is_number()) {
            // prepared in hook
            int progressCbId = opts["progress"].int_value();
            JQuick::wp<JSHttp> that = this;
            req->progressCb = [that, progressCbId] (int err, size_t bytes, size_t totalBytes) {
                JQuick::sp<JSHttp> thiz = that.promote();
                if (thiz == NULL) {
                    return;
                }
                thiz->moduleEnv()->postJSThread([thiz, progressCbId, err, bytes, totalBytes]() {
                    if (!thiz->isAttached()) {
                        return;
                    }
                    JQuick::sp<JQAsyncExecutor> asyncExecutor = thiz->getAsyncExecutor();
                    if (!err) {
                        JSContext *ctx = thiz->getContext();
                        JSValueConst res[2];
                        res[0] = JS_NewUint32(ctx, bytes);
                        res[1] = JS_NewUint32(ctx, totalBytes);
                        asyncExecutor->onCallbackJSValue(progressCbId, 2, res, NULL, false);
                        JS_FreeValue(ctx, res[0]);
                        JS_FreeValue(ctx, res[1]);
                    }
                    if (err || (bytes > 0 && bytes == totalBytes)) {
                        asyncExecutor->removeCallback(progressCbId);
                    }
                });
            };
        };
    }

    req->finishCb = [info](JQuick::sp<Request> req, JQuick::sp<Response> rep) {
        if (rep->ret != 0) {
            JQErrorDesc errDesc;
            errDesc.message = rep->msg;
            errDesc.code = rep->ret;
            info.postError(errDesc);
        } else {
            Bson::object res, headers;
            res["status"] = rep->status;
            res["body"]  = rep->body;
            for (auto& item : rep->headers) {
                headers[item.first] = item.second;
            }
            res["headers"] = headers;
            info.post(res);
        }
    };

    return req;
}

void JSHttp::request(JQAsyncInfo &info)
{
    JQuick::sp<Request> req = _createRequest(info, false);
    if (req != NULL) {
        _http.request(req);
    }
}

void JSHttp::download(JQAsyncInfo &info)
{
    JQuick::sp<Request> req = _createRequest(info, true);
    if (req != NULL) {
        _http.request(req);
    }
}

static JSValue createHttp(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "foo");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
      return new JSHttp();
    });
    JQPublishObject::InitTpl(tpl);
    tpl->SetProtoMethodPromise("request", &JSHttp::request);
    tpl->SetProtoMethodPromise("download", &JSHttp::download);

    static JQuick::ThreadPool* threadPool = new JQuick::ThreadPool("http", 0, 2, 8192);
    tpl->setAsyncScheduleHook([](JQAsyncScheduleInfo &info) {
        info.setThreadPool(threadPool);
    });

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_http(JQModuleEnv* env)
{
    // 返回模块
    return createHttp(env);
}

}  // namespace aiot

