/**
 * @file snapshot.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_RECORD_SNAPSHOT_H
#define CX_RECORD_SNAPSHOT_H

#include <memory>
#include <cx/source/vid_channel.h>
#include <atomic>

namespace cx {
namespace record {

typedef void (*snapshot_callback_t)(string file_path, string file_name, string type);
class RecorderManager;

class Snapshot final {
public:
    Snapshot(source::VidChannelConfig &vidConfig, RecorderStorage &storage, int chanID);
    ~Snapshot();
    void SetCallback(RecorderManager *manager, snapshot_callback_t cb);
    int trigger(int num, uint32_t intervalMs, int quality);
    bool IsSnapshoting();
    
private:

    enum VidTaskStat : int {
        VID_TASK_IDLE = 0,
        VID_TASK_RUNNING,
    };
    std::atomic<VidTaskStat>        mTaskStat;

    std::shared_ptr<source::VidChannel> mPictureChan;
    RecorderStorage                     &mStorage;
    RecorderManager*                    mRecordManager;
    snapshot_callback_t                 mSnapshotCalllback;
};


}
} 



#endif /* CX_RECORD_SNAPSHOT_H */

