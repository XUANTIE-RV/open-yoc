#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "jqutil_v2/jqutil.h"
#include "JSVoiceImpl.h"
#include <cJSON.h>

#define TAG "JSVoice"

static std::vector<void* > g_jsvoice_obj;

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
class JSVoice : public JQPublishObject
{
public:
    void OnCtor(JQFunctionInfo &info);
    void OnGCCollect();

    void reqTTS(JQAsyncInfo &info);
    void setMute(JQAsyncInfo &info);
    void getMute(JQFunctionInfo &info);
    void pushToTalk(JQAsyncInfo &info);
    void stopTalk(JQAsyncInfo &info);
private:
    JSVoiceImpl _impl;
};

void JSVoice::OnCtor(JQFunctionInfo &info){
    LOGD(TAG, ">>>>> JSVoice::OnCtor %x", this);
    
    jsapi_obj_lock();
    g_jsvoice_obj.push_back(this);
    jsapi_obj_unlock();
}

void JSVoice::OnGCCollect(){
    LOGD(TAG, ">>>>> JSVoice::OnGCCollect");
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsvoice_obj.begin(); itor != g_jsvoice_obj.end(); ){
        if (*itor == this) {
            itor = g_jsvoice_obj.erase(itor);
        } else {
            itor++;
        }
    }
    jsapi_obj_unlock();
}

void JSVoice::reqTTS(JQAsyncInfo &info){
    std::string text = info[0].string_value();
    if(text.empty()){
        info.postError("key should not be empty");
        info.post(-1);
        return;
    }
    int ret = _impl.reqTTS(text);
    info.post(ret);
}

void JSVoice::setMute(JQAsyncInfo &info){
    int status = info[0].int_value();
    int ret = _impl.setMute(status);
    info.post(ret);
}

void JSVoice::getMute(JQFunctionInfo &info){
    int status = _impl.getMute();
    info.GetReturnValue().Set(status);
}

void JSVoice::pushToTalk(JQAsyncInfo &info){
    int ret = _impl.pushToTalk();
    info.post(ret);
}

void JSVoice::stopTalk(JQAsyncInfo &info){
    int ret = _impl.stopTalk();
    info.post(ret);
}

static JSValue createJSVoice(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "voice");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
        return new JSVoice();
    });

       tpl->SetProtoMethodPromise("reqTTS", &JSVoice::reqTTS);
       tpl->SetProtoMethodPromise("setMute", &JSVoice::setMute);
       tpl->SetProtoMethodPromise("pushToTalk", &JSVoice::pushToTalk);
       tpl->SetProtoMethodPromise("stopTalk", &JSVoice::stopTalk);

       tpl->SetProtoMethod("getMute", &JSVoice::getMute);

       JQPublishObject::InitTpl(tpl);

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_voice(JQModuleEnv* env)
{
    // 返回模块
    return createJSVoice(env);
}
}  // namespace aiot

cJSON *cJSON_GetObjectItemByPath(cJSON *object, const char *path)
{
    if (object == NULL) {
        return NULL;
    }

    char *dup_path = strdup(path);
    char *saveptr  = NULL;
    char *next     = strtok_r(dup_path, ".", &saveptr);

    cJSON *item = object;
    while (next) {
        char *arr_ptr = strchr(next, '[');
        if (arr_ptr) {
            *arr_ptr++ = '\0';
            item = cJSON_GetObjectItem(item, next);
            if (item) {
                item = cJSON_GetArrayItem(item, atoi(arr_ptr));
            }
        } else {
            item = cJSON_GetObjectItem(item, next);
        }
        if (item == NULL) {
            break;
        }

        next = strtok_r(NULL, ".", &saveptr);
    }

    free(dup_path);

    return item;
}

int json_string_eq(cJSON *js, const char *str)
{
    if (cJSON_IsString(js)) {
        if (strcmp(js->valuestring, str) == 0) {
            return 1;
        }
    }
    return 0;
}

extern "C" void jsapi_voice_publish_sessionBegin(char* wk_word, int wk_score){
    aiot::wakeInfo _info;
    _info.tag = wk_word;
    _info.score = wk_score;

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsvoice_obj.begin(); itor != g_jsvoice_obj.end(); itor++){
        aiot::JSVoice * self = (aiot::JSVoice *)(*itor);
        LOGD(TAG, ">>>>> jsapi_voice_publish %x", self);
        self->publish(aiot::VOICE_EVENT_SESSION_BEGIN , _info.toBson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_voice_publish_sessionEnd(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsvoice_obj.begin(); itor != g_jsvoice_obj.end(); itor++){
        aiot::JSVoice * self = (aiot::JSVoice *)(*itor);
        LOGD(TAG, ">>>>> jsapi_voice_publish_sessionEnd %x", self);
        self->publish(aiot::VOICE_EVENT_SESSION_END, Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_voice_publish_shortcutCMD(cJSON *object, const char * resultCMD){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsvoice_obj.begin(); itor != g_jsvoice_obj.end(); itor++){

        aiot::JSVoice * self = (aiot::JSVoice *)(*itor);
        LOGD(TAG, ">>>>> jsapi_voice_publish_shortcutCMD %x", self);

        self->publish(aiot::VOICE_EVENT_SHORTCUT_CMD, resultCMD);
    }
    jsapi_obj_unlock();
}
extern "C" void jsapi_voice_publish_nlpBegin(){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsvoice_obj.begin(); itor != g_jsvoice_obj.end(); itor++){
        aiot::JSVoice * self = (aiot::JSVoice *)(*itor);
        LOGD(TAG, ">>>>> jsapi_voice_publish_nlpBegin %x", self);
        self->publish(aiot::VOICE_EVENT_NLP_BEGIN, Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_voice_publish_nlpEnd(const char * data){
    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsvoice_obj.begin(); itor != g_jsvoice_obj.end(); itor++){
        aiot::JSVoice * self = (aiot::JSVoice *)(*itor);
        LOGD(TAG, ">>>>> jsapi_voice_publish_nlpEnd %x", self);
        self->publish(aiot::VOICE_EVENT_NLP_END, data);
    }
    jsapi_obj_unlock();
}
