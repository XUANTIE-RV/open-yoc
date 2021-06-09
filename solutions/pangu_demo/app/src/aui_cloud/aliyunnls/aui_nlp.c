/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"
#include "app_main.h"
#include <cJSON.h>
#include <sys/time.h>
#include <devices/wifi.h>
#include <avutil/named_straightfifo.h>
#include <media.h>
#include <cJSON.h>
#include <yoc/aui_cloud_mit.h>

#define TAG "auinlp"

/* tts event */
#define EVENT_TTS_FINISHED              (1 << 0)
#define EVENT_TTS_PLAYER_FINISHED       (1 << 1)

/* wwv event */
#define EVENT_WWV_CONFIRMED             (1 << 0)
#define EVENT_WWV_REJECTED              (1 << 1)

/* tts running state */
#define TTS_STATE_IDLE                   0
#define TTS_STATE_RUN                    1
#define TTS_STATE_PLAY                   2

/* ai engine */
static aui_t       *g_aui_handler;
static aos_event_t  event_tts_state;
static aos_event_t  event_wwv_result;
static nsfifo_t    *aui_fifo    = NULL;
static bool         player_running = false;
static char        *mit_tts_fifo = NULL;


static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

/* 处理云端反馈的 NLP 文件，进行解析处理 */

int aui_kws_proc_mit(cJSON *js, const char *json_text)
{
//{\"aui_kws_result\":%d,\"msg\":宝拉宝拉};
    int ret = 0;
    cJSON *state = cJSON_GetObjectItem(js, "aui_kws_result");

    if (!state) {
        return -1;
    }

    int event = atoi(state->valuestring);

    switch (event) {
        case AUI_KWS_REJECT:
            LOGD(TAG, "wwv rejected");
            aos_event_set(&event_wwv_result, EVENT_WWV_REJECTED, AOS_EVENT_OR);
            break;

        case AUI_KWS_CONFIRM:
            LOGD(TAG, "wwv confirmed");
            local_audio_play(LOCAL_AUDIO_HELLO);
            aos_event_set(&event_wwv_result, EVENT_WWV_CONFIRMED, AOS_EVENT_OR);
            break;

        default: 
            ret = -1;
            break;
    }

    return ret;
}

int json_string_eq(cJSON *js, const char *str)
{
    if (cJSON_IsString(js)) {
        if (strcmp(js->valuestring, str) == 0) {
            return 1;
        }
    }
    return 0;
}

void mit_asr_handle(void *data, int len, void *priv)
{
    char *json_text = (char *)data;
    int ret;

    cJSON *js = cJSON_Parse(json_text);

    ret = aui_kws_proc_mit(js, json_text);
    
    if (ret < 0) {
        ret = aui_nlp_proc_mit(js, json_text);
    }

    if (ret < 0) {
        local_audio_play(LOCAL_AUDIO_SORRY2); /* 不懂 */
    }
    
    cJSON_Delete(js);
}

static int mit_tts_audio(void *data, int data_len)
{
    int total_len;

    if (!player_running) {
        if (aui_fifo) {
            nsfifo_set_eof(aui_fifo, 0, 1); //set weof
            aui_player_stop(MEDIA_SYSTEM);
        }

        if (!aui_fifo){
            aui_fifo = nsfifo_open(mit_tts_fifo, O_CREAT, 10 * 16 * 1024);
        }
        nsfifo_reset(aui_fifo);

        app_player_play(MEDIA_SYSTEM, mit_tts_fifo, 1);
        player_running = true;
    }

    int   reof;
    char *pos;
    total_len = data_len;

    while (total_len > 0) {
        reof = nsfifo_is_reof(aui_fifo); /** peer read reach to end */
        if (reof) {
            //LOGD(TAG, "named fifo read eof");
            break;
        }

        int len = nsfifo_get_wpos(aui_fifo, &pos, 500);
        if (len <= 0) {
            continue;
        }

        len = MIN(len, total_len);
        memcpy(pos, (char *)data + (data_len - total_len), len);
        nsfifo_set_wpos(aui_fifo, len);

        total_len -= len;
    }

    return 0;
}

