#include "JSScreenImpl.h"
#include  <ulog/ulog.h>
#include <aos/kv.h>
#include <aos/cli.h>

#ifdef __cplusplus
extern "C" {
#endif
extern void jquick_set_screen_on(int ON);
#ifdef __cplusplus
}
#endif

namespace aiot
{
static  ScreenStatusInfo screenStatusInfo;
static  ScreenManageInfo screenManageInfo;
static  aos_timer_t screen_timer;
static  aos_timer_t screen_timer2;
static rvm_dev_t *    disp_dev;
static rvm_dev_t *    touch_dev;

static void touchscreen_event(rvm_dev_t *dev, int event_id, void *obj)
{
    JSScreenImpl *Obj = (JSScreenImpl*)obj;
    if(screenManageInfo.isAutoOff){
        // 刷新定时器
        aos_timer_stop(&screen_timer);
        aos_timer_change(&screen_timer, screenManageInfo.autoOffTimeout*1000);
        aos_timer_start(&screen_timer);
    }
    if(!screenStatusInfo.isOn){
        // 当屏幕关闭时打开屏幕
        Obj->turnOn();
    }
}
// 这个定时器是用来关闭屏幕的
static void screen_timer_cb(void *_pub, void* obj)
{
    JSScreenImpl* Obj = (JSScreenImpl*)obj;
    Obj->turnOff();
    aos_timer_stop(&screen_timer);
    printf("timer handler\r\n");
}
// 这个定时器是用来关闭防止误触的
static void screen_timer_cb2(void *_pub, void* obj)
{
    jquick_set_screen_on(1);
    aos_timer_stop(&screen_timer2);
}

void JSScreenImpl::init(JQuick::sp< JQPublishObject > pub)
{
    _pub = pub;

    // register event ,
    touch_dev = rvm_hal_input_open("input");
    rvm_hal_input_set_event(touch_dev, touchscreen_event, this);
    disp_dev = rvm_hal_display_open("disp");
    // 默认屏幕打开状态
    screenStatusInfo.isOn = true;
    screenManageInfo.isOn = true;
    _pub->publish(SCREEN_EVENT_STATUS_CHANGE, screenStatusInfo.toBson());
    
    int buf;
    if(aos_kv_getint(KV_SCREEN_brightness.c_str(), &buf)){
        //获取当前亮度
        rvm_hal_display_get_brightness(disp_dev,&screenManageInfo.brightness);    
        screenManageInfo.brightness = screenManageInfo.brightness /  2.25;
        aos_kv_setint(KV_SCREEN_brightness.c_str(), screenManageInfo.brightness);
    }else{
        rvm_hal_display_set_brightness(disp_dev, buf*2.25);
        screenManageInfo.brightness = buf;
    }
    memset(&buf, 0, sizeof(int));

    if(aos_kv_getint(KV_SCREEN_isAutoBrightness.c_str(), &buf)){
        //默认关闭自动调光 
        screenManageInfo.isAutoBrightness = false;
        aos_kv_setint(KV_SCREEN_isAutoBrightness.c_str(), 0);
    }else{
        screenManageInfo.isAutoBrightness = buf;
    }
    memset(&buf, 0, sizeof(int));
    if(aos_kv_getint(KV_SCREEN_isAutoOff.c_str(), &buf)){
        //默认关闭自动息屏
        screenManageInfo.isAutoOff = false;
        aos_kv_setint(KV_SCREEN_isAutoOff.c_str(), 0);
    }else{
        screenManageInfo.isAutoOff = buf;
    }
    memset(&buf, 0, sizeof(int));

    if(aos_kv_getint(KV_SCREEN_autoOffTimeout.c_str(), &buf)){
        //默认息屏时间为30秒
        screenManageInfo.autoOffTimeout = 30;
        aos_kv_setint(KV_SCREEN_autoOffTimeout.c_str(), 30);
    }else{
        screenManageInfo.autoOffTimeout = buf;
    }
    memset(&buf, 0, sizeof(int));

    LOGD(TAG, "isOn:%d,isAutoBrightness:%d,brightness:%d,autoOffTimeout%d,isAutoOff%d",
    screenManageInfo.isOn, screenManageInfo.isAutoBrightness,
    screenManageInfo.brightness, screenManageInfo.autoOffTimeout,
    screenManageInfo.isAutoOff);

    // 定义一个定时器 使用start 使用stop 控制
    int times = screenManageInfo.autoOffTimeout;
    if(!aos_timer_new_ext(&screen_timer, screen_timer_cb, this, times*1000 ,0, 0)){
        LOGD(TAG, "timer create success\r\n");}
    else{
        LOGD(TAG, "aos timer is error");
    }

    if(!aos_timer_new_ext(&screen_timer2, screen_timer_cb2, this, 700 ,0, 0)){
        LOGD(TAG, "timer create success\r\n");}
    else{
        LOGD(TAG, "aos timer is error");
    }
}

/**
 * 获取屏幕配置和状态
 * @param cb
 */
void JSScreenImpl::getInfo(ScreenCallback cb)
{
    // TODO  获取信息并回传
    //  MOCK data
    cb(screenManageInfo);
}

/**
 * 亮屏
 */
void JSScreenImpl::turnOn()
{
    // TODO turnOn
    // jquick_set_screen_on(1);
    // 打开一个定时器 定时500ms，之后打开jquick_set_screen_on(1)
    aos_timer_start(&screen_timer2);
    screenManageInfo.isOn = true;
    screenStatusInfo.isOn = true;
    rvm_hal_display_set_brightness(disp_dev, screenManageInfo.brightness*2.25);
    rvm_hal_display_blank_on_off(disp_dev, 1);
    LOGD(TAG, ">>>>>> screen on");
    _pub->publish(SCREEN_EVENT_STATUS_CHANGE, screenStatusInfo.toBson());
}

/**
 * 熄屏
 */
void JSScreenImpl::turnOff()
{
    // TODO turnOff
    jquick_set_screen_on(0);
    rvm_hal_display_blank_on_off(disp_dev, 0);
    screenManageInfo.isOn = false;
    screenStatusInfo.isOn = false;
    LOGD(TAG, ">>>>>> screen off");
    _pub->publish(SCREEN_EVENT_STATUS_CHANGE, screenStatusInfo.toBson());
}

/**
 * 设置是否启用自动亮度调节<无感光chip，无法使用改功能>
 * @param isAuto true-启用；false-禁用
 */
void JSScreenImpl::setAutoBrightness(bool isAuto)
{
    // TODO setAutoBrightness
    screenManageInfo.isAutoBrightness = isAuto;
}

/**
 * 设置亮度
 * @param percent 亮度值 范围0-100
 */
void JSScreenImpl::setBrightness(int percent)
{
    // TODO setBrightness
    rvm_hal_display_set_brightness(disp_dev, percent*2.25);
    screenManageInfo.brightness = percent;
    aos_kv_setint(KV_SCREEN_brightness.c_str(), percent);
}

/**
 * 设置是否启用闲置自动灭屏
 * @param isAuto true-启用；false-禁用
 */
void JSScreenImpl::setAutoOff(bool isAuto)
{
    // TODO setAutoOff
    screenManageInfo.isAutoOff = isAuto;
    LOGD(TAG, ">>>>>> %d", isAuto);

    // 关闭或者启动定时器
    if(isAuto){
        // 开启定时器后 改变软件定时器的周期。 注意：需要在定时器未启动状态是才能修改
        aos_timer_change(&screen_timer, screenManageInfo.autoOffTimeout*1000);
        if(!aos_timer_start(&screen_timer)){
            LOGD(TAG, "timer start success\r\n");
            aos_kv_setint(KV_SCREEN_isAutoOff.c_str(), 1);
        }else{
            LOGD(TAG, "aos_timer is error");
        }
    }else{
        aos_kv_setint(KV_SCREEN_isAutoOff.c_str(), 0);
        aos_timer_stop(&screen_timer);
    }
}

/**
 * 设置闲置自动灭屏时间，仅当启用自动灭屏时生效
 * @param time 单位：毫秒
 */
void JSScreenImpl::setAutoOffTimeout(int time)
{
    // TODO setAutoOffTimeout
    screenManageInfo.autoOffTimeout = time;
    if(screenManageInfo.isAutoOff){
        aos_kv_setint(KV_SCREEN_autoOffTimeout.c_str(), screenManageInfo.autoOffTimeout);
        aos_timer_stop(&screen_timer);
        aos_timer_change(&screen_timer, time*1000);
        aos_timer_start(&screen_timer);
    }else{
        LOGD(TAG, "no set auto off !!");
    }
}

}  // namespace aiot