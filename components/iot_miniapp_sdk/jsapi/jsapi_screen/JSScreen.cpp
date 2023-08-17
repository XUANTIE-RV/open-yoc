#include "jqutil_v2/jqutil.h"
#include "JSScreenImpl.h"
#include  <ulog/ulog.h>

#define TAG "JSScreen"

using namespace jqutil_v2;
namespace aiot {
class JSScreen: public JQPublishObject{
public:
    void OnCtor(JQFunctionInfo &info);
    void getInfo(JQAsyncInfo &info);
    void turnOn(JQAsyncInfo &info);
    void turnOff(JQAsyncInfo &info);
    void setAutoBrightness(JQAsyncInfo &info);
    void setBrightness(JQAsyncInfo &info);
    void setAutoOff(JQAsyncInfo &info);
    void setAutoOffTimeout(JQAsyncInfo &info);
private:
    JSScreenImpl _impl;
};

void JSScreen::OnCtor(JQFunctionInfo &info){
    _impl.init(this);
}
void JSScreen::getInfo(JQAsyncInfo &info){
    _impl.getInfo([info](ScreenManageInfo &record){
        info.post(record.toBson());
    });
}

void JSScreen::turnOn(JQAsyncInfo &info){
    _impl.turnOn();
    info.post(0);
}

void JSScreen::turnOff(JQAsyncInfo &info){
    _impl.turnOff();
    info.post(0);
}

void JSScreen::setAutoBrightness(JQAsyncInfo &info){
    bool isOn = info[0].bool_value();
    _impl.setAutoBrightness(isOn);
    info.post(0);
}

void JSScreen::setBrightness(JQAsyncInfo &info){
    int percent = info[0].number_value();
    if(percent < 0 || percent >100)
    {
        printf("input number 0~100!!");
        return;
    }
    _impl.setBrightness(percent);
    info.post(0);
}

void JSScreen::setAutoOff(JQAsyncInfo &info){
    bool isOn = info[0].bool_value();
    _impl.setAutoOff(isOn);
    info.post(0);
}

void JSScreen::setAutoOffTimeout(JQAsyncInfo &info){
    int time = info[0].number_value();
    _impl.setAutoOffTimeout(time);
    info.post(0);
}

static JSValue createScreen(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "screen");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
        return new JSScreen();
    });

    tpl->SetProtoMethodPromise("getInfo", &JSScreen::getInfo);
    tpl->SetProtoMethodPromise("turnOn", &JSScreen::turnOn);
    tpl->SetProtoMethodPromise("turnOff", &JSScreen::turnOff);
    tpl->SetProtoMethodPromise("setAutoBrightness", &JSScreen::setAutoBrightness);
    tpl->SetProtoMethodPromise("setBrightness", &JSScreen::setBrightness);
    tpl->SetProtoMethodPromise("setAutoOff", &JSScreen::setAutoOff);
    tpl->SetProtoMethodPromise("setAutoOffTimeout", &JSScreen::setAutoOffTimeout);
    JQPublishObject::InitTpl(tpl);

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_screen(JQModuleEnv* env)
{
    // 返回模块
    return createScreen(env);
}
}
