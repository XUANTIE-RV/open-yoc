/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "media_parser.h"

#include <ulog/ulog.h>
#include "local_time.h"
#include "link_visual_api.h"
#include "sdk_assistant.h"
#include "posix/timer.h"
#include "lv_internal_api.h"
#include "lv_util.h"

#define TAG "media_parser"

struct IndexInfo {
    IndexInfo() {
        media_type = 0;//0-video, 1-audio
        len = 0;
        offset = 0;
        timestamp = 0;
        key_frame = 0;//only for video
    }
    unsigned int media_type;//0-video, 1-audio
    unsigned int len;
    unsigned int offset;
    unsigned int timestamp;
    unsigned int key_frame;//only for video
};

MediaParse::MediaParse(VideoBufferHandler video_handler, AudioBufferHandler audio_handler, VodCmdHandler vod_handler, void *arg) {
#ifdef DUMMY_IPC
    assert(video_handler != NULL);
    assert(audio_handler != NULL);
#endif
    index_position_ = 0;
    system_start_time_ = 0;
    media_start_time_ = 0;
    live_base_time_ = 100000;//做减法可能溢出，做简单处理
    vod_base_time_ = 0;
    service_id_ = 0;
    seek_time_ = 0;
    wait_for_i_frame_ = false;
    seek_ = false;
    pause_last_ = false;
    pause_ = false;
    stop_ = false;
    speed_operate_ = false;
    key_only_ = false;
    name_index_ = 0;
    send_running_ = false;
    vod_speed_ = 1;//1倍速，没有0倍速
    video_handler_ = video_handler;
    audio_handler_ = audio_handler;
    vod_handler_ = vod_handler;
    vod_segment_count = 0;
    vod_curent_segment = 0;
    vod_running_flag = 0;
    live_running_flag = 0;
    arg_ = arg;
    video_param_ = new lv_video_param_s;
    memset(video_param_, 0, sizeof(lv_video_param_s));
    audio_param_ = new lv_audio_param_s;
    memset(audio_param_, 0, sizeof(lv_audio_param_s));
    duration_ = 0;
    stream_video_file_ = NULL;
    stream_audio_file_ = NULL;
    buf_ = new unsigned char [kBufferMaxLen];
    thread_ = new ThreadEntry();

#ifndef DUMMY_IPC
    linkvisual_get_video_attributes(video_param_);
    linkvisual_get_audio_attributes(audio_param_);
#endif

    stream_file_ = nullptr;
}

MediaParse::~MediaParse() {
    Stop();
    delete video_param_;
    delete audio_param_;
    delete [] buf_;
    delete thread_;
}

int MediaParse::StartLive(int service_id, const std::string &stream_prefix) {
#ifdef DUMMY_IPC
    if (openAllFile(stream_prefix) < 0) {
        LOGE(TAG, "Open file failed");
        return -1;
    }
#endif
    stop_ = false;
    service_id_ = service_id;
    vod_base_time_ = 0;//不需要时间戳修正

    system_start_time_ = GetLocalTime();

    thread_->Start(liveThread, this, stream_prefix.c_str(), 40);

    return 0;
}

