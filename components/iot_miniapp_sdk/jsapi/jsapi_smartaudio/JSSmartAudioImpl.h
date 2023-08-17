#pragma once
#include <string>
#include "jqutil_v2/jqutil.h"

#include <aos/kv.h>
#include <av/player.h>
#include <av/media.h>
#include <smart_audio.h>
#include <board.h>
#include <vector>
#include "jsapi_publish.h"
using namespace jqutil_v2;

#define VOLUME_SAVE_KV_NAME "volume"

namespace aiot
{
typedef struct _VolumeRecord {
    int oldVol;      
    int newVol;           
    inline Bson toBson()
    {
        Bson::object result;
        result["oldVol"] = oldVol;
        result["newVol"] = newVol;
        return result;
    }
} VolumeRecord;

typedef enum _PlayerStatus{
    UNKNOWN = 0,
    STOPED,
    PLAYING,
    PAUSED,
    MUTE,
} SmtaPlayerStatus;

/* JS方法实现 */
class JSSmartAudioImpl
{
public:
    void init(JQuick::sp<JQPublishObject> pub);

    int play(std::string url, int type, int resume);
    int stop(void);
    int pause(void);
    int resume(void);
    int resumeEnable(int enable);
    int mute(void);
    int unmute(void);
    int getVol(void);
    int setVol(int vol);
    int seek(int seek_time_ms);
    int setSpeed(double speed);
    float getSpeed(void);
    SmtaPlayerStatus getStatus(void);
    int getCurTime(void);
    void getUrl(char **url);
    void getDuration(int *duration);

    // bool isFinish(void);
    JQuick::sp<JQPublishObject> _pub;
private:
    
};

/* JS事件定义 */
static std::string SMTA_EVENT_ERROR = "error";
static std::string SMTA_EVENT_START = "start";
static std::string SMTA_EVENT_PAUSE = "pause";
static std::string SMTA_EVENT_RESUME = "resume";
static std::string SMTA_EVENT_FINISH = "finish";
static std::string SMTA_EVENT_STOP = "stop";
static std::string SMTA_EVENT_VOL_CHANGE = "vol_change";
static std::string SMTA_EVENT_MUTE = "mute";
}  // namespace aiot
