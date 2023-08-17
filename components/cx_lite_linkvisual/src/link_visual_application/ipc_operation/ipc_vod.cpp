/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include "local_time.h"
#include "link_visual_struct.h"
#include "posix/timer.h"
#include <cx/record.h>
#include "lv_internal_api.h"
#include "ipc_vod.h"
#include <cx/common/log.h>
#include <tmedia_core/entity/format/ts_demuxer.h>

#define TAG "ipc_vod"


int IpcVodUnit::ParseOneAVFrame(uint64_t streaming_start_time)
{
    int rc;
    TMPacket avPaket;
    avPaket.Init();

    rc = mDemuxer->RecvPacket(avPaket, 0);
    if (rc == TMResult::TM_OK) {
        uint8_t *data = avPaket.mData;
        size_t data_size = avPaket.mDataLength;
        unsigned int ts = avPaket.mPTS.Get().timestamp/(MPEG_TIME_BASE / 1000) + (mTaskParam.file_begin_time - check_day_start_time)*1000;

        if (avPaket.mStreamIndex != -1 && 
            (avPaket.mCodecID == TMMediaInfo::CodecID::H264 || avPaket.mCodecID == TMMediaInfo::CodecID::H265)) {
            while(data_size > 0) {
                TMVideoPacket videoPkt;
                rc = mH264Parser->Parse(videoPkt, data, data_size);
                if(rc >= 0) {
                    data += rc;
                    data_size -= rc;
                } else {
                    CX_LOGE(TAG, "parser read packet error, rc = %d", rc);
                    break;
                }
                if (videoPkt.mDataLength) {
                    int key_frame = (videoPkt.mPictureType == TMMediaInfo::PictureType::I) ? 1 : 0; 

                    if(mTaskParam.service_id > 0) {
                        rc = video_data_(mTaskParam.service_id, LV_VIDEO_FORMAT_H264, videoPkt.mData, 
                                    videoPkt.mDataLength, ts, key_frame);
                        if (LV_WARN_BUF_FULL == rc) {
                            CX_LOGE(TAG, "vod send frame error, will send again later\n");
                        }
                    }
                }

                videoPkt.UnRef();
            }
        } else if (avPaket.mCodecID == TMMediaInfo::CodecID::G711) {
            if(mTaskParam.service_id > 0) {
                rc = audio_data_(mTaskParam.service_id, LV_AUDIO_FORMAT_G711A, data, data_size, ts);
                if (LV_WARN_BUF_FULL == rc) {
                    CX_LOGE(TAG, "vod send frame error, will send again later\n");
                }
            }
        } else {
            CX_LOGE(TAG, "stream type not supported index=% codec=%d", avPaket.mStreamIndex, avPaket.mCodecID);
            rc = -1;
            goto END;
        }

        rc = 1;
    } else if (rc == TMResult::TM_TIMEOUT || rc == TMResult::TM_EAGAIN) {
        rc = 0;
    }

END:
    avPaket.UnRef();
    return rc;
}

IpcVodUnit::IpcVodUnit(VideoData video, AudioData audio, PropertyData property, RecordData record, VodCmdData vod_cmd)
{
    mDemuxer = static_cast<TMTsDemuxer *>(TMFormatDemuxerFactory::CreateEntity(TMMediaInfo::FormatID::TS));    
    mH264Parser = TMParserFactory::CreateEntity(TMMediaInfo::CodecID::H264);
    if(!mDemuxer || !mH264Parser) {
        CX_LOGE(TAG, "demuxer or parser create fail, %p %p", mDemuxer, mH264Parser);
    }
    pthread_condattr_t  cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_cond_init(&mSignal, &cond_attr);
    pthread_mutex_init(&mLock, 0);
    pthread_condattr_destroy(&cond_attr);

    video_data_ = video;
    audio_data_ = audio;
    property_data_ = property;
    record_data_ = record;
    vod_cmd_data_ = vod_cmd;

    memset((void *)&mTaskParam, 0, sizeof(TaskParams));
    check_day_start_time = 0;
    pause_ = 0;
}

IpcVodUnit::~IpcVodUnit() 
{
    pthread_cond_destroy(&mSignal);
    pthread_mutex_destroy(&mLock);
    delete mDemuxer;
    delete mH264Parser;
}