int MediaParse::GetIndexInfo(const std::string &stream_prefix, int seg_count)
{
#if 0
    LOGD(TAG, "%s", __func__);
    if (!index_.empty()) {
        std::vector<struct IndexInfo>().swap(index_);
    }
    if (!segment_index_bound_.empty()) {
        std::vector<uint32_t>().swap(segment_index_bound_);
    }
    unsigned int segment_index_len;
    cx_record_index_frame frame_info;
    uint16_t  calc_crc = 0, raw_crc = 0;
    struct IndexInfo index_info;

    stream_prefix_ = stream_prefix;
    std::string stream_index = stream_prefix_ + ".index";
    FILE *file_index = fopen(stream_index.c_str(), "r");
    if(!file_index) {
        LOGE(TAG, "%s %d open file:%s error", __func__, __LINE__, stream_index.c_str());
        return -1;
    }
    cx_record_index_head head_info;
    cx_record_index_segment_head segment_head;
    fread(&head_info, 1, sizeof(cx_record_index_head), file_index);

    for(unsigned int i=0;i<head_info.segment_num;i++) {
        fread(&segment_head, 1, sizeof(segment_head), file_index);
        LOGD(TAG, "segment:%d start time:%lld duration:%d pos:%d", i, segment_head.start_time, segment_head.duration, segment_head.next_segment_pos);
        segment_index_len = segment_head.next_segment_pos - ftell(file_index);
        calc_crc = 0;
        for(unsigned int j=0; j < segment_index_len/sizeof(cx_record_index_frame); j++) {
            fread(&frame_info, 1, sizeof(frame_info), file_index);
            index_info.media_type = frame_info.media_type;
            index_info.offset = frame_info.offset;
            index_info.len = frame_info.len;
            index_info.timestamp = frame_info.time_stamp;
            index_info.key_frame = frame_info.key_frame_flag;
            index_.push_back(index_info);
            calc_crc = cx_util_crc16(calc_crc, (unsigned char *)&frame_info, sizeof(frame_info));
        } 
        segment_index_bound_.push_back(index_.size());
        fread(&raw_crc, 1, sizeof(raw_crc), file_index);
        if(calc_crc != raw_crc) {
            LOGE(TAG, "!!!! %s %d crc err calc:0x%x raw:0x%x len:%d seg:%d", __func__, __LINE__, calc_crc, raw_crc, segment_index_len-1, i+1);
            fclose(file_index);
            std::vector<struct IndexInfo>().swap(index_);
            return -1;
        }
    } 
    fclose(file_index);
    if (index_.empty()) {
        LOGE(TAG, "Read index file failed: %s", stream_index.c_str());
        return -1;
    }
    vod_segment_count = seg_count;
#endif
    return 0;
}

int MediaParse::PreapareRecordRawData(int seg_index) {
    char stream_video_data[MAX_RECORD_NAME_LEN];
    char stream_audio_data[MAX_RECORD_NAME_LEN];
    LOGD(TAG, "%s", __func__);
    snprintf(stream_video_data, sizeof(stream_video_data), "%s_%02d.video", stream_prefix_.c_str(), seg_index);
    snprintf(stream_audio_data, sizeof(stream_audio_data), "%s_%02d.audio", stream_prefix_.c_str(), seg_index);
    stream_video_file_ = fopen(stream_video_data, "r");
    stream_audio_file_ = fopen(stream_audio_data, "r");
    if ((!stream_video_file_) || (!stream_audio_file_)) {
        LOGE(TAG, "Open file failed: %s %s", stream_video_data, stream_audio_data);
        return -1;
    }
    vod_curent_segment = seg_index;
    return 0;
}
#ifndef DUMMY_IPC
int MediaParse::StartVod(int service_id, uint64_t base_time) {
    stop_ = false;
    service_id_ = service_id;
    vod_base_time_ = base_time;
    system_start_time_ = GetLocalTime();
    LOGD(TAG, "StartVod base_time:%lld", vod_base_time_);

    char name[128];
    int ret = snprintf(name, sizeof(name), "%s_%02d", "lv_vod", service_id);
    if(ret < 0) {
        LOGE(TAG, "%d snprintf fail\n", __LINE__);
    }
    thread_->Start(vodThread, this, name, 30);
    vod_running_flag = 1;
    return 0;
}
#else
int MediaParse::StartVod(int service_id, const std::string &stream_prefix, uint64_t base_time) {
    if (openAllFile(stream_prefix) < 0) {
        printf("Open file failed\n");
        return -1;
    }

    stop_ = false;
    service_id_ = service_id;
    vod_base_time_ = base_time;
    system_start_time_ = GetLocalTime();
    speed_operate_ = false;
    key_only_ = false;
    char name[128] = "lv_vod";
    int ret = snprintf(name, sizeof(name), "%s_%02d", name, service_id);
    if(ret < 0) {
        LOGE(TAG, "%d snprintf fail\n", __LINE__);
    }
    thread_->Start(vodThread, this, name, 30);

    return 0;
}
#endif
int MediaParse::Pause(bool flag) {
    if (pause_ == flag) {
        return 0;
    }

    //例如暂停等操作，多线程下需要保证状态安全;此处在未响应完上个请求前，拒绝当前请求(但会导致命令无法快速响应）；
    // 也可以使用命令队列、用锁保护等
    pause_last_ = pause_;
    pause_ = flag;
#ifndef DUMMY_IPC
    if(flag) {
        while(send_running_) {    //等待发送退出
            LOGD(TAG, "still send wait..");
            usleep(10000);
        }
    }
#endif
    return 0;
}

