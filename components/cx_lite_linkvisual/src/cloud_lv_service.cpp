/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include "link_visual_enum.h"
#include "linkkit_client.h"
#include "linkvisual_client.h"
#ifdef DUMMY_IPC
#include "dummy_ipc.h"
#else
#include "normal_ipc.h"
#endif
#include "sdk_assistant.h"
#include "iot_import.h"
#include <cx/cloud_lv_service.h>
#include <lv_internal_api.h>
#include <cx/framework/service.h>
#include <cx/framework/client.h>
#include <cx/init/svr_config_parser.h>
#include <cx/source/vid_manager.h>
#include <cx/source/aud_manager.h>
#include <cx/common/json.h>
#include <cx/common/debug.h>
#include "link_visual_api.h"
#include "link_visual_struct.h"

#define TAG "cloud_lv"

using namespace std;
using namespace cx::framework;
using namespace cx;
using namespace cx::source;
using namespace cx::init;

struct LinkvisualConfig {
    VidChannelConfig vidConfig[3];
    AudChannelConfig audConfig;
};
static LinkvisualConfig linkvisualConfig;
static int chanLiveHD, chanLiveSD, chanLiveLD, audioChanLive;
shared_ptr<cx::source::VidChannel> channelLiveHD, channelLiveSD, channelLiveLD;
shared_ptr<cx::source::AudChannel> channelLiveAudio;
DEFINE_API_IMPL(cx_lv_start)
{
    virtual int Invoke(ServiceManager *svrManager, void *args) override
    {
        auto tplArgs = reinterpret_cast<std::tuple<cx_lv_callback_cb> *>(args);

        auto cb = std::get<0>(*tplArgs);
        lv_event_callback_register(cb);

        channelLiveHD = VidSrcManager::GetInstance()->CreateChannel(chanLiveHD);
        channelLiveSD = VidSrcManager::GetInstance()->CreateChannel(chanLiveSD);
        channelLiveLD = VidSrcManager::GetInstance()->CreateChannel(chanLiveLD);
        channelLiveAudio = AudSrcManager::GetInstance()->CreateChannel(audioChanLive);

        return 0;
    }
};
DEFINE_API_IMPL(cx_lv_stop)
{
    virtual int Invoke(ServiceManager *svrManager, void *args) override
    {
        channelLiveHD->Close();
        channelLiveSD->Close();
        channelLiveLD->Close();
        channelLiveAudio->Close();

        return 0;
    }
};
DEFINE_API_IMPL(cx_lv_connect)
{
    virtual int Invoke(ServiceManager *svrManager, void *args) override
    {
        int ret = 0;
        lv_device_auth_s auth_ipc;
        lv_log_level_e log_level = LV_LOG_DEBUG;

        char product_key[PRODUCT_KEY_LEN + 1];
        char device_name[DEVICE_NAME_LEN + 1];
        char device_secret[DEVICE_SECRET_LEN + 1];
        char product_secret[PRODUCT_SECRET_LEN + 1];
        if ((0 == HAL_GetProductKey(product_key)) || (0 == HAL_GetProductSecret(product_secret)) ||
            (0 == HAL_GetDeviceSecret(device_secret)) || (0 == HAL_GetDeviceName(device_name))) {
            CX_LOGE(TAG, "!!!!!! missing ProductKey/ProductSecret/DeviceName/DeviceSecret\n");
            return -1;
        }
    
        CX_LOGD(TAG, "device_name:%s", device_name);
        CX_LOGD(TAG, "device_secret:%s", device_secret);
        CX_LOGD(TAG, "product_key:%s", product_key);
        CX_LOGD(TAG, "product_secret:%s", product_secret);

        auth_ipc.dev_id = 0;//主设备-ipc
        auth_ipc.product_key = product_key;
        auth_ipc.device_name = device_name;
        auth_ipc.device_secret = device_secret;

#ifdef DUMMY_IPC//如果支持宏LINKKIT_INSTANCE
        /* 0.1.初始化虚拟IPC的功能 */

        IpcUnitParam unit;//设置单个IPC的属性
        memset(&unit, 0, sizeof(IpcUnitParam));
        unit.auth = &auth_ipc;

        DummyIpcConfig dummy_ipc_config = {0};//设置整个SDK需要支持的IPC属性
        dummy_ipc_config.ipc_num = 1;
        dummy_ipc_config.ipc_series = &unit;
        unit.picture_interval_s = 30;//设置虚拟IPC生成图片事件的间隔
        dummy_ipc_config.audio_handler = linkvisual_client_audio_handler;//设置虚拟IPC生成音频数据的输入函数
        dummy_ipc_config.video_handler = linkvisual_client_video_handler;//设置虚拟IPC生成视频数据的输入函数
        dummy_ipc_config.picture_handler = linkvisual_client_picture_handler;//设置虚拟IPC生成图片事件数据的输入函数
        dummy_ipc_config.query_record_handler = linkvisual_client_query_record;//设置虚拟IPC查询卡录像的输入函数
        dummy_ipc_config.set_property_handler = linkkit_client_set_property_handler;//设置虚拟IPC生成属性设置的输入函数
        dummy_ipc_config.vod_cmd_handler = linkvisual_client_vod_handler;

        ret = dummy_ipc_start(&dummy_ipc_config);
        if (ret < 0) {
            printf("Start dummy ipc failed\n");
            return -1;
        }

        /* 0.2.初始化在linkvisual中使用虚拟IPC的功能 */
        ret = linkvisual_client_assistant_start(0, &auth_ipc, 0, NULL);
        if (ret < 0) {
            printf("Start linkvisual client dummy ipc assistant failed\n");
            dummy_ipc_stop();
            return -1;
        }
#else
        IpcUnitParam unit;//设置单个IPC的属性
        memset(&unit, 0, sizeof(IpcUnitParam));
        unit.auth = &auth_ipc;
        NormalIpcConfig normal_ipc_config = {0};//设置整个SDK需要支持的IPC属性
        normal_ipc_config.ipc_num = 1;
        normal_ipc_config.ipc_series = &unit;
        unit.picture_interval_s = 30;
        // unit.picture_source = g_image_file;
        normal_ipc_config.audio_handler = linkvisual_client_audio_handler;
        normal_ipc_config.video_handler = linkvisual_client_video_handler;
        normal_ipc_config.picture_handler = linkvisual_client_picture_handler;
        normal_ipc_config.query_record_handler = linkvisual_client_query_record;
        normal_ipc_config.set_property_handler = linkvisual_get_property_handler();
        normal_ipc_config.vod_cmd_handler = linkvisual_client_vod_handler;
        ret = normal_ipc_start(&normal_ipc_config);
        if (ret < 0) {
            printf("Start normal ipc failed\n");
            return -1;
        }

        /* 0.2.初始化在linkvisual中使用IPC的功能 */
        ret = linkvisual_client_assistant_start(0, &auth_ipc, 0, NULL);
        if (ret < 0) {
            printf("Start linkvisual client normal ipc assistant failed\n");
            normal_ipc_stop();
            return -1;
        }
#endif // DUMMY_IPC

        /* 1. 初始化LinkVisual的资源 */
        ret = linkvisual_client_init(0, 0, log_level);
        if (ret < 0) {
            printf("linkvisual_client_init failed\n");
            linkvisual_client_assistant_stop();
#ifdef DUMMY_IPC
            dummy_ipc_stop();
#else
            normal_ipc_stop();
#endif
            return -1;
        }
#if (defined LINKVISUAL_WITH_LINKKIT) && LINKVISUAL_WITH_LINKKIT
        /* 2. 初始化linkkit的资源，并长连接到服务器 */
        iotx_linkkit_dev_meta_info_t auth = {0};
        string_safe_copy(auth.product_key, product_key, PRODUCT_KEY_LEN);
        string_safe_copy(auth.product_secret, product_secret, PRODUCT_SECRET_LEN);
        string_safe_copy(auth.device_name, device_name, DEVICE_NAME_LEN);
        string_safe_copy(auth.device_secret, device_secret, DEVICE_SECRET_LEN);
        ret = linkkit_client_start(&auth, 0, NULL);
        if (ret < 0) {
            printf("linkkit_client_start failed\n");
            linkvisual_client_destroy();
            linkvisual_client_assistant_stop();
#ifdef DUMMY_IPC
            dummy_ipc_stop();
#else
            normal_ipc_stop();
#endif
            return -1;
        }
#endif
        return 0;
    }
};
DEFINE_API_IMPL(cx_lv_disconnect)
{
    virtual int Invoke(ServiceManager *svrManager, void *args) override
    {
        /* linkkit长连接断开，并释放资源 */
        linkkit_client_destroy();

        /* LinkVisual断开音视频连接，并释放资源 */
        linkvisual_client_destroy();

        linkvisual_client_assistant_stop();
#ifdef DUMMY_IPC
        /* 停止虚拟IPC的功能 */
        dummy_ipc_stop();
#else
        normal_ipc_stop();
#endif // DUMMY_IPC
        return 0;
    }
};