static void mit_tts_handle(void *data, int data_len, void *priv)
{
    char *json_text = (char *)data;

    cJSON *js = cJSON_Parse(json_text);

//{\"aui_tts_state\":%d,\"data\":%d,\"len\":%d}", AUI_TTS_PLAYING

    cJSON *state = cJSON_GetObjectItem(js, "aui_tts_state");

    int tts_state = atoi(state->valuestring);

    // LOGD(TAG, "TTS state(%d)", tts_state);

    if (tts_state == AUI_TTS_PLAYING) {
        cJSON *data_string = cJSON_GetObjectItem(js, "data");
        cJSON *len_string  = cJSON_GetObjectItem(js, "len");

        char *data = (char *)((int)(atoi(data_string->valuestring)));
        int len = atoi(len_string->valuestring);

        // LOGD(TAG, "auido recv(%p)len(%d)", data, len);
        mit_tts_audio(data, len);
    } else {
        if (tts_state == AUI_TTS_FINISH || tts_state == AUI_TTS_ERROR) {
            nsfifo_set_eof(aui_fifo, 0, 1); //set weof
        }
        player_running = false;
    }
    
    cJSON_Delete(js);
}

static void get_hex_mac(char *hex_mac)
{
    static uint8_t s_mac[6] = {0};
    int ret, try = 0;

    if (s_mac[0] == 0 && s_mac[1] == 0 && s_mac[2] == 0 && 
        s_mac[3] == 0 && s_mac[4] == 0 && s_mac[5] == 0) {
        aos_dev_t *wifi_dev = device_open_id("wifi", 0);

        do {
            ret = hal_wifi_get_mac_addr(wifi_dev, s_mac);
            if (ret == 0) {
                break;
            }

            aos_msleep(100);
        } while (++try < 5);
    }

    if (try == 5) {
        LOGE(TAG, "Can't get mac address");
    }

    for (int i = 0; i < 6; i++) {
        sprintf(hex_mac + i * 2, "%02x", s_mac[i]);
    }
}

static void get_uuid(char *uuid)
{
    char product_key[32 + 1] = {0};
    char device_name[32 + 1] = {0};
    int pk_len = sizeof(product_key), dn_len = sizeof(device_name);

    int ret1 = aos_kv_get("hal_devinfo_pk", product_key, &pk_len);
    int ret2 = aos_kv_get("hal_devinfo_dn", device_name, &dn_len);
    if (ret1 == 0 && ret2 == 0) {
        sprintf(uuid, "%s&%s", product_key, device_name);
    } else {
        get_hex_mac(uuid);
    }

    LOGD(TAG, "device uuid %s", uuid);
}