extern cx_dvr_hdl_t record_handle;
void *IpcVodUnit::VodTask(void *args)
{
    auto params = reinterpret_cast<TaskParams *>(args);
    auto ptr = params->ptr;
    int rc;
    TMPropertyList propList;
    FILE *fp;
    uint8_t buffer[188 * 5];
    TMPacket muxPaket;

    fp = fopen(params->record_name.c_str(), "r");
    if (!fp) {
        CX_LOGE(TAG, "vod cannot open file:%s", params->record_name.c_str());
        return nullptr;
    }

    propList.Add(TMProperty((int)TMTsDemuxer::PropID::TSDEMUXER_INBUF_SIZE,     50 * 1024));
    propList.Add(TMProperty((int)TMTsDemuxer::PropID::TSDEMUXER_OUTBUF_SIZE,    50 * 1024));
    rc = ptr->mDemuxer->Open("", &propList);
    if (rc != TMResult::TM_OK) {
        CX_LOGE(TAG, "open demuxer failed rc=%d", rc);
        return nullptr;
    }

    propList.Reset();
    propList.Add(TMProperty((uint32_t)TMParser::PropID::RETAIN_FRAME_SIZE,      100 * 1024));
    rc = ptr->mH264Parser->Open(&propList);
    if (rc != TMResult::TM_OK)
    {
        CX_LOGE(TAG, "h264parser open failed rc=%d", rc);
        ptr->mDemuxer->Close();
        return nullptr;
    }

    uint64_t system_start_time_ = GetLocalTime();

    CX_LOGD(TAG, "vod task enter");
    while(params->task_running) {
        if(ptr->pause_) {
            usleep(5000);
            continue;
        }

        size_t bytes = fread(buffer, 1, 188 * 5, fp);
        if (bytes == 0) {
            CX_LOGE(TAG, "read packet eof");
            break;
        } else if (bytes < 0) {
            CX_LOGE(TAG, "demuxer read packet error, rc = %d", bytes);
            break;
        }

        muxPaket.Init();
        muxPaket.PrepareBuffer(bytes);
        muxPaket.Copy(buffer, bytes);

        rc = ptr->mDemuxer->SendPacket(muxPaket, 20);
        muxPaket.UnRef();

        while (1) {
            rc = ptr->ParseOneAVFrame(system_start_time_);
            if (rc == 0) {
                break;
            } else if (rc < 0) {
                params->task_running = 0;
                break;
            }
        }
    }

    muxPaket.Free();
    ptr->mH264Parser->Close();
    ptr->mDemuxer->Close();
    fclose(fp);

    params->task_running = 0;
    pthread_mutex_lock(&ptr->mLock);
    pthread_cond_signal(&ptr->mSignal);
    pthread_mutex_unlock(&ptr->mLock);

    CX_LOGD(TAG, "vod task quit");
    return nullptr;
}

int IpcVodUnit::Start(int service_id, unsigned int seek_timestamp_ms)
{
    /* get record info */
    int record_exist = 0;
    string record_name;
    unsigned int absolute_time_s = check_day_start_time+seek_timestamp_ms/1000;
    cx_dvr_file_info_t infos[MAX_RECORD_NUM_PERDAY];
    int ret = cx_dvr_search(record_handle, check_day_start_time, check_day_start_time+seek_timestamp_ms/1000+1, CXDVR_FILE_TYPE_VIDEO, infos, MAX_RECORD_NUM_PERDAY);
    if(ret < 0) {
        CX_LOGE("cannot find record in:%s\n", __func__);
        return -1;
    }
    CX_LOGD(TAG, "search record in:%d in %d file", absolute_time_s, ret);
    int i;
    for(i = 0; i < ret; i++) {
        if(absolute_time_s >= infos[i].begin_time && absolute_time_s < infos[i].end_time) {
            record_name = infos[i].file_path;
            CX_LOGD(TAG, "find record file %s", record_name.c_str());
            record_exist = 1;
            break;
        }
    }
    if(record_exist) {
        if(mTaskParam.task_running == 1) {
            this->Stop();
        }
        pthread_t       tid;
        pthread_attr_t  attr;
        int ret = pthread_attr_init(&attr);
        if (ret < 0) {
            CX_LOGE(TAG, "Create thread attr failed, ret = %d", ret);
            return -1;
        }
        // attr.sched_priority = ;
        attr.stacksize = 8192;
        ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (ret < 0) {
            CX_LOGE(TAG, "Set thread attr failed, ret = %d", ret);
            return -1;
        }
        mTaskParam.ptr = this;
        mTaskParam.service_id = service_id;
        mTaskParam.record_name = record_name;
        mTaskParam.task_running = 1;
        mTaskParam.absolute_time_s = absolute_time_s;
        mTaskParam.file_begin_time = infos[i].begin_time;
        ret = pthread_create(&tid, &attr, &VodTask, &mTaskParam);
        if (ret != 0) {
            CX_LOGE(TAG, "Create thread failed, ret = %d", ret);
            mTaskParam.task_running = 0;
            return -1;
        }
        pthread_setname_np(tid, "vod");
    } else {
        CX_LOGE(TAG, "cannot find record in %d", absolute_time_s);
    }
    return 0;
}
 
