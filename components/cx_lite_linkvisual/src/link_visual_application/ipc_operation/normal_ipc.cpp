/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "normal_ipc.h"

#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>

#include "link_visual_struct.h"
#include "ipc_unit.h"
#include <cx/cloud_lv_service.h>

static bool g_init = false;
static NormalIpcConfig g_config = {0};
static std::list<IpcUnit *> g_ipc;

int normal_ipc_start(const NormalIpcConfig *config) {
    if (g_init) {
        return 0;
    }

    if (!config) {
        return -1;
    }
    memcpy(&g_config, config, sizeof(NormalIpcConfig));

    if (config->ipc_num == 0) {
        printf("Ipc num illegal\n");
        return -1;
    }

    for (unsigned int i = 0; i < config->ipc_num; i++) {
        std::vector<std::string> live_vector(2);
        std::vector<std::string> vod_vector(2);
        std::string pic;
        IpcUnit *unit = new IpcUnit(config->video_handler, config->audio_handler, config->picture_handler, config->set_property_handler, config->query_record_handler, config->vod_cmd_handler);
        // if (config->ipc_series[i].live_num != 2) {
        //     live_vector[0] = g_default_files[0];
        //     live_vector[1] = g_default_files[1];
        // } else {
        //     live_vector[0] = config->ipc_series[i].live_source[0].source_file;//未做验空
        //     live_vector[1] = config->ipc_series[i].live_source[1].source_file;
        // }
        // for(unsigned int j = 0; j < config->ipc_series[i].vod_num; j++) {
        //     vod_vector[j] = config->ipc_series[i].vod_source[j].source_file;
        // }
        if (config->ipc_series[i].picture_source) {
            pic = config->ipc_series[i].picture_source;
        } 
        live_vector[0] = "lv_live_main";
        live_vector[1] = "lv_live_sub";
        if (unit->Start(live_vector, vod_vector, pic, config->ipc_series[0].picture_interval_s) < 0) {
            printf("Start ipc unit failed\n");
            return -1;//失败的时候未删除资源
        }
        if (unit->SetDevAuth(config->ipc_series[i].auth) < 0) {
            printf("Ipc unit auth failed\n");
            return -1;
        }
        g_ipc.push_back(unit);
    }

    g_init = true;

    return 0;
}

void normal_ipc_stop() {
    if (!g_init) {
        return;
    }

    for (std::list<IpcUnit *>::iterator it = g_ipc.begin(); it != g_ipc.end(); ++it) {
        (*it)->Stop();
        delete (*it);
    }
    g_ipc.clear();
    g_init = false;
}

extern cx_lv_callback_cb lv_event_cb;
int normal_ipc_live(const lv_device_auth_s *auth, NormalIpcMediaCmd cmd, const NormalIpcMediaParam *param) {
    if (!g_init) {
        return -1;
    }
    if (!param) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    /* 直播流一直处于打开状态，仅在SDK的连接建立时，才对外抛出数据 */
    if (cmd == NORMAL_IPC_MEDIA_START) {
        lv_event_cb(CXLV_EVENT_LIVE_START, nullptr);
        (*it)->LiveStart(param->stream_type, param->service_id);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_STOP){
        lv_event_cb(CXLV_EVENT_LIVE_STOP, nullptr);
        (*it)->LiveStop(param->service_id);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_REQUEST_I_FRAME) {
        (*it)->LiveRequestIFrame(param->service_id);
        return 0;
    }

    return 0;
}

int normal_ipc_live_get_media_param(const lv_device_auth_s *auth, int service_id, lv_audio_param_s *aparam, lv_video_param_s *vparam) {
    if (!g_init) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    return (*it)->LiveGetParam(service_id, aparam, vparam);
}

int normal_ipc_vod_get_media_param(const lv_device_auth_s *auth, lv_video_param_s *vparam, lv_audio_param_s *aparam) {
    if (!g_init) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    return (*it)->VodGetParam(aparam, vparam);
}

int normal_ipc_vod_by_utc(const lv_device_auth_s *auth, NormalIpcMediaCmd cmd, const NormalIpcMediaParam *param) {
    if (!g_init) {
        return -1;
    }
    if (!param) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    if (cmd == NORMAL_IPC_MEDIA_STOP){
        (*it)->VodStop();
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_PAUSE){
        (*it)->VodPause(true);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_UNPAUSE){
        (*it)->VodPause(false);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_SEEK) {
        (*it)->VodSeek(param->service_id, param->seek_timestamp_ms);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_SET_PARAM) {
        (*it)->VodSetParam(param->speed, param->key_only);
        return 0;
    }

    return 0;
}

int normal_ipc_vod_by_file(const lv_device_auth_s *auth, NormalIpcMediaCmd cmd, const NormalIpcMediaParam *param) {
    if (!g_init) {
        return -1;
    }
    if (!param) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    if (cmd == NORMAL_IPC_MEDIA_STOP){
        (*it)->VodStop();
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_PAUSE){
        (*it)->VodPause(true);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_UNPAUSE){
        (*it)->VodPause(false);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_SEEK) {
        (*it)->VodSeekFile(param->service_id, param->seek_timestamp_ms, param->source_file);
        return 0;
    }

    if (cmd == NORMAL_IPC_MEDIA_SET_PARAM) {
        (*it)->VodSetParam(param->speed, param->key_only);
        return 0;
    }

    return 0;
}

void normal_ipc_report_vod_list(const lv_device_auth_s *auth, const lv_query_record_param_s *param) {
    if (!g_init) {
        return;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return;
    }

    (*it)->VodQueryList(param);
}

int normal_ipc_set_property(const lv_device_auth_s *auth, const char *key, const char *value) {
    if (!g_init) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    (*it)->SetProperty(key== NULL?"":key, value== NULL?"":value);
    return 0;
}

void normal_ipc_get_all_property(const lv_device_auth_s *auth) {
    if (!g_init) {
        return;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return;
    }

    (*it)->GetAllProperty();
}

int normal_ipc_capture(const lv_device_auth_s *auth, unsigned char **data, unsigned int *data_len) {
    if (!g_init) {
        return -1;
    }

    std::list<IpcUnit *>::iterator it = g_ipc.begin();
    bool find = false;
    for (;it != g_ipc.end(); ++it) {
        if ((*it)->DevAuthCheck(auth)) {
            find = true;
            break;
        }
    }

    if (!find) {
        printf("Ipc not exist\n");
        return -1;
    }

    return (*it)->GetPicture(data, data_len);
}







