#pragma once
#include "jqutil_v2/jqutil.h"
#include "utils/StrongPtr.h"
#include <string>
#include <functional>
#include <map>
#include "utils/REF.h"

namespace aiot {

class Request;
class Response;

using ProgressCb = std::function<void(int err, size_t bytes, size_t totalBytes)>;
using FinishCb = std::function<void(JQuick::sp<Request> req, JQuick::sp<Response> rep)>;

class JSHttpImpl {
public:
    void request(JQuick::sp<Request> req);

};

class Request: public JQuick::REF_BASE {
public:
    std::map<std::string, std::string> headers;
    std::string url;
    std::string method;
    std::string body;
    std::string outPath;
    long timeout = 0L;  // ms
    long connectTimeout = 6000L;  // ms
    bool decompress = true;
    long progressInterval = 3000L;  // ms
    ProgressCb progressCb = nullptr;
    FinishCb finishCb = nullptr;
};

class Response: public JQuick::REF_BASE {
public:
    int ret = 0;  // ret == 0 is success, other is failed
    std::string msg;  // readable error message

    int status = 0;  // http status, valid when ret == 0
    std::map<std::string, std::string> headers;
    std::string body;  // text normally, or maybe binary buffer
};

}  // namespace aiot