void MediaParse::Stop() {
    stop_ = true;
    thread_->Stop();
    if (!index_.empty()) {
        std::vector<struct IndexInfo>().swap(index_);
    }
    index_position_ = 0;
#ifndef DUMMY_IPC
    if (stream_video_file_) {
        fclose(stream_video_file_);
        stream_video_file_ = NULL;
    }
    if (stream_audio_file_) {
        fclose(stream_audio_file_);
        stream_audio_file_ = NULL;
    }

    // memset(video_param_, 0, sizeof(lv_video_param_s));
    // memset(audio_param_, 0, sizeof(lv_audio_param_s));
    if(vod_running_flag) {
        vod_running_flag = 0;
    }
#else
    if (stream_file_) {
        fclose(stream_file_);
        stream_file_ = NULL;
    }
    memset(video_param_, 0, sizeof(lv_video_param_s));
    memset(audio_param_, 0, sizeof(lv_audio_param_s));
#endif
}

void MediaParse::Seek(unsigned int timestamp_ms) {
    seek_time_ = timestamp_ms;
    seek_ = true;
}

bool MediaParse::liveThread(void *parse) {
    assert(parse != NULL);

    MediaParse *media_parse = (MediaParse *)parse;

    while (media_parse->liveProcess() == 0 && !media_parse->stop_) {
        usleep(5000);
    }
    LOGD(TAG, "live quit %d", media_parse->stop_);
    media_parse->stop_ = true;

    return false;
}

bool MediaParse::vodThread(void *parse) {
    assert(parse != NULL);

    MediaParse *media_parse = (MediaParse *)parse;
    while (media_parse->vodProcess() == 0 && !media_parse->stop_) {
        usleep(20000);
    }
    LOGD(TAG, "vod quit %d", media_parse->stop_);
    media_parse->stop_ = true;

    return false;
}

int MediaParse::liveProcess() {
#ifdef DUMMY_IPC
    if (!stream_file_) {
        return -1;
    }

    processLiveRequestIFrame();

    return processLiveFrames();
#else
	int ret = 0;
	int is_iframe;
    static int p_frame_count = 0;
    long long time_stamp_ms_audio, time_stamp_ms_video, time_stamp;
    static long long time_stamp_ms_audio_last = 0, time_stamp_ms_video_last = 0, time_stamp_last = 0;
    if (service_id_ <= 0) {
        time_stamp_ms_audio_last = time_stamp_ms_video_last = time_stamp_last = 0;
        return 0;
    } 

    // if (wait_for_i_frame_) {
    //     cx_video_seek_iframe(1100);
    //     wait_for_i_frame_ = false;
    // }
    ret = linkvisual_get_encoded_video(buf_, kBufferMaxLen, &is_iframe, &time_stamp_ms_video);
    if((ret > 0) && (service_id_ > 0) && (live_running_flag)) {
        //发送video
        if(time_stamp_ms_video < time_stamp_last) {
            if((time_stamp_last - time_stamp_ms_video) < 40) {
                time_stamp_ms_video = time_stamp_last+1;
            } else {
                printf("drop video frame\n");
                goto audio_process;
            }
        }
        video_handler_(arg_, video_param_->format, buf_, ret, time_stamp_ms_video, is_iframe, service_id_);
        if(is_iframe) {
            // LOGD(TAG, "lv_stream_send_video i frame p frame count:%d %d", p_frame_count, decode_offset);
            p_frame_count = 0;
        } else {
            p_frame_count++;
        }
        if((time_stamp_ms_video - time_stamp_ms_video_last) > 60 && (time_stamp_ms_video_last != 0)) {
            LOGE(TAG, "video stamp too big %lld\n", time_stamp_ms_video - time_stamp_ms_video_last);
        }
        time_stamp_ms_video_last = time_stamp_ms_video;
        time_stamp = time_stamp_ms_video;
        if(time_stamp < time_stamp_last) {
            LOGE(TAG, "time stamp error %lld %lld", time_stamp_last, time_stamp);
        }
        time_stamp_last = time_stamp;
    }
audio_process:     
    ret = linkvisual_get_encoded_audio(buf_, kBufferMaxLen, &time_stamp_ms_audio);
    if((ret > 0) && (service_id_ > 0) && (live_running_flag)) {
        //发送audio
        // if(time_stamp_ms_audio < time_stamp_last) {
        //     if((time_stamp_last - time_stamp_ms_audio) < 40) {
        //         time_stamp_ms_audio = time_stamp_last+1;
        //     } else {
        //         printf("drop audio frame %lld %lld\n", time_stamp_last, time_stamp_ms_audio);
        //         return 0;
        //     }
        // }
        audio_handler_(arg_, audio_param_->format, buf_, ret, time_stamp_ms_audio, service_id_);
        if((time_stamp_ms_audio - time_stamp_ms_audio_last) > 60 && (time_stamp_ms_audio_last != 0)) {
            LOGE(TAG, "audio stamp too big %lld\n", time_stamp_ms_audio - time_stamp_ms_audio_last);
        }
        time_stamp_ms_audio_last = time_stamp_ms_audio;
        time_stamp = time_stamp_ms_audio;
        // if(time_stamp < time_stamp_last) {
        //     LOGE(TAG, "time stamp error %lld %lld", time_stamp_last, time_stamp);
        // }
        time_stamp_last = time_stamp;
    }

    return 0;
#endif
}

