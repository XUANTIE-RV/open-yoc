/**
 * @file video.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_RECORD_VIDEO_H
#define CX_RECORD_VIDEO_H

#include <memory>
#include <atomic>
#include <cx/record/storage.h>
#include <cx/source/vid_channel.h>
#include <cx/source/aud_channel.h>
#include <tmedia_core/entity/format/format_inc.h>

namespace cx {
namespace record {

typedef void (*record_callback_t)(string file_path, string file_name, string type);
class RecorderManager;

class Video final {
public:
    Video(source::VidChannelConfig &vidConfig, RecorderStorage &storage, int videoChanID, int audioChanID);
    ~Video();

    void SetCallback(RecorderManager *manager, record_callback_t cb);
    int Start(bool autoStop, uint32_t timeoutMs);
    int Stop();
    bool IsRecording();
    
private:
    struct TaskParams {
        bool autoStop;
        uint32_t timeoutMs;
        Video *ptr;
    };

    enum VidTaskStat : int {
        VID_TASK_IDLE = 0,
        VID_TASK_RUNNING,
        VID_TASK_STOP,
    };

    static void *RecTask(void *args);
    TaskParams                      mTaskParam;
    std::atomic<VidTaskStat>        mTaskStat;

    pthread_cond_t                  mSignal;
    pthread_mutex_t                 mLock;
    
    std::shared_ptr<source::VidChannel> mVideoChan;
    std::shared_ptr<source::AudChannel> mAudioChan;
    std::unique_ptr<TMFormatMuxer>          mMuxer;

    RecorderStorage                     &mStorage;
    RecorderManager*                    mRecordManager;
    record_callback_t                   mRecordCalllback;
};


}
} 



#endif /* CX_RECORD_VIDEO_H */