/* ai engine init */
int app_aui_init(void)
{
    int ret = 0;
    cJSON *js_account_info = NULL;
    char *s_account_info = NULL;
    char device_uuid[100] = {0};

    get_uuid(device_uuid);

    aos_event_new(&event_tts_state, 0);
    // event_subscribe(EVENT_MEDIA_SYSTEM_ERROR, media_state_cb, NULL);
    // event_subscribe(EVENT_MEDIA_SYSTEM_FINISH, media_state_cb, NULL);

    js_account_info = cJSON_CreateObject();
    CHECK_RET_WITH_GOTO(js_account_info, END);

    int mitasr_key = 0;
    aos_kv_getint("mitasr", &mitasr_key);

    switch(mitasr_key) {
    case 1:
        /*ASR IoT feiyan*/
        //cJSON_AddStringToObject(js_account_info, "device_uuid", "a13RZKHNpQZ&deviceformit1");
        //cJSON_AddStringToObject(js_account_info, "asr_app_key", "oFKyuDs5ktE2Te6x");
        //cJSON_AddStringToObject(js_account_info, "asr_token", "4a37aa0c0373498ea04f732054841b62");
        //cJSON_AddStringToObject(js_account_info, "asr_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");
        cJSON_AddStringToObject(js_account_info, "device_uuid", device_uuid);
        cJSON_AddStringToObject(js_account_info, "asr_app_key", "DxcfGXG8NLCuH37h");
        cJSON_AddStringToObject(js_account_info, "asr_token", "4a37aa0c0373498ea04f732054841b62");
        cJSON_AddStringToObject(js_account_info, "asr_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");
        cJSON_AddStringToObject(js_account_info, "dialog_context", "{\"systemInfo\":\"{\\\"app_package\\\":\\\"com.cibn.tv\\\",\\\"package_name\\\":\\\"com.cibn.tv\\\",\\\"ykPid\\\":\\\"68935a5f396b549b\\\",\\\"uuid\\\":\\\"C55301A5037835E20B07AF9B10697AD9\\\",\\\"device_model\\\":\\\"Konka Android TV 551\\\",\\\"device_system_version\\\":\\\"5.1.1\\\",\\\"device_sn\\\":\\\"C55301A5037835E2\\\",\\\"device_firmware_version\\\":\\\"5.1.1\\\",\\\"firmware\\\":\\\"5.1.1\\\",\\\"charge_type\\\":\\\"2,3,5,7\\\",\\\"sw\\\":\\\"sw1080\\\",\\\"version_code\\\":2120601225,\\\"yingshi_version\\\":2120601225,\\\"com.cibn.tv\\\":2120601225,\\\"device_media\\\":\\\"\\\",\\\"mac\\\":\\\"90C35FB9D08C\\\",\\\"ethmac\\\":\\\"88795B2C38D6\\\",\\\"from\\\":\\\"0,7,9\\\",\\\"license\\\":\\\"7\\\",\\\"bcp\\\":\\\"7\\\",\\\"v_model\\\":\\\"F\\\",\\\"version_name\\\":\\\"6.1.2.25\\\"}\",\"platformKey\":\"\",\"sceneInfo\":\"{\\\"appPackage\\\":\\\"com.konka.athena\\\",\\\"awakenWord\\\":\\\"小康小康\\\",\\\"childVoiceOpen\\\":0,\\\"city\\\":\\\"深圳\\\",\\\"clientVersion\\\":1083,\\\"clientVersionName\\\":\\\"1.0.1083\\\",\\\"deviceMode\\\":0,\\\"media_source\\\":\\\"konka_tencent\\\",\\\"micType\\\":0,\\\"speakerInfoBO\\\":{},\\\"supportChild\\\":0,\\\"useApp\\\":\\\"com.konka.livelauncher\\\",\\\"useAppClientVersion\\\":90008,\\\"vipType\\\":0}\",\"packageInfo\":\"{\\\"com.konka.multimedia\\\":\\\"89649\\\",\\\"com.hisilicon.android.hiRMService\\\":\\\"1\\\",\\\"com.android.defcontainer\\\":\\\"22\\\",\\\"com.konka.message\\\":\\\"100079\\\",\\\"com.konka.konkabtctlbind_5\\\":\\\"82425\\\",\\\"com.konka.applist\\\":\\\"88654\\\",\\\"com.konka.smartengine\\\":\\\"90426\\\",\\\"com.iflytek.showcomesettings\\\":\\\"1159\\\",\\\"com.konka.kksmarthome\\\":\\\"175\\\",\\\"com.gitvkonka.video\\\":\\\"73513\\\",\\\"com.konka.bootlogicproxy\\\":\\\"2\\\",\\\"com.android.inputdevices\\\":\\\"22\\\",\\\"com.tencent.karaoketv\\\":\\\"32\\\",\\\"com.konka.systemsetting\\\":\\\"90086\\\",\\\"com.konka.setupwizard\\\":\\\"89490\\\",\\\"com.konka.mor.tv\\\":\\\"180818\\\",\\\"com.android.externalstorage\\\":\\\"22\\\",\\\"com.konka.vadr\\\":\\\"39815\\\",\\\"com.konka.SmartControl\\\":\\\"20190412\\\",\\\"com.konka.localserver\\\":\\\"89893\\\",\\\"com.konka.hotelmenu\\\":\\\"90227\\\",\\\"com.android.keychain\\\":\\\"22\\\",\\\"com.konka.downloadcenter\\\":\\\"102\\\",\\\"com.konka.cloudsearch\\\":\\\"88623\\\",\\\"com.konka.fourkshow\\\":\\\"90060\\\",\\\"com.android.managedprovisioning\\\":\\\"22\\\",\\\"com.iflytek.xiri.ime\\\":\\\"10493\\\",\\\"com.konka.kkfactory\\\":\\\"89882\\\",\\\"com.konka.systemadvert\\\":\\\"88999\\\",\\\"com.iflytek.itvs\\\":\\\"20110\\\",\\\"com.konka.livelauncher\\\":\\\"90008\\\",\\\"com.bestv.mishitong.tv\\\":\\\"23081306\\\",\\\"com.shafa.konka.appstore\\\":\\\"401\\\",\\\"com.ktcp.tvvideo\\\":\\\"3720\\\",\\\"com.konka.kksystemui\\\":\\\"90084\\\",\\\"com.xiaodianshi.tv.yst\\\":\\\"101602\\\",\\\"com.konka.tvsettings\\\":\\\"90155\\\",\\\"com.konka.passport\\\":\\\"80571\\\",\\\"android\\\":\\\"22\\\",\\\"com.yunos.tvtaobao\\\":\\\"2110500004\\\",\\\"com.konka.familycontrolcenter\\\":\\\"72300\\\",\\\"com.konka.quickstandby\\\":\\\"1\\\",\\\"com.android.webview\\\":\\\"399992\\\",\\\"com.dianshijia.newlive\\\":\\\"335\\\",\\\"com.android.providers.settings\\\":\\\"22\\\",\\\"com.android.systemui\\\":\\\"22\\\",\\\"com.yunos.tv.appstore\\\":\\\"2101403003\\\",\\\"hdpfans.com\\\":\\\"93\\\",\\\"com.konka.appupgrade\\\":\\\"1000199\\\",\\\"com.konka.activitycontainer\\\":\\\"788\\\",\\\"com.konka.a2dpsink\\\":\\\"10904\\\",\\\"com.konka.athena\\\":\\\"1083\\\",\\\"com.konka.tvmall\\\":\\\"38586\\\",\\\"com.konka.account\\\":\\\"83534\\\",\\\"com.konka.market.main\\\":\\\"90000\\\",\\\"com.bajintech.karaok\\\":\\\"1031\\\",\\\"com.kangjia.dangbeimarket\\\":\\\"132\\\",\\\"com.android.packageinstaller\\\":\\\"81283\\\",\\\"com.iflytek.showcome\\\":\\\"20433\\\",\\\"com.android.bluetooth\\\":\\\"22\\\",\\\"com.konka.videorecords\\\":\\\"89981\\\",\\\"com.android.shell\\\":\\\"22\\\",\\\"com.konka.kkmultiscreen\\\":\\\"90152\\\",\\\"com.konka.upgrade\\\":\\\"100206\\\",\\\"com.iflytek.xiri\\\":\\\"190000001\\\",\\\"com.konka.adverttool\\\":\\\"89495\\\",\\\"com.konka.tvmanager\\\":\\\"89701\\\",\\\"com.cibn.tv\\\":\\\"2120601225\\\",\\\"com.tencent.qqmusictv\\\":\\\"311\\\",\\\"com.konka.systeminfo\\\":\\\"190226\\\"}\"}");
        break;
    case 2:
        /*ASR kaishu*/
        cJSON_AddStringToObject(js_account_info, "device_uuid", device_uuid);
        cJSON_AddStringToObject(js_account_info, "asr_app_key", "y5QsLk2A3acWEhCs");
        cJSON_AddStringToObject(js_account_info, "asr_token", "4a37aa0c0373498ea04f732054841b62");
        cJSON_AddStringToObject(js_account_info, "asr_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");
        break;
    case 3:
        /*ASR IoT meeting*/
        cJSON_AddStringToObject(js_account_info, "device_uuid", "a1IB0paJvIz&H000029J00000034");
        cJSON_AddStringToObject(js_account_info, "asr_app_key", "d119971b");
        cJSON_AddStringToObject(js_account_info, "asr_token", "df28a632ca2e41d38db53ddf4957e573");
        cJSON_AddStringToObject(js_account_info, "asr_url", "wss://smarth.alibaba-inc.com/ws/v1");
        break;
    default:
        /*ASR kaishu test account*/
        cJSON_AddStringToObject(js_account_info, "device_uuid", device_uuid);
        cJSON_AddStringToObject(js_account_info, "asr_app_key", "g3aHMdL7v63bZCS3");
        cJSON_AddStringToObject(js_account_info, "asr_token", "4a37aa0c0373498ea04f732054841b62");
        cJSON_AddStringToObject(js_account_info, "asr_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");
        ;
    }

    /*TTS*/
    cJSON_AddStringToObject(js_account_info, "tts_app_key", "9a7f47f2");
    cJSON_AddStringToObject(js_account_info, "tts_token", "a2f8b80e04f14fdb9b7c36024fb03f78");
    cJSON_AddStringToObject(js_account_info, "tts_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");

    /*TTS*/
    cJSON_AddStringToObject(js_account_info, "tts_app_key", "9a7f47f2");
    cJSON_AddStringToObject(js_account_info, "tts_token", "a2f8b80e04f14fdb9b7c36024fb03f78");
    cJSON_AddStringToObject(js_account_info, "tts_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");
    
    s_account_info = cJSON_PrintUnformatted(js_account_info);
    CHECK_RET_TAG_WITH_GOTO(s_account_info, END);

    aui_config_t cfg;
    cfg.per             = "aixia";
    cfg.vol             = 100;      /* 音量 0~100 */
    cfg.spd             = 0;        /* -500 ~ 500*/
    cfg.pit             = 0;        /* 音调*/
    cfg.asr_fmt         = 2;        /* 编码格式，1：PCM 2：MP3 */
    cfg.tts_fmt         = 2;        /* 编码格式，1：PCM 2：MP3 */
    cfg.srate           = 16000;    /* 采样率，16000 */
    cfg.tts_cache_path  = NULL;     /* TTS内部缓存路径，NULL：关闭缓存功能 */
    cfg.cloud_vad       = 1;        /* 云端VAD功能使能， 0：关闭；1：打开 */
    cfg.js_account      = s_account_info;

    g_aui_handler = aui_cloud_init(&cfg);
    mit_tts_fifo = cfg.tts_fmt == 1 ? "fifo://mittts?avformat=rawaudio&avcodec=pcm_s16le&channel=1&rate=16000" : "fifo://mittts";

    aui_asr_register_mit(g_aui_handler, mit_asr_handle, NULL);
    aui_tts_register_mit(g_aui_handler, mit_tts_handle, NULL);
    aos_event_new(&event_wwv_result, 0);

    if (ret != 0) {
        LOGE(TAG, "ai engine error");
        goto END;
    }

    // aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_proc_mit);

END:
    cJSON_Delete(js_account_info);
    free(s_account_info);
    
    return ret;
}

