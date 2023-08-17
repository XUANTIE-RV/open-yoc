#pragma once
#include <string>
#include <devices/display.h>
#include <devices/input.h>
#include "jqutil_v2/jqutil.h"

#define TAG "JSScreen"
using namespace jqutil_v2;

namespace aiot
{
static std::string SCREEN_EVENT_STATUS_CHANGE = "StatusChange";

// kv save info
static std::string KV_SCREEN_isAutoBrightness   = "KV_SCREEN_isAutoBrightness";     // 自动调光
static std::string KV_SCREEN_isAutoOff          = "KV_SCREEN_isAutoOff";            // 自动息屏
static std::string KV_SCREEN_autoOffTimeout     = "KV_SCREEN_autoOffTimeout";       // 自动息屏时间
static std::string KV_SCREEN_brightness         = "KV_SCREEN_brightness";           // 屏幕亮度

// screen infso
typedef struct ScreenManageInfo {
    bool isOn;
    bool isAutoBrightness;
    uint8_t brightness;
    int autoOffTimeout;
    bool isAutoOff;
    inline Bson toBson()
    {
        Bson::object result;
        result["isOn"] = isOn;
        result["isAutoBrightness"] = isAutoBrightness;
        result["brightness"] = (int)brightness;
        result["autoOffTimeout"] = (int)autoOffTimeout;
        result["isAutoOff"] = isAutoOff;
        return result;
    }
} ScreenManageInfo;

// screen info
typedef struct ScreenStatusInfo {
    bool isOn;
    inline Bson toBson()
    {
        Bson::object result;
        result["isOn"] = isOn;
        return result;
    }
} ScreenStatusInfo;

class JSScreenImpl
{
public:
    using ScreenCallback = std::function< void(ScreenManageInfo& recordInfo) >;

public:
    void init(JQuick::sp< JQPublishObject > pub);
    void getInfo(ScreenCallback cb);
    void turnOn();
    void turnOff();
    void setAutoBrightness(bool isAuto);
    void setBrightness(int percent);
    void setAutoOff(bool isAuto);
    void setAutoOffTimeout(int time);


    // static void screen_timer_cb(void *_pub, void* arg);
    // void touchscreen_event(rvm_dev_t *dev, int event_id, void *arg);
    
    JQuick::sp< JQPublishObject > _pub;

private:
};
};  // namespace aiot
