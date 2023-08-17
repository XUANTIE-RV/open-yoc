/**
 * @file manager.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_RECORD_MANAGER_H
#define CX_RECORD_MANAGER_H

#include <memory>
#include <string>
#include <cx/framework/service.h>
#include <cx/record/video.h>
#include <cx/record/snapshot.h>
#include <pthread.h>

namespace cx {
namespace record {

struct RecoderConfig {
    source::VidChannelConfig    vidRecordConfig;
    source::AudChannelConfig    audRecordConfig;
    source::VidChannelConfig    vidSnapshotConfig;
    uint32_t                    storageSpace;       // KB
    uint8_t                     storageRecycleLevel; // percentage, 0~100
    uint32_t                    segmentSize;        //KB
    std::string                 rootPath;
};

class RecorderManager final : public framework::ServiceManager {
public:
    RecorderManager();
    ~RecorderManager();

    virtual int ServiceInit() override {return 0;}
    virtual int ServiceDeinit() override {return 0;}
    void SetCallback(record_callback_t cb);
    int VideoStart(bool autoStop, uint32_t timeoutMs);
    int VideoStop(void);
    bool IsRecording(void);
    bool IsSnapshoting(void);

    int SnapShot(int num, uint32_t intervalMs, int quality);

    record::RecorderStorage &GetStorageManager() {
        return *mStorage;
    }
    
    static RecoderConfig            recConfig;

private:
    record::RecorderStorage             *mStorage;
    std::unique_ptr<Video>              mVideo;
    std::unique_ptr<Snapshot>           mSnapshot;
};

}
}



#endif /* CX_RECORD_MANAGER_H */

