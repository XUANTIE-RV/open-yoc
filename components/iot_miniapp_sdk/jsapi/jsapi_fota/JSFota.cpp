#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "jqutil_v2/jqutil.h"
#include "JSFotaImpl.h"

#define TAG "JSFota"

static std::vector<void* > g_jsFota_obj;

/* jsapi message publish lock */
static aos_mutex_t g_jsapi_obj_mutex = NULL;
static void jsapi_obj_lock()
{
    if(g_jsapi_obj_mutex == NULL) {
        aos_mutex_new(&g_jsapi_obj_mutex);
    }

    aos_mutex_lock(&g_jsapi_obj_mutex, AOS_WAIT_FOREVER);
}

static void jsapi_obj_unlock()
{
    aos_mutex_unlock(&g_jsapi_obj_mutex);
}

using namespace jqutil_v2;
namespace aiot
{
class JSFota : public JQPublishObject
{
public:
    void OnCtor(JQFunctionInfo &info);
    void OnGCCollect();

    void start(JQAsyncInfo &info);
    void stop(JQAsyncInfo &info);
    void getState(JQAsyncInfo &info);
    void versionCheck(JQAsyncInfo &info);
    void download(JQAsyncInfo &info);
    void restart(JQAsyncInfo &info);

private:
    JSFotaImpl _impl;
};

void JSFota::OnCtor(JQFunctionInfo &info)
{
    LOGD(TAG, ">>>>> JSFota::OnCtor %x", this);

    jsapi_obj_lock();
    g_jsFota_obj.push_back(this);
    jsapi_obj_unlock();

    _impl.init(this); 
}

void JSFota::OnGCCollect(){
    LOGD(TAG, ">>>>> JSFota::OnGCCollect");

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsFota_obj.begin(); itor != g_jsFota_obj.end(); ){
        if (*itor == this) {
            itor = g_jsFota_obj.erase(itor);
        } else {
            itor++;
        }
    }
    jsapi_obj_unlock();
}

void JSFota::start(JQAsyncInfo &info){
    int ret = _impl.start();
    info.post(ret);
}

void JSFota::stop(JQAsyncInfo &info){
    int ret = _impl.stop();
    info.post(ret);
}

void JSFota::getState(JQAsyncInfo &info){
    std::string state = _impl.getState();

    info.post(Bson(state));
}

void JSFota::versionCheck(JQAsyncInfo &info){

    _impl.versionCheck();
    info.post(0);

}

void JSFota::download(JQAsyncInfo &info){

    _impl.download();
    info.post(0);
}

void JSFota::restart(JQAsyncInfo &info){
    int ms = info[0].int_value();
    int ret = _impl.restart(ms);
    info.post(ret);
}

static JSValue createJSFota(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "fota");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
        return new JSFota();
    });
       tpl->SetProtoMethodPromise("start", &JSFota::start);
       tpl->SetProtoMethodPromise("stop", &JSFota::stop);
       tpl->SetProtoMethodPromise("getState", &JSFota::getState);
       tpl->SetProtoMethodPromise("versionCheck", &JSFota::versionCheck);
       tpl->SetProtoMethodPromise("download", &JSFota::download);
       tpl->SetProtoMethodPromise("restart", &JSFota::restart);
       JQPublishObject::InitTpl(tpl);
       
    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_fota(JQModuleEnv* env)
{
    // 返回模块
    return createJSFota(env);
}

}  // namespace aiot

void publish_fota_jsapi(int event, void *P){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsFota_obj.begin(); itor != g_jsFota_obj.end(); itor++){
        aiot::JSFota * self = (aiot::JSFota *)(*itor);

        aiot::FotaVersion* _v;
        aiot::FotaDownload* _d;
        aiot::FotaEnd* _e;
        aiot::FotaRestart* _r;
        switch (event) {
        case 0:
            _v = (aiot::FotaVersion*)P;
            self->publish(aiot::JS_FOTA_EVENT_VERSION, (*_v).toBson());
            LOGD(TAG, "publish version");
            break;
        case 1:
            _d = (aiot::FotaDownload*)P;        
            self->publish(aiot::JS_FOTA_EVENT_DOWNLOAD, (*_d).toBson());
            LOGD(TAG, "publish download");
            break;
        case 2:
            _e = (aiot::FotaEnd*)P;
            self->publish(aiot::JS_FOTA_EVENT_END, (*_e).toBson());
            LOGD(TAG, "publish end");
            break;
        case 3:
            _r = (aiot::FotaRestart*)P;
            self->publish(aiot::JS_FOTA_EVENT_RESTART, (*_r).toBson());
            LOGD(TAG, "publish restart");
            break;
        default:
            //self->publish("fota_hello", "hello");
            break;
        }
    }
    jsapi_obj_unlock();
}