int MediaParse::vodProcess() {
#ifndef DUMMY_IPC
    if ((!stream_video_file_) || (!stream_audio_file_)) {
        return -1;
    }
#else
    if (!stream_file_) {
        return -1;
    }
#endif

    if (processPause() < 0) {
        return 0;
    }

    processSeek();

    processSpeed();

    processVodRequestIFrame();

    return processVodFrames();
}

int MediaParse::openParamFile(const std::string &stream_param) {
    FILE *fp = fopen(stream_param.c_str(), "r");
    if (!fp) {
        LOGE(TAG, "Open file failed: %s", stream_param.c_str());
        return -1;
    }

    int ret = 0;
    const char* split = "=";
    while (!feof(fp)) {
        char line[kIndexLineMaxLne];
        memset(line, 0, kIndexLineMaxLne);
        fgets(line, kIndexLineMaxLne, fp);
        char *key = strtok(line, split);
        if (!key) {
            continue;
        }
        char *value = strtok(NULL, "\r\n");
        if (!strcmp(key, "video.format")) {
            if (!strcmp(value, "h264")) {
                video_param_->format = LV_VIDEO_FORMAT_H264;
            } else if (!strcmp(value, "h265")) {
                video_param_->format = LV_VIDEO_FORMAT_H265;
            } else {
                LOGE(TAG, "Invalid param:  %s", value);
            }
        } else if (!strcmp(key, "video.fps")) {
            video_param_->fps = atoi(value);
        } else if (!strcmp(key, "audio.format")) {
            if (!strcmp(value, "aac")) {
                audio_param_->format = LV_AUDIO_FORMAT_AAC;
            } else if (!strcmp(value, "g711a")) {
                audio_param_->format = LV_AUDIO_FORMAT_G711A;
            } else if (!strcmp(value, "pcm")) {
                audio_param_->format = LV_AUDIO_FORMAT_PCM;
            } else if (!strcmp(value, "g711u")) {
                audio_param_->format = LV_AUDIO_FORMAT_G711U;
            } else {
                LOGE(TAG, "Invalid param:  %s", value);
            }
        } else if (!strcmp(key, "audio.sample_rate")) {
            switch (atoi(value)) {
                case 8000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_8000;
                    break;
                case 11025:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_11025;
                    break;
                case 12000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_12000;
                    break;
                case 16000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_16000;
                    break;
                case 22050:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_22050;
                    break;
                case 24000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_24000;
                    break;
                case 32000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_32000;
                    break;
                case 44100:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_44100;
                    break;
                case 48000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_48000;
                    break;
                case 64000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_64000;
                    break;
                case 88200:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_88200;
                    break;
                case 96000:
                    audio_param_->sample_rate = LV_AUDIO_SAMPLE_RATE_96000;
                    break;
                default:
                    LOGE(TAG, "Invalid param:  %s", value);
                    ret = -1;
                    break;
            }
        } else if (!strcmp(key, "audio.bits_per_sample")) {
            switch (atoi(value)) {
                case 8:
                    audio_param_->sample_bits = LV_AUDIO_SAMPLE_BITS_8BIT;
                    break;
                case 16:
                    audio_param_->sample_bits = LV_AUDIO_SAMPLE_BITS_16BIT;
                    break;
                default:
                    LOGE(TAG, "Invalid param:  %s", value);
                    ret = -1;
                    break;
            }
        } else if (!strcmp(key, "audio.channel")) {
            switch (atoi(value)) {
                case 1:
                    audio_param_->channel = LV_AUDIO_CHANNEL_MONO;
                    break;
                case 2:
                    audio_param_->channel = LV_AUDIO_CHANNEL_STEREO;
                    break;
                default:
                    LOGE(TAG, "Invalid param:  %s", value);
                    ret = -1;
                    break;
            }
        } else if (!strcmp(key, "duration")) {
            duration_ = atoi(value);
        } else {

        }
    }

    fclose(fp);
    return ret;
}

