#include <board.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "jqutil_v2/jqutil.h"
#include "JSWifiImpl.h"

#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <uservice/eventid.h>
#include <devices/netdrv.h>
#include <devices/wifi.h>

#include "jsapi_publish.h"

#define TAG "JSWiFi"

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT 
static std::vector<void* > g_jswifi_obj;

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
#endif

using namespace jqutil_v2;
namespace aiot {


class JSWifi: public JQPublishObject {
public:
    void OnCtor(JQFunctionInfo &info);
    void OnGCCollect();

    // wifi 扫描&通知机制
    void scan(JQAsyncInfo &info);

    // wifi 配置增删改查接口
    void addConfig(JQAsyncInfo &info);
    void removeConfig(JQAsyncInfo &info);
    void changeConfig(JQAsyncInfo &info);
    void listConfig(JQAsyncInfo &info);

    // 发起连接和断开
    void connect(JQAsyncInfo &info);
    void disconnect(JQAsyncInfo &info);

private:
    JSWifiImpl _impl;
};

void JSWifi::OnCtor(JQFunctionInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT    
    LOGD(TAG, ">>>>> JSWifi::OnCtor %x", this);
    jsapi_obj_lock();
    g_jswifi_obj.push_back(this);
    jsapi_obj_unlock();

    _impl.init(this);
#endif
}

void JSWifi::OnGCCollect(){
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT    
    LOGD(TAG, ">>>>> JSWifi::OnGCCollect");

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jswifi_obj.begin(); itor != g_jswifi_obj.end(); ){
        if (*itor == this) {
            itor = g_jswifi_obj.erase(itor);
        } else {
            itor++;
        }
    }
    jsapi_obj_unlock();
#endif
}

void JSWifi::scan(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT  
    // 模拟通知 JS 空间扫描结果
    JQuick::sp<JSWifi> self = this;
    _impl.scan(
        [info, self]
        (std::vector<WifiApRecord> &recordList){
        Bson::array result;
        for (auto &record: recordList) {
            result.push_back(record.toBson());
        }
        self->publish("scan_result", result);
        info.post(result);
    });
#endif
}

void JSWifi::addConfig(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT 
    if (!info[0].is_object()) {
        info.postError("first param should be object of WifiConfig");
        return;
    }

    std::string ssid = info[0]["ssid"].string_value();
    std::string psk = info[0]["psk"].string_value();
    if (ssid.empty()) {
        info.postError("ssid should not be empty");
        return;
    }

    _impl.addConfig(ssid, psk);
    info.post(0);
#endif
}
void JSWifi::removeConfig(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT  
    std::string ssid = info[0].string_value();
    if (ssid.empty()) {
        info.postError("first arg should be ssid, but got empty");
        return;
    }

    _impl.removeConfig(ssid);
    info.post(0);
#endif
}

void JSWifi::changeConfig(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT 
    if (!info[0].is_object()) {
        info.postError("first param should be object of WifiConfig");
        return;
    }

    std::string ssid = info[0]["ssid"].string_value();
    std::string psk = info[0]["psk"].string_value();
    if (ssid.empty()) {
        info.postError("ssid should not be empty");
        return;
    }

    _impl.changeConfig(ssid, psk);
    info.post(0);
#endif
}

void JSWifi::listConfig(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT 
    std::vector<WifiConfig> configList = _impl.listConfig();

    Bson::array result;
    for (auto &item: configList) {
        Bson::object config;
        config["ssid"] = item.ssid;
        config["psk"] = item.psk;
        result.push_back(config);
    }

    info.post(result);
#endif
}

void JSWifi::connect(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT  
    int ret;
    std::string ssid = info[0].string_value();
    if (ssid.empty()) {
        info.postError("first arg should be ssid, but got empty");
        return;
    }
    JQuick::sp<JSWifi> self = this;
    ret = _impl.connect(ssid);
    info.post(ret);
#endif
}

void JSWifi::disconnect(JQAsyncInfo &info)
{
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT 
    int ret;
    ret = _impl.disconnect();

    info.post(ret);
#endif
}

static JSValue createWifi(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "wifi");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
      return new JSWifi();
    });
    tpl->SetProtoMethodPromise("scan", &JSWifi::scan);
    tpl->SetProtoMethodPromise("addConfig", &JSWifi::addConfig);
    tpl->SetProtoMethodPromise("removeConfig", &JSWifi::removeConfig);
    tpl->SetProtoMethodPromise("changeConfig", &JSWifi::changeConfig);
    tpl->SetProtoMethodPromise("listConfig", &JSWifi::listConfig);
    tpl->SetProtoMethodPromise("connect", &JSWifi::connect);
    tpl->SetProtoMethodPromise("disconnect", &JSWifi::disconnect);
    JQPublishObject::InitTpl(tpl);

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_wifi(JQModuleEnv* env)
{
    // 返回模块
    return createWifi(env);
}

}  // namespace aiot
// 发送事件
extern "C" void publish_wifi_jsapi(std::string result , aiot::WifiConnectInfo* _info){
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT 
    LOGD(TAG,">>>> %s", _info->bssid.c_str());
    LOGD(TAG,">>>> %s", _info->reason.c_str());
    LOGD(TAG,">>>> %s", _info->ssid.c_str());
    
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    if(result == aiot::WIFI_EVENT_COMPLETED){
        for (itor = g_jswifi_obj.begin(); itor != g_jswifi_obj.end(); itor++){
            aiot::JSWifi * self = (aiot::JSWifi *)(*itor);
            LOGD(TAG, ">>>>> jsapi_voice_publish %x", self);
            self->publish(aiot::WIFI_EVENT_COMPLETED , _info->toBson());
        }
    }else if(result == aiot::WIFI_EVENT_DISCONNECTED){
        for (itor = g_jswifi_obj.begin(); itor != g_jswifi_obj.end(); itor++){
            aiot::JSWifi * self = (aiot::JSWifi *)(*itor);
            LOGD(TAG, ">>>>> jsapi_voice_publish %x", self);
            self->publish(aiot::WIFI_EVENT_DISCONNECTED , _info->toBson());
        }
    }
    jsapi_obj_unlock();
#endif
}