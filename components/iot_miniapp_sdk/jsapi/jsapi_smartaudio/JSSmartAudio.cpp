#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "jqutil_v2/jqutil.h"
#include "JSSmartAudioImpl.h"

#define TAG "JSSmartAudio"

static std::vector<void* > g_jsAudio_obj;

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
class JSSmartAudio : public JQPublishObject
{
public:
    void OnCtor(JQFunctionInfo &info);  
    void OnGCCollect();

    void play(JQAsyncInfo &info);
    void stop(JQAsyncInfo &info);
    void pause(JQAsyncInfo &info);
    void resume(JQAsyncInfo &info);
    void resumeEnable(JQAsyncInfo &info);
    void mute(JQAsyncInfo &info);
    void unmute(JQAsyncInfo &info);
    void setVol(JQAsyncInfo &info);
    void seek(JQAsyncInfo &info);
    void setSpeed(JQAsyncInfo &info);


    void getVol(JQFunctionInfo &info);
    void getSpeed(JQFunctionInfo &info);
    void getStatus(JQFunctionInfo &info);
    void getCurTime(JQFunctionInfo &info);
    void getUrl(JQFunctionInfo &info);
    void getDuration(JQFunctionInfo &info);
private:
    JSSmartAudioImpl _impl;
};

void JSSmartAudio::OnCtor(JQFunctionInfo &info)
{
    _impl.init(this);
    LOGD(TAG, ">>>>> JSAudio::OnCtor %x", this);

    jsapi_obj_lock();
    g_jsAudio_obj.push_back(this);
    jsapi_obj_unlock();
}

void JSSmartAudio::OnGCCollect(){
    LOGD(TAG, ">>>>> JSAudio::OnGCCollect");

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); ){
        if (*itor == this) {
            itor = g_jsAudio_obj.erase(itor);
        } else {
            itor++;
        }
    }
    jsapi_obj_unlock();
}

void JSSmartAudio::play(JQAsyncInfo &info){
    std::string url = info[0].string_value();
    int type = info[1].int_value();
    //smta.play(system,1);smta.play(system,1,0);smta.play(system,1,1);
    //smta.play(music,0);
    //第三个参数（默认为0），只对系统提示音有效，代表放完后是否恢复被打断的音乐
    int resume=0;
    if(info.Length()>2){
        resume=info[2].int_value();

    }
    int ret = _impl.play(url, type,resume);
    info.post(ret);
}

void JSSmartAudio::stop(JQAsyncInfo &info){
    int ret = _impl.stop();
    info.post(ret);
}

void JSSmartAudio::pause(JQAsyncInfo &info){
    int ret = _impl.pause();
    info.post(ret);
}

void JSSmartAudio::resume(JQAsyncInfo &info){
    int ret = _impl.resume();
    info.post(ret);
}

void JSSmartAudio::resumeEnable(JQAsyncInfo &info){
    int enable = info[0].int_value();
    int ret = _impl.resumeEnable(enable);
    info.post(ret);
}

void JSSmartAudio::mute(JQAsyncInfo &info){
    int ret = _impl.mute();
    if(!ret){
        this->publish(SMTA_EVENT_MUTE, Bson(true));
    }
    info.post(0);
}

void JSSmartAudio::unmute(JQAsyncInfo &info){
    int ret = _impl.unmute();
    if(!ret){
        this->publish(SMTA_EVENT_MUTE, Bson(false));
    }
    info.post(0);
}

    
void JSSmartAudio::setVol(JQAsyncInfo &info){
    VolumeRecord record;
    int newVol = info[0].int_value();
    record.oldVol = _impl.getVol();
    record.newVol = newVol;

    if(_impl.setVol(newVol)){
        this->publish(SMTA_EVENT_VOL_CHANGE, record.toBson());    
    }
    info.post(0);
}
    
void JSSmartAudio::seek(JQAsyncInfo &info){
    int ms = info[0].int_value();
    int ret = _impl.seek(ms);
    info.post(ret);
}
    
void JSSmartAudio::setSpeed(JQAsyncInfo &info){
    double speed = info[0].number_value();
    int ret = _impl.setSpeed(speed);
    info.post(ret);
}