#ifdef DUMMY_IPC
int MediaParse::openIndexFile(const std::string &stream_index) {
    FILE *index_file = fopen(stream_index.c_str(), "r");
    if (!index_file) {
        printf("Open file failed: %s\n", stream_index.c_str());
        return -1;
    }

    int ret = 0;
    while (!feof(index_file)) {
        char line[kIndexLineMaxLne];
        memset(line, 0, kIndexLineMaxLne);
        struct IndexInfo index_info;

        fgets(line, kIndexLineMaxLne, index_file);
        ret = sscanf(line, "media=%d,offset=%d,len=%d,timestamp=%d,key=%d",
                     &index_info.media_type, &index_info.offset, &index_info.len, &index_info.timestamp, &index_info.key_frame);
        if (ret != 5) {
            printf("Read file failed: %s\n", stream_index.c_str());
            fclose(index_file);
            return -1;
        }
        index_.push_back(index_info);
    }
    fclose(index_file);

    if (index_.empty()) {
        printf("Read index file failed: %s\n", stream_index.c_str());
        return -1;
    }

    return 0;
}
int MediaParse::openStreamFile(const std::string &stream_file) {
    stream_file_ = fopen(stream_file.c_str(), "r");
    if (!stream_file_) {
        printf("Open file failed: %s\n", stream_file.c_str());
        return -1;
    }

    return 0;
}

int MediaParse::openAllFile(const std::string &stream_prefix) {
    std::string stream_index = stream_prefix + ".index";
    if (openIndexFile(stream_index) < 0) {
        printf("Parse stream index failed\n");
        return -1;
    }

    std::string stream_param = stream_prefix + ".meta";
    if (openParamFile(stream_param) < 0) {
        printf("Parse stream param failed\n");
        return -1;
    }

    if (openStreamFile(stream_prefix) < 0) {
        printf("Parse stream file failed\n");
        return -1;
    }

    stream_prefix_ = stream_prefix;
    return 0;
}
#endif

void MediaParse::GetParam(lv_video_param_s &video_param, lv_audio_param_s &audio_param) const {
    video_param.format = video_param_->format;
    video_param.fps = video_param_->fps;
    video_param.key_frame_interval_ms = video_param_->key_frame_interval_ms;
    video_param.duration = duration_;

    audio_param.format = audio_param_->format;
    audio_param.sample_rate = audio_param_->sample_rate;
    audio_param.sample_bits = audio_param_->sample_bits;
    audio_param.channel = audio_param_->channel;
}

void MediaParse::processSeek() {
    /* seek处理 */
    if (seek_) {
        if (index_[index_.size() - 1].timestamp < seek_time_) {
            seek_time_ = index_[index_.size() - 1].timestamp;//超过文件时间戳数据纠正
        }

        bool find = false;
        unsigned int i = 0;
        unsigned int last = 0;
        if (index_[index_position_].timestamp >= index_[0].timestamp) {
            system_start_time_ += index_[index_position_].timestamp - index_[0].timestamp;
        }
        for (i = 0; i + 1 < index_.size() ; i++) {
            if (index_[i].key_frame) {
                last = i;
            }
            if (index_[i].timestamp >= seek_time_ &&
                index_[i].key_frame) {
                find = true;
                break;
            }
        }
        if (find) {
            index_position_ = i;
        } else {
            index_position_ = last;//找不到则使用最后一个I帧
        }
#ifdef DUMMY_IPC
        fseek(stream_file_, index_[index_position_].offset, SEEK_SET);
#endif
        vodReset();
        LOGD(TAG, "processSeek %d pos:%d/%d set:%lldms real:%dms", find, index_position_, index_.size(), seek_time_, index_[index_position_].timestamp);
        seek_ = false;
    }
}

