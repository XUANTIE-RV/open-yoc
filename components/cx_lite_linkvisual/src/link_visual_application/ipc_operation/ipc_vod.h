/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef  IPC_VOD_H
#define  IPC_VOD_H

#include <tmedia_core/entity/format/format_inc.h>
#include <tmedia_core/entity/parser/parser_inc.h>
#include "ipc_defs.h"
#include "pthread.h"

class IpcVodUnit {
public:
    IpcVodUnit(VideoData video, AudioData audio, PropertyData property, RecordData record, VodCmdData vod_cmd);
    virtual ~IpcVodUnit();

    int Start(int service_id, unsigned int seek_timestamp_ms);
    int Pause(int pause);
    int Stop();
    int SetParam(unsigned int speed, unsigned int key_only);
    int GetParam(lv_audio_param_s &aparam, lv_video_param_s &vparam);
    int QueryList(const lv_query_record_param_s *param);
    int ParseOneAVFrame(uint64_t streaming_start_time);
private:
    int       pause_;
    VideoData video_data_;
    AudioData audio_data_;
    PropertyData property_data_;
    RecordData record_data_;
    VodCmdData vod_cmd_data_;

    const int MAX_RECORD_NUM_PERDAY = 100;
    unsigned int check_day_start_time;

    struct TaskParams {
        int service_id;
        IpcVodUnit *ptr;
        string record_name;
        int task_running;
        unsigned int absolute_time_s;
        unsigned int file_begin_time;
    };
    static void *VodTask(void *args);
    TaskParams  mTaskParam;
    pthread_cond_t  mSignal;
    pthread_mutex_t mLock;

    TMTsDemuxer *mDemuxer;
    TMParser *mH264Parser;
}; 

#endif // IPC_VOD_H