DEFINE_API_IMPL(cx_lv_post_alarm_event)
{
    virtual int Invoke(ServiceManager *svrManager, void *args) override
    {
        auto tplArgs = reinterpret_cast<std::tuple<cx_lv_alarm_param_t *> *>(args);
        auto param = std::get<0>(*tplArgs);

        lv_device_auth_s auth;
        linkkit_get_auth(0, &auth, NULL);

        if(param->event_type == CX_LV_MOTION_DETECTED_EVENT) {
            string data = "test";
            lv_intelligent_alarm_param_s alarm_param;
            int service_id;
            memset(&alarm_param, 0, sizeof(lv_intelligent_alarm_param_s));
            switch (param->media_format) {
                case CX_MEIDA_JPEG:
                    alarm_param.format = LV_MEDIA_JPEG;
                break;
                default:
                CX_LOGE(TAG, "unknown media_format %d", param->media_format);
                return -1;
                break;
            }
            alarm_param.type = LV_INTELLIGENT_EVENT_MOVING_CHECK;
            alarm_param.media.p = param->media.p;
            alarm_param.media.len = param->media.len;
            alarm_param.addition_string.p = const_cast<char*>(data.c_str());
            alarm_param.addition_string.len = data.size();
            lv_post_intelligent_alarm(&auth, &alarm_param, &service_id);
            CX_LOGD(TAG, "lv_post_intelligent_alarm, service id = %d", service_id);
        } else {
            CX_LOGE(TAG, "unknown event_type %d", param->event_type);
            return -1;
        }
        return 0;
    }
};

