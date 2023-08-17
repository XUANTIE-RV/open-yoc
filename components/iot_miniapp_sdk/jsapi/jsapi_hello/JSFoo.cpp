#include "jqutil_v2/jqutil.h"
// #include "utils/PathUtil.h"

using namespace jqutil_v2;

namespace hello {

class JSFoo: public JQBaseObject {
public:
    // 简单同步接口
    void joinPath(JQFunctionInfo &info);
    // 文件异步接口
    void readFile(JQAsyncInfo &info);
    // 网络异步接口
    void requestHttp(JQAsyncInfo &info);

};

class JSFooWifi: public JQPublishObject {
public:
    // wifi 扫描&通知机制
    void scanWifi(JQAsyncInfo &info)
    {
        // 模拟通知 JS 空间扫描结果
        Bson::array result;
        result.push_back("ssid0");
        result.push_back("ssid1");
        result.push_back("ssid2");
        publish("scan_result", result);
        // 异步接口必须回调
        info.post(0);
    }
};

void JSFoo::joinPath(JQFunctionInfo &info)
{
    JSContext *ctx = info.GetContext();
    std::vector<std::string> slices;

    for (unsigned idx=0; idx < info.Length(); idx++) {
        slices.push_back(JQString(ctx, info[idx]).getString());
    }

    // info.GetReturnValue().Set(JQuick::pathjoin(slices));
}

void JSFoo::readFile(JQAsyncInfo &info)
{
    std::string path = info[0].string_value();
    printf("JSFoo::readFile path: %s\n", path.c_str());
    // read file from disk
    std::string content = "abcd1234";
    info.post(content);
}

void JSFoo::requestHttp(JQAsyncInfo &info)
{
    static bool mockError = false;

    std::string url = info[0].string_value();
    printf("JSFoo::requestHttp url: %s\n", url.c_str());

    if (mockError) {
        // 模拟请求错误
        info.postError("got mockError(%d) from requestHttp", mockError);
    } else {
        // get/post from server
        std::string body = "<html><body>here is a http result</body></html>";
        info.post(body);
    }
}

static JSValue createFoo(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "foo");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
      return new JSFoo();
    });
    tpl->SetProtoMethod("joinPath", &JSFoo::joinPath);
    tpl->SetProtoMethodPromise("readFile", &JSFoo::readFile);
    tpl->SetProtoMethodPromise("requestHttp", &JSFoo::requestHttp);

    return tpl->CallConstructor();
}

static JSValue createFooWifi(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "fooWifi");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
      return new JSFooWifi();
    });
    tpl->SetProtoMethodPromise("scanWifi", &JSFooWifi::scanWifi);
    JSFooWifi::InitTpl(tpl);

    return tpl->CallConstructor();
}

// extern
void foo_init(JQModuleEnv* env)
{
    // 导出第一个模块
    env->setModuleExport("foo", createFoo(env));
    // 导出第二个模块
    env->setModuleExport("fooWifi", createFooWifi(env));
}

}  // namespace hello

