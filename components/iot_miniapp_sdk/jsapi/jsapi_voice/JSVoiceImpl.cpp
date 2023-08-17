#include <ulog/ulog.h>
#include <yoc/aui_cloud.h>
#include "JSVoiceImpl.h"

#define TAG "JSVoice"

#define SESSION_STATE_IDLE  0
#define SESSION_STATE_START 1
#define SESSION_STATE_WWV   2


typedef struct mic {
    int           mute_state;
}_mic;
static _mic my_mic;


namespace aiot
{
wakeInfo _wakeInof;
/**
 * 初始化
*/
void JSVoiceImpl::init(JQuick::sp<JQPublishObject> pub)
{
    _pub = pub;
    LOGD(TAG, "call %s", __func__);
    my_mic.mute_state = 0;
}

/**
 * 请求云端TTS
 */
int JSVoiceImpl::reqTTS(std::string text)
{
    LOGD(TAG, "call %s text:%s", __func__, text.c_str());

    /* 句柄 NULL，使用默认实例 */
    int ret = aui_cloud_req_tts(NULL, text.c_str());

    return ret;
}

/**
 * 设置静默模式
 * @param status 1:进入静默 0:退出静默
 */
int JSVoiceImpl::setMute(int status)
{
    LOGD(TAG, "call %s", __func__);
    // TODO
    if(!aui_mic_control(MIC_CTRL_VOICE_MUTE, status))
    {
        my_mic.mute_state = status;
        LOGD(TAG, "set mute success!!");
        return 0;
    }
    return -1;
}

/**
 * 获取当前是否静默模式
 * @return  1:静默模式 0:非静默模式
 */
int JSVoiceImpl::getMute(void)
{
    LOGD(TAG, "call %s", __func__);
    // TODO
   return my_mic.mute_state;
}

/**
 * 进入唤醒状态
 */
int JSVoiceImpl::pushToTalk(void)
{
    LOGD(TAG, "call %s", __func__);
    // TODO
    aui_mic_control(MIC_CTRL_START_SESSION, 0);
    aui_mic_control(MIC_CTRL_START_SESSION, 1);
    return 0;
}

/**
 * 停止向云端发送数据
 */
int JSVoiceImpl::stopTalk(void)
{
    LOGD(TAG, "call %s", __func__);
    // TODO
    aui_mic_control(MIC_CTRL_START_SESSION, 0);
    return 0;
}

}  // namespace aiot