void MediaParse::processSpeed() {
    if (speed_operate_) {
        //切换倍速重新计时
        vodReset();
        speed_operate_ = false;
    }
}

int MediaParse::processPause() {
    if (pause_) {
        return -1;
    }
    if (pause_last_) {
        //从暂停恢复播放，重新计时
        vodReset();
    }
    pause_last_ = pause_;

    return 0;
}

void MediaParse::processLiveRequestIFrame() {
    /* 强制I帧处理 */
    if (wait_for_i_frame_) {
        uint64_t position = index_position_;
        bool forward = false;
        uint32_t i = 0;
        for (i = 0; i + index_position_ < index_.size(); i++) {
            if (index_[index_position_ + i].key_frame) {
                forward = true;
                break;
            }
        }
        if (forward) {
            index_position_ += i;
            fseek(stream_file_, index_[index_position_].offset, SEEK_SET);
        } else {
            bool back = false;
            for (i = 0; index_position_ > i; i++) {
                if (index_[index_position_ - i].key_frame) {
                    back = true;
                    break;
                }
            }
            if (back) {
                index_position_ -= i;
                fseek(stream_file_, index_[index_position_].offset, SEEK_SET);
            }
        }
        liveReset(position, index_position_);
        wait_for_i_frame_ = false;
    }
}

void MediaParse::processVodRequestIFrame() {
    /* 强制I帧处理 */
    if (wait_for_i_frame_) {
        bool forward = false;
        uint32_t i = 0;
        for (i = 0; i + index_position_ < index_.size(); i++) {
            if (index_[index_position_ + i].key_frame) {
                forward = true;
                break;
            }
        }
        if (forward) {
            index_position_ += i;
#ifdef DUMMY_IPC
            fseek(stream_file_, index_[index_position_].offset, SEEK_SET);
#endif
        } else {
            bool back = false;
            for (i = 0; index_position_ > i; i++) {
                if (index_[index_position_ - i].key_frame) {
                    back = true;
                    break;
                }
            }
            if (back) {
                index_position_ -= i;
#ifdef DUMMY_IPC
                fseek(stream_file_, index_[index_position_].offset, SEEK_SET);
#endif
            }
        }
        LOGD(TAG, "!!! processVodRequestIFrame %d", forward);
        vodReset();
        wait_for_i_frame_ = false;
    }
}

int MediaParse::processLiveFrames() {
    uint64_t current = GetLocalTime();
    bool finish = false;
    while (!finish) {
        if ((index_[index_position_].timestamp - media_start_time_) <= (current - system_start_time_)) {
            if (index_[index_position_].len > kBufferMaxLen) {
                LOGE(TAG, "processLiveFrames Buf is too large");
                return -1;
            }
            uint64_t read_bit = fread(buf_, 1, index_[index_position_].len, stream_file_);
            if (read_bit != index_[index_position_].len || ((unsigned int *)buf_)[0] != kMediaFrameHeader || read_bit < sizeof(kMediaFrameHeader)) {
                LOGE(TAG, "processLiveFrames Read frame failed");
                return -1;
            }

            if (index_[index_position_].media_type == 0) {
#if 0 //自定义SEI信息示例
            uint8_t* tmp_buf = new uint8_t[read_bit + 100];
            uint8_t sei_message[64];
            //sei start code
            sei_message[0] = sei_message[1] = 0;
            sei_message[2] = 1;
            //自定义SEI头
            sei_message[3] = 0x50;//H264时为0x06,H265时为0X50
            sei_message[4] = 'B';
            sei_message[5] = 'A';
            sei_message[6] = 'B';
            sei_message[7] = 'A';
            //SEI数据长度
            int len = 10;
            sei_message[8] = len>>24;
            sei_message[9] = (len&0xffffff)>>16;
            sei_message[10] = (len&0xffff)>>8;
            sei_message[11] = len&0xff;
            //SEI数据
            for (int i = 0; i < len; i++) {
                sei_message[12+i] = 'D' + i;
            }
            //SEI和数据帧拼接
            memcpy(tmp_buf, sei_message, 12+len);
            memcpy(tmp_buf+12+len, buf_ + sizeof(kMediaFrameHeader), read_bit - sizeof(kMediaFrameHeader));
            video_handler_(video_param_->format, tmp_buf, 12+len + read_bit - sizeof(kMediaFrameHeader), (unsigned int)(index_[index_position_].timestamp + live_base_time_), index_[index_position_].key_frame, service_id_);
            delete [] tmp_buf;
#else
                video_handler_(arg_, video_param_->format, buf_ + sizeof(kMediaFrameHeader), read_bit - sizeof(kMediaFrameHeader), (unsigned int)(index_[index_position_].timestamp + live_base_time_), index_[index_position_].key_frame, service_id_);
#endif
            } else if (index_[index_position_].media_type == 1) {
                audio_handler_(arg_, audio_param_->format, buf_+ sizeof(kMediaFrameHeader), read_bit - sizeof(kMediaFrameHeader), (unsigned int)(index_[index_position_].timestamp + live_base_time_), service_id_);
            }

            if (index_position_ + 1 >= index_.size()) {
                //重置为文件头
                liveReset(index_position_, 0);
                index_position_ = 0;
                fseek(stream_file_, 0, SEEK_SET);
                finish = true;
            } else {
                index_position_++;
            }
        } else {
            finish = true;
        }
    }

    return 0;
}