int app_aui_get_wwv_result(unsigned int timeout)
{
    unsigned int flags = 0;

    aos_event_get(&event_wwv_result, EVENT_WWV_CONFIRMED | EVENT_WWV_REJECTED, AOS_EVENT_OR_CLEAR, &flags, timeout);
    if (flags & EVENT_WWV_CONFIRMED) {
        return 0;
    }

    return -1;
}

int app_aui_cloud_push_audio(void *data, size_t size)
{
    return aui_cloud_push_audio(g_aui_handler, data, size);
}

int app_aui_cloud_stop(int force_stop)
{
    if (force_stop) {
        return aui_cloud_stop(g_aui_handler);
    }

    return aui_cloud_stop_audio(g_aui_handler);
}

int app_aui_cloud_start(int do_wwv)
{
    aos_event_set(&event_wwv_result, 0, AOS_EVENT_AND);
    return aui_cloud_start_audio(g_aui_handler, 0);
}

int app_aui_cloud_start_tts()
{
    nsfifo_set_eof(aui_fifo, 0, 1); //set weof
    player_running = false;
    return aui_cloud_start_tts(g_aui_handler);
}

int app_aui_push_wwv_data(void *data, size_t len)
{
    aui_cloud_start_audio(g_aui_handler, 1);
    aui_cloud_push_audio(g_aui_handler, data, len);
    return 0;
}

int app_aui_cloud_stop_tts()
{
    // nsfifo_set_eof(aui_fifo, 0, 1); //set weof
    player_running = false;
    return aui_cloud_stop_tts(g_aui_handler);
}

int app_aui_cloud_tts_run(const char *text, int wait_last)
{
	aos_event_set(&event_tts_state, 0, AOS_EVENT_AND);
    return aui_cloud_req_tts(g_aui_handler, text);
}

int app_aui_cloud_push_text(char *text)
{
    return aui_cloud_push_text(g_aui_handler, text);
}