class ServiceManagerCloud : public ServiceBackend {
public:
    ServiceManagerCloud() : ServiceBackend("linkvisual") {}
protected:
    virtual int ServiceInit(void) override
    {
        REGISTER_SERVICE_API(cx_lv_start);
        REGISTER_SERVICE_API(cx_lv_stop);
        REGISTER_SERVICE_API(cx_lv_connect);
        REGISTER_SERVICE_API(cx_lv_disconnect);
        REGISTER_SERVICE_API(cx_lv_post_alarm_event);
        return 0;
    }
};

/**
 ************************************user api side*************************************
 */
static cxlv_hdl_t lv_hdls[] = {NULL};

cxlv_hdl_t cx_lv_start(const char *lv_name, cx_lv_callback_cb cb)
{
    if (lv_hdls[0]) {
        return lv_hdls[0];
    }

    auto client = new ServiceClient(new ServiceManagerCloud);
    cx::ThreadAttributes attrs;
    attrs.name      = "lv_proxy";
    attrs.stackSize = 20 * 1024;
    attrs.prio      = 30;

    int ret = client->Init(lv_name, &attrs);
    if (ret < 0) {
        CX_LOGE(TAG, "client init failed");
        return nullptr;
    }
    auto args = make_tuple(cb);
    client->Request(__FUNCTION__, reinterpret_cast<void *>(&args), -1);
    CX_LOGD(TAG, "cx_lv_start");


    lv_hdls[0] = reinterpret_cast<cxlv_hdl_t>(client);
    return lv_hdls[0];
}

int cx_lv_stop(cxlv_hdl_t hdl)
{
    CXCHECK_PARAM(hdl && hdl == lv_hdls[0], -1);

    lv_hdls[0] = NULL;
    auto client = reinterpret_cast<ServiceClient *>(hdl);
    int ret = client->Request(__FUNCTION__, nullptr, -1);
    client->Deinit();
    delete client;

    return ret;
}