int MediaParse::processVodFrames() {
    uint64_t current = GetLocalTime();
    bool finish = false;
    int ret = -1;
#ifndef DUMMY_IPC
    static int p_frame_count = 0;
    send_running_ = 1;
    while (!finish) {
        if(pause_) {
            send_running_ = 0;
            return 0;
        }
        if(current < system_start_time_) {
            current = GetLocalTime();
        }
        if ((index_[index_position_].timestamp - media_start_time_) <= ((current - system_start_time_) * vod_speed_) || (index_[index_position_].timestamp < media_start_time_)) {
            if (index_[index_position_].len > kBufferMaxLen) {
                LOGE(TAG, "processVodFrames Buf is too large");
                send_running_ = 0;
                return -1;
            }
            for (int i = 0; i < 100; i++) {
                if (index_[index_position_].media_type == 0) {
                    if((unsigned int)ftell(stream_video_file_) != index_[index_position_].offset) {
                        fseek(stream_video_file_, index_[index_position_].offset, SEEK_SET);
                    }
                    uint64_t read_bit = fread(buf_, 1, index_[index_position_].len, stream_video_file_);
                    if(read_bit != index_[index_position_].len) {
                        printf("%s %d read err\n", __func__, __LINE__);
                    }
                    ret = video_handler_(arg_, video_param_->format, buf_, index_[index_position_].len, (unsigned int)(index_[index_position_].timestamp + vod_base_time_), index_[index_position_].key_frame, service_id_);
                    if(index_[index_position_].key_frame) {
                        // LOGD(TAG, "send i frame p frame count:%d", p_frame_count);
                        p_frame_count = 0;
                    } else {
                        p_frame_count++;
                    }
                } else if (index_[index_position_].media_type == 1) {
                    if((unsigned int)ftell(stream_audio_file_) != index_[index_position_].offset) {
                        fseek(stream_audio_file_, index_[index_position_].offset, SEEK_SET);
                    }
                    uint64_t read_bit = fread(buf_, 1, index_[index_position_].len, stream_audio_file_);
                    if(read_bit != index_[index_position_].len) {
                        printf("%s %d read err\n", __func__, __LINE__);
                    }
                    ret = audio_handler_(arg_, audio_param_->format, buf_, index_[index_position_].len, (unsigned int)(index_[index_position_].timestamp + vod_base_time_), service_id_);
                }
                if (LV_WARN_BUF_FULL != ret) {
                    break;
                } else {
                    printf("vod send frame error, will send again later\n");
                }
                usleep(40000);
            }
            if (LV_WARN_BUF_FULL == ret) {
                printf("vod send frame error, buf full!!!!!!\n");
            }
            if (index_position_ + 1 >= segment_index_bound_[vod_curent_segment - 1]) {
                fclose(stream_video_file_);
                fclose(stream_audio_file_);
                stream_video_file_ = stream_audio_file_ = NULL;
                if(vod_curent_segment < vod_segment_count) {
                    LOGD(TAG, "seg:%d/%d, index_position_:%d", vod_curent_segment + 1, vod_segment_count, index_position_);
                    PreapareRecordRawData(vod_curent_segment + 1);
                    index_position_++;
                } else {
                    send_running_ = 0;
                    return -1;
                }
                send_running_ = 0;
                return 0;
            } else {
                index_position_++;
            }
        } else {
            finish = true;
        }
    }

    send_running_ = 0;
#else
    while (!finish) {
        if ((index_[index_position_].timestamp - media_start_time_) <= ((current - system_start_time_) * vod_speed_)) {
            if (index_[index_position_].len > kBufferMaxLen) {
                printf("processVodFrames Buf is too large\n");
                return -1;
            }
            uint64_t read_bit = fread(buf_, 1, index_[index_position_].len, stream_file_);
            if (read_bit != index_[index_position_].len || ((unsigned int *)buf_)[0] != kMediaFrameHeader || read_bit < sizeof(kMediaFrameHeader)) {
                printf("processVodFrames Read frame failed\n");
                return -1;
            }
            //点播时可能返回LV_WARN_BUF_FULL，等待一会，继续发送，最多持续4秒
            for (int i = 0; i < 100; i++) {
                if (key_only_) {
                    if (index_[index_position_].media_type == 0 && index_[index_position_].key_frame) {
                        ret = video_handler_(arg_, video_param_->format, buf_ + sizeof(kMediaFrameHeader), read_bit - sizeof(kMediaFrameHeader), (unsigned int)(index_[index_position_].timestamp + vod_base_time_), index_[index_position_].key_frame, service_id_);
                    }
                } else {
                    if (index_[index_position_].media_type == 0) {
                        ret = video_handler_(arg_, video_param_->format,buf_ + sizeof(kMediaFrameHeader), read_bit - sizeof(kMediaFrameHeader), (unsigned int)(index_[index_position_].timestamp + vod_base_time_), index_[index_position_].key_frame, service_id_);
                    } else if (index_[index_position_].media_type == 1) {
                        ret = audio_handler_(arg_, audio_param_->format, buf_+ sizeof(kMediaFrameHeader), read_bit - sizeof(kMediaFrameHeader), (unsigned int)(index_[index_position_].timestamp + vod_base_time_), service_id_);
                    }
                }
                if (LV_WARN_BUF_FULL != ret) {
                    break;
                } else {
                    printf("vod send frame error, will send again later\n");
                }
                usleep(40000);
            }
    
            if (index_position_ + 1 >= index_.size()) {
                index_position_ = 0;
                vodReset();
                fseek(stream_file_, 0, SEEK_SET);
                vod_base_time_ += duration_ * 2000;
                printf("processVodFrames end\n");
            } else {
                index_position_++;
            }
        } else {
            finish = true;
        }
    }
#endif
    return 0;
}


