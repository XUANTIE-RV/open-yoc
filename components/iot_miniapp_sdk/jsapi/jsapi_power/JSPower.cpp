#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "jqutil_v2/jqutil.h"
#include "JSPowerImpl.h"

#include "jsapi_publish.h"
#define TAG "JSPower"

static std::vector<void* > g_jsPower_obj;

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
namespace aiot {
class JSPower: public JQPublishObject{
public:
    void OnCtor(JQFunctionInfo &info);
    void OnGCCollect();

    void getInfo(JQAsyncInfo &info);
    void setAutoHibernate(JQAsyncInfo &info);
    void setHibernateTime(JQAsyncInfo &info);
    void shutdown(JQAsyncInfo &info);
    void reboot(JQAsyncInfo &info);
private:
    JSPowerImpl _impl;
};

void JSPower::OnCtor(JQFunctionInfo &info){
    LOGD(TAG, ">>>>> JSPower::OnCtor %x", this);

    jsapi_obj_lock();
    g_jsPower_obj.push_back(this);
    jsapi_obj_unlock();

    _impl.init(this);
}

void JSPower::OnGCCollect(){
    LOGD(TAG, ">>>>> JSPower::OnGCCollect");

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsPower_obj.begin(); itor != g_jsPower_obj.end(); ){
        if (*itor == this) {
            itor = g_jsPower_obj.erase(itor);
        } else {
            itor++;
        }
    }
    jsapi_obj_unlock();
}

void JSPower::getInfo(JQAsyncInfo &info){
   _impl.getInfo([info](PowerManageInfo &record){
       info.post(record.toBson());
   });
}

void JSPower::setAutoHibernate(JQAsyncInfo &info){
  bool isOn = info[0].bool_value();
  _impl.setAutoHibernate(isOn);
  info.post(0);
}
void JSPower::setHibernateTime(JQAsyncInfo &info){
    int time = info[0].number_value();
    _impl.setHibernateTime(time);
    info.post(0);
}
void JSPower::shutdown(JQAsyncInfo &info){
    _impl.shutdown();
    info.post(0);
}
void JSPower::reboot(JQAsyncInfo &info){
    _impl.reboot();
    info.post(0);
}

static JSValue createPower(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "power");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
        return new JSPower();
    });
    tpl->SetProtoMethodPromise("getInfo", &JSPower::getInfo);
    tpl->SetProtoMethodPromise("setAutoHibernate", &JSPower::setAutoHibernate);
    tpl->SetProtoMethodPromise("setHibernateTime", &JSPower::setHibernateTime);
    tpl->SetProtoMethodPromise("shutdown", &JSPower::shutdown);
    tpl->SetProtoMethodPromise("reboot", &JSPower::reboot);
    JQPublishObject::InitTpl(tpl);

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_power(JQModuleEnv* env)
{
    // 返回模块
    return createPower(env);
}
}

extern "C" void jsapi_power_publish_batteryChange(int battery){
    LOGD(TAG, ">>>>> power debug\n");

    Bson::object result;
    result["name"] = aiot::POWER_EVENT_BATTERY_CHANGE;
    result["battery"] = battery;

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsPower_obj.begin(); itor != g_jsPower_obj.end(); itor++){
        aiot::JSPower * self = (aiot::JSPower *)(*itor);
        self->publish(aiot::POWER_EVENT_BATTERY_CHANGE, result);
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_power_publish_charge(bool isCharging){
    Bson::object result;
    result["name"] = aiot::POWER_EVENT_CHARGE;
    result["isCharging"] = isCharging;

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsPower_obj.begin(); itor != g_jsPower_obj.end(); itor++){
        aiot::JSPower * self = (aiot::JSPower *)(*itor);
        self->publish(aiot::POWER_EVENT_CHARGE, result);
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_power_publish_batteryLow(int battery){
    Bson::object result;
    result["name"] = aiot::POWER_EVENT_BATTERY_LOW;
    result["battery"] = battery;

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsPower_obj.begin(); itor != g_jsPower_obj.end(); itor++){
        aiot::JSPower * self = (aiot::JSPower *)(*itor);
        self->publish(aiot::POWER_EVENT_BATTERY_LOW, result);
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_power_publish_batteryEmergency(int battery){
    Bson::object result;
    result["name"] = aiot::POWER_EVENT_BATTERY_EMERGENCY;
    result["battery"] = battery;

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsPower_obj.begin(); itor != g_jsPower_obj.end(); itor++){
        aiot::JSPower * self = (aiot::JSPower *)(*itor);
        self->publish(aiot::POWER_EVENT_BATTERY_EMERGENCY, result);
    }
    jsapi_obj_unlock();
}