void JSSmartAudio::getVol(JQFunctionInfo &info){
    int ret = _impl.getVol();
    info.GetReturnValue().Set(ret);
}
    
void JSSmartAudio::getSpeed(JQFunctionInfo &info){
    double ret = _impl.getSpeed();
    LOGD(TAG, "JS get speed %f", ret);
    info.GetReturnValue().Set(ret);
}
    
void JSSmartAudio::getStatus(JQFunctionInfo &info){
    SmtaPlayerStatus ret = _impl.getStatus();
    info.GetReturnValue().Set(ret);
}

void JSSmartAudio::getCurTime(JQFunctionInfo &info){
    int ret = _impl.getCurTime();
    info.GetReturnValue().Set(ret);
}

void JSSmartAudio::getUrl(JQFunctionInfo &info){
    char *url=NULL;
    _impl.getUrl(&url);
    if (url){
        info.GetReturnValue().Set(url);
    }
    else{
        info.GetReturnValue().Set("name");

    }
}

void JSSmartAudio::getDuration(JQFunctionInfo &info){
    int  duration=0;
    _impl.getDuration(&duration);
    info.GetReturnValue().Set(duration);
}


static JSValue createJSSmartAudio(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "smta");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
        return new JSSmartAudio();
    });

    tpl->SetProtoMethodPromise("play", &JSSmartAudio::play);
    tpl->SetProtoMethodPromise("stop", &JSSmartAudio::stop);
    tpl->SetProtoMethodPromise("pause", &JSSmartAudio::pause);
    tpl->SetProtoMethodPromise("resume", &JSSmartAudio::resume);
    tpl->SetProtoMethodPromise("resumeEnable", &JSSmartAudio::resumeEnable);
    tpl->SetProtoMethodPromise("mute", &JSSmartAudio::mute);
    tpl->SetProtoMethodPromise("unmute", &JSSmartAudio::unmute);
    tpl->SetProtoMethodPromise("setVol", &JSSmartAudio::setVol);
    tpl->SetProtoMethodPromise("seek", &JSSmartAudio::seek);
    tpl->SetProtoMethodPromise("setSpeed", &JSSmartAudio::setSpeed);
    
    tpl->SetProtoMethod("getVol", &JSSmartAudio::getVol);
    tpl->SetProtoMethod("getSpeed", &JSSmartAudio::getSpeed);
    tpl->SetProtoMethod("getStatus", &JSSmartAudio::getStatus);
    tpl->SetProtoMethod("getCurTime", &JSSmartAudio::getCurTime);
    tpl->SetProtoMethod("getUrl", &JSSmartAudio::getUrl);
    tpl->SetProtoMethod("getDuration", &JSSmartAudio::getDuration);
   
    JQPublishObject::InitTpl(tpl);
    
    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_smta(JQModuleEnv* env)
{
    // 返回模块
    return createJSSmartAudio(env);
}
}  // namespace aiot

extern "C" void jsapi_audio_publish_error(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); itor++){
        aiot::JSSmartAudio * self = (aiot::JSSmartAudio *)(*itor);
        self->publish(aiot::SMTA_EVENT_ERROR , Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_audio_publish_start(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); itor++){
        aiot::JSSmartAudio * self = (aiot::JSSmartAudio *)(*itor);
        self->publish(aiot::SMTA_EVENT_START , Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_audio_publish_pause(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); itor++){
        aiot::JSSmartAudio * self = (aiot::JSSmartAudio *)(*itor);
        self->publish(aiot::SMTA_EVENT_PAUSE , Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_audio_publish_resume(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); itor++){
        aiot::JSSmartAudio * self = (aiot::JSSmartAudio *)(*itor);
        self->publish(aiot::SMTA_EVENT_RESUME , Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_audio_publish_stop(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); itor++){
        aiot::JSSmartAudio * self = (aiot::JSSmartAudio *)(*itor);
        self->publish(aiot::SMTA_EVENT_STOP , Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_audio_publish_finish(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsAudio_obj.begin(); itor != g_jsAudio_obj.end(); itor++){
        aiot::JSSmartAudio * self = (aiot::JSSmartAudio *)(*itor);
        self->publish(aiot::SMTA_EVENT_FINISH , Bson());
    }
    jsapi_obj_unlock();
}