int MediaParse::GetDuration(const std::string &stream_prefix, double &duration) {
    std::string stream_param = stream_prefix + ".meta";
    FILE *fp = fopen(stream_param.c_str(), "r");
    if (!fp) {
        LOGE(TAG, "Open file failed: %s", stream_param.c_str());
        return -1;
    }

    const char* split = "=";
    while (!feof(fp)) {
        char line[kIndexLineMaxLne];
        memset(line, 0, kIndexLineMaxLne);
        fgets(line, kIndexLineMaxLne, fp);
        char *key = strtok(line, split);
        char *value = strtok(NULL, "\r");
        if (!key || !value) {
            continue;
        }
        if (!strcmp(key, "duration")) {
            duration = atoi(value);
        } else {
            //LOGE(TAG, "Invalid line: %s", key);
        }
    }

    fclose(fp);
    return 0;
}

void MediaParse::liveReset(uint32_t position_last, uint32_t position_new) {
    uint64_t current = GetLocalTime();
    if (position_new > position_last) {
        live_base_time_ -= (index_[position_new].timestamp - index_[position_last].timestamp);
    } else {
        live_base_time_ += (index_[position_last].timestamp - index_[position_new].timestamp);
    }
    system_start_time_ = current;
    media_start_time_ = index_[position_new].timestamp;
}

void MediaParse::vodReset() {
    media_start_time_ = index_[index_position_].timestamp;
    system_start_time_ = GetLocalTime();
}

void MediaParse::SetSpeed(uint32_t speed, bool key_only) {
    if (speed_operate_) {
        return;
    }
    if (speed == 0 || speed > kVodSpeedMax) {
        return;
    }
    vod_speed_ = speed;
    if (key_only_ && !key_only) {
        wait_for_i_frame_ = true;//仅I帧切为非仅I帧时，需要寻找下一I帧
    }
    key_only_ = key_only;
    speed_operate_ = true;
}

