/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#ifndef HEVC_PARSE_H_
#define HEVC_PARSE_H_

#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>

#include "link_visual_struct.h"
#include "thread_entry.h"

typedef int (*VideoBufferHandler)(void *arg, lv_video_format_e format, unsigned char *buffer, unsigned int buffer_size,
                          unsigned int timestamp_ms, int nal_type, int service_id);
typedef int (*AudioBufferHandler)(void *arg, lv_audio_format_e format, unsigned char *buffer, unsigned int buffer_size,
                                   unsigned int timestamp_ms, int service_id);
typedef int (*VodCmdHandler)(void *arg, int vod_cmd, int param, int service_id);

struct IndexInfo;
class MediaParse {
public:
    MediaParse(VideoBufferHandler video_handler, AudioBufferHandler audio_handler, VodCmdHandler vod_handler, void *arg);
    virtual ~MediaParse();

    int StartLive(int service_id, const std::string &stream_prefix);
#ifndef DUMMY_IPC
    int StartVod(int service_id, uint64_t base_time);
#else 
    int StartVod(int service_id, const std::string &stream_prefix, uint64_t base_time);
#endif 
    int GetIndexInfo(const std::string &stream_prefix, int seg_count);

    int PreapareRecordRawData(int seg_index);

    int Pause(bool flag);

    void Stop();

    void Seek(unsigned int timestamp_ms);

    void RequestIFrame() {wait_for_i_frame_ = true;};

    void SetSpeed(unsigned int speed, bool key_only);

    void SetServiceId(int service_id) {service_id_ = service_id;};

    int GetServiceId() const {return service_id_;} ;

    bool GetStatus() const {return thread_->GetStatus();};

    std::string GetStreamPrefix() const {return stream_prefix_;};

    void GetParam(lv_video_param_s &video_param, lv_audio_param_s &audio_param) const;

    static int GetDuration(const std::string &stream_prefix, double &duration);

    void SetNameIndex(size_t index) {name_index_ = index;};

    unsigned int GetNameIndex() const {return name_index_;};

private:
    int openIndexFile(const std::string &stream_index);
    int openParamFile(const std::string &stream_param);
    int openStreamFile(const std::string &stream_file);
    int openAllFile(const std::string &stream_prefix);
    int liveProcess();
    int vodProcess();
    void processSeek();
    int processPause();
    void processSpeed();
    void processLiveRequestIFrame();
    void processVodRequestIFrame();
    int processLiveFrames();
    int processVodFrames();
    void liveReset(uint32_t position_last, uint32_t position_new);
    void vodReset();

private:
    static bool liveThread(void *parse);
    static bool vodThread(void *parse);
public:
    int vod_curent_segment;
    int vod_running_flag;
    int live_running_flag;

private:
    //媒体文件信息
    unsigned duration_;
    lv_video_param_s *video_param_;
    lv_audio_param_s *audio_param_;
    FILE *stream_file_;
    FILE *stream_video_file_;
    FILE *stream_audio_file_;
    unsigned char *buf_;
    std::string stream_prefix_;
    std::vector<struct IndexInfo> index_;
    std::vector<uint32_t> segment_index_bound_;
    //播放控制信息
    uint32_t index_position_;
    uint64_t system_start_time_;
    uint64_t media_start_time_;
    uint64_t seek_time_;
    bool pause_;
    bool send_running_;
    uint64_t live_base_time_;//直播循环时的时间戳补足
    uint64_t vod_base_time_;//点播的基础时间戳补足（比如第一个文件为0，第二个文件的补足是第一个文件的时长）
    bool wait_for_i_frame_;
    bool seek_;
    bool pause_last_;
    bool stop_;
    bool speed_operate_;
    bool key_only_;
    uint32_t vod_speed_;
    unsigned int name_index_;
    //回调信息
    int service_id_;
    VideoBufferHandler video_handler_;
    AudioBufferHandler audio_handler_;
    VodCmdHandler vod_handler_;
    void *arg_;
    int vod_segment_count;

    ThreadEntry *thread_;
private:
    static const unsigned int kBufferMaxLen = 500 * 1024;
    static const unsigned int kIndexLineMaxLne = 128;
    static const unsigned int kMillisecondPerSecond = 1000;
    static const unsigned int kMediaFrameHeader = 0xFCFCFCFC;
    static const unsigned int kVodSpeedMax = 16;
    static const int MAX_RECORD_NAME_LEN = 128;
};



#endif // HEVC_PARSE_H_