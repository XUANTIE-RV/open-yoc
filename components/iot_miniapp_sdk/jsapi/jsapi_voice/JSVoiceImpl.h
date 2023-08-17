#pragma once
#include <string>

#include <yoc/mic.h>
#include <yoc/mic_port.h>
#include <smart_audio.h>
#include "jqutil_v2/jqutil.h"
#include "jsapi_publish.h"

#include <yoc/aui_cloud.h>

using namespace jqutil_v2;

namespace aiot
{
typedef struct wakeInfo
{
    std::string tag;
    int score;
    inline Bson toBson()
    {
        Bson::object result;
        result["tag"] = tag;
        result["score"] = score;
        return result;
    }
}wakeInfo;

/* JS方法实现 */
class JSVoiceImpl : public JQPublishObject
{
public:
    void init(JQuick::sp<JQPublishObject> pub);
    
    int reqTTS(std::string text);
    int setMute(int status);
    int getMute(void);
    int pushToTalk(void);
    int stopTalk(void);

    // void publishEvent(const char* event);

private:
    JQuick::sp<JQPublishObject> _pub;
    
};
/* JS事件定义 */
static std::string VOICE_EVENT_SESSION_BEGIN = "sessionBegin";
static std::string VOICE_EVENT_SESSION_END = "sessionEnd";
static std::string VOICE_EVENT_SHORTCUT_CMD = "shortcutCMD";
static std::string VOICE_EVENT_NLP_BEGIN = "nlpBegin";
static std::string VOICE_EVENT_NLP_END = "nlpEnd";
}  // namespace aiot