/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <string.h>
#include <smart_audio.h>

#include "ex_cjson.h"

#include "aui_nlp.h"

#define TAG "nlptxt"

#include "event_mgr/app_event.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

/**
 * 文本命令解析的处理
*/
int aui_nlp_proc_textcmd(const char *asr_text)
{
    //LOGD(TAG, "Enter %s", __FUNCTION__);

    if (asr_text == NULL) {
        LOGD(TAG, "textcmd not found");
        return -1;
    }

    const char *cmd = asr_text;
    size_t len = strlen(cmd);
    LOGD(TAG, "process_textcmd:%s,%d\n", cmd, len);

    /**
     * 设备端可通过分析asr文本，扩展解析意图
     * 若执行了具体意图通过 return 0返回
     * 示例云端返回内容为UTF8，请确保当前文件为UTF8格式，中文字符串才能匹配上
    */
    if (strstr(cmd, "打开窗帘") != NULL || strstr(cmd, "开窗帘") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "打开窗帘");
        return 0;
    } else if (strstr(cmd, "关闭窗帘") != NULL || strstr(cmd, "关窗帘") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "关闭窗帘");
        return 0;
    }else if (strstr(cmd, "开卧室") != NULL || strstr(cmd, "打开卧室") != NULL || strstr(cmd, "打开卧室灯") != NULL || strstr(cmd, "开卧室灯") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "打开卧室灯");
        return 0;
    } else if (strstr(cmd, "关卧室") != NULL || strstr(cmd, "关闭卧室") != NULL || strstr(cmd, "关闭卧室灯") != NULL || strstr(cmd, "关卧室灯") != NULL ){
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "关闭卧室灯");
        return 0;
    }
    else if (strstr(cmd, "打开厨房") != NULL || strstr(cmd, "打开厨房灯") != NULL || strstr(cmd, "开厨房灯") != NULL ) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "打开厨房灯");
        return 0;
    } else if (strstr(cmd, "关闭厨房") != NULL || strstr(cmd, "关闭厨房灯") != NULL || strstr(cmd, "关厨房灯") != NULL ){
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "关闭厨房灯");
        return 0;
    }else if (strstr(cmd, "打开客厅灯") != NULL || strstr(cmd, "开客厅灯") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "打开客厅灯");
        return 0;
    } else if (strstr(cmd, "关闭客厅灯") != NULL || strstr(cmd, "关客厅灯") != NULL){ 
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "关闭客厅灯");
        return 0;
    }else if (strstr(cmd, "暂停") != NULL || strstr(cmd, "暂停音乐") != NULL || strstr(cmd, "暂停播放") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "暂停");
        smtaudio_enable_ready_list(0);
        return 0;
    }else if (strstr(cmd, "停止") != NULL || strstr(cmd, "停止音乐") != NULL || strstr(cmd, "停止播放") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "停止");
        smtaudio_enable_ready_list(0);
        return 0;
    }else if (strstr(cmd, "回家模式") != NULL ) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "回家模式");
        return 0;
    }else if (strstr(cmd, "离家模式") != NULL ) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "离家模式");
        return 0;
    }else if (strstr(cmd, "会议模式") != NULL ) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "会议模式");
        return 0;
    }else if (strstr(cmd, "电影模式") != NULL ) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "电影模式");
        return 0;
    }else if (strstr(cmd, "打开灯") != NULL || strstr(cmd, "开灯") != NULL || strstr(cmd, "开所有灯") != NULL || strstr(cmd, "打开所有灯") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "开所有灯");
        return 0;
    }else if (strstr(cmd, "关灯") != NULL || strstr(cmd, "关闭灯") != NULL || strstr(cmd, "关所有灯") != NULL || strstr(cmd, "关闭所有灯") != NULL) {
        event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "关所有灯");
        return 0;
    }

    return -1;
}