int cx_lv_connect(cxlv_hdl_t hdl)
{
    CXCHECK_PARAM(hdl && hdl == lv_hdls[0], -1);

    auto client = reinterpret_cast<ServiceClient *>(hdl);
    return client->Request(__FUNCTION__, NULL, -1);
}

int cx_lv_disconnect(cxlv_hdl_t hdl)
{
    CXCHECK_PARAM(hdl && hdl == lv_hdls[0], -1);

    auto client = reinterpret_cast<ServiceClient *>(hdl);
    return client->Request(__FUNCTION__, NULL, -1);
}

int cx_lv_post_alarm_event(cxlv_hdl_t hdl, cx_lv_alarm_param_t *param)
{
    CXCHECK_PARAM(hdl && param && hdl == lv_hdls[0], -1);

    auto client = reinterpret_cast<ServiceClient *>(hdl);
    auto args = std::make_tuple(param);
    return client->Request(__FUNCTION__, reinterpret_cast<void *>(&args), -1);
}

REGISTER_SERVICE_CONFIG(
    link_visual, 
    [](int svrIndex, Json obj) 
    {
        if (svrIndex == 0) {
            /* default values */
            ServiceConfigParser::GetVidSourceConfig(obj, 0, linkvisualConfig.vidConfig[0]);
            chanLiveHD = VidSrcManager::GetInstance()->AddChannelConfig(linkvisualConfig.vidConfig[0]);
            CX_LOGD(TAG, "vid config[0] sensor=%s size=%u*%u fps=%d encoder=%d", 
                    linkvisualConfig.vidConfig[0].vidInput->sensorTag.name.c_str(), 
                    linkvisualConfig.vidConfig[0].outputSize.width,
                    linkvisualConfig.vidConfig[0].outputSize.height, 
                    linkvisualConfig.vidConfig[0].outputFps, 
                    (int)linkvisualConfig.vidConfig[0].encoder);
            ServiceConfigParser::GetVidSourceConfig(obj, 1, linkvisualConfig.vidConfig[1]);
            chanLiveSD = VidSrcManager::GetInstance()->AddChannelConfig(linkvisualConfig.vidConfig[1]);
            CX_LOGD(TAG, "vid config[1] sensor=%s size=%u*%u fps=%d encoder=%d", 
                    linkvisualConfig.vidConfig[1].vidInput->sensorTag.name.c_str(), 
                    linkvisualConfig.vidConfig[1].outputSize.width,
                    linkvisualConfig.vidConfig[1].outputSize.height, 
                    linkvisualConfig.vidConfig[1].outputFps, 
                    (int)linkvisualConfig.vidConfig[1].encoder);
            ServiceConfigParser::GetVidSourceConfig(obj, 2, linkvisualConfig.vidConfig[2]);
            chanLiveLD = VidSrcManager::GetInstance()->AddChannelConfig(linkvisualConfig.vidConfig[2]);
            CX_LOGD(TAG, "vid config[2] sensor=%s size=%u*%u fps=%d encoder=%d", 
                    linkvisualConfig.vidConfig[2].vidInput->sensorTag.name.c_str(), 
                    linkvisualConfig.vidConfig[2].outputSize.width,
                    linkvisualConfig.vidConfig[2].outputSize.height, 
                    linkvisualConfig.vidConfig[2].outputFps, 
                    (int)linkvisualConfig.vidConfig[2].encoder);
            ServiceConfigParser::GetAudSourceConfig(obj, 0, linkvisualConfig.audConfig);
            audioChanLive = AudSrcManager::GetInstance()->AddChannelConfig(linkvisualConfig.audConfig);
            CX_LOGD(TAG, "aud config encoder=%d sample_bits=%d sample_rate=%d channels=%d data_type=%d", 
                    (int)linkvisualConfig.audConfig.encoder, 
                    (int)linkvisualConfig.audConfig.sampleBits, 
                    linkvisualConfig.audConfig.sampleRate, 
                    linkvisualConfig.audConfig.sampleChannels, 
                    linkvisualConfig.audConfig.pcmDataType);
        } else {
            cout << "not support linkvisual" << svrIndex << endl;
        }
    }
)