int IpcVodUnit::Pause(int pause)
{
    CX_LOGD(TAG, "pause %d", pause);
    pause_ = pause;
    return 0;
}

int IpcVodUnit::Stop()
{
    if(mTaskParam.task_running) {
        mTaskParam.task_running = 0;
        pthread_mutex_lock(&mLock);
        pthread_cond_wait(&mSignal, &mLock);
        pthread_mutex_unlock(&mLock);
    }
    CX_LOGD(TAG, "stop");
    return 0;
}

int IpcVodUnit::QueryList(const lv_query_record_param_s *param)
{
    if (!param) {
        return -1;
    }
    if (param->common.type == LV_QUERY_RECORD_BY_DAY) {
        lv_query_record_response_param_s *response = (lv_query_record_response_param_s *)malloc(sizeof(lv_query_record_response_param_s));
        memset(response, 0, sizeof(lv_query_record_response_param_s));
/*save day start time*/
        time_t utime = param->by_day.start_time;
        struct tm *tm = NULL;
        tm = localtime(&utime);
        check_day_start_time = utime - (tm->tm_hour*3600+tm->tm_min*60+tm->tm_sec);
/* find record */
        cx_dvr_file_info_t infos[MAX_RECORD_NUM_PERDAY];
        int ret = cx_dvr_search(record_handle, param->by_day.start_time, param->by_day.stop_time, CXDVR_FILE_TYPE_VIDEO, infos, MAX_RECORD_NUM_PERDAY);
        if(ret < 0) {
            CX_LOGE("cannot find record in:%s\n", __func__);
            return -1;
        }
        response->by_day.days = (lv_query_record_response_day *)malloc(sizeof(lv_query_record_response_day) *ret);
        memset(response->by_day.days, 0, sizeof(lv_query_record_response_day) * ret);
        for(int i=0; i < ret; i++) {
            response->by_day.days[i].file_size = infos[i].size;
            response->by_day.days[i].record_type = LV_STORAGE_RECORD_INITIATIVE;
            response->by_day.days[i].file_name = (char *)malloc(strlen(infos[i].file_name));
            strncpy(response->by_day.days[i].file_name, infos[i].file_name, strlen(infos[i].file_name));
            response->by_day.days[i].start_time = infos[i].begin_time;
            response->by_day.days[i].stop_time = infos[i].end_time;
        }
        response->by_day.num = ret;
        record_data_(param->common.service_id, response);
        for (unsigned int i = 0; i < response->by_day.num; i++) {
            free(response->by_day.days[i].file_name);
        }
        free(response->by_day.days);
        free(response);
    } else {
        CX_LOGE(TAG, "unsupported query:%d", param->common.type);
    }
    return 0;
}

int IpcVodUnit::SetParam(unsigned int speed, unsigned int key_only)
{
    CX_LOGD(TAG, "%s %d", __func__, __LINE__);
    return 0;
}

int IpcVodUnit::GetParam(lv_audio_param_s &aparam, lv_video_param_s &vparam)
{
    linkvisual_get_video_attributes(&vparam);
    linkvisual_get_audio_attributes(&aparam);
    return 0;
}
