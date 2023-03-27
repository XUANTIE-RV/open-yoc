/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <string.h>
#include <smart_audio.h>

#include "ex_cjson.h"

#include "aui_nlp.h"

#define TAG "nlptxt"

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
    LOGD(TAG, "process_textcmd:%s", cmd);

    /**
     * 设备端可通过分析asr文本，扩展解析意图
     * 若执行了具体意图通过 return 0返回
     * 示例云端返回内容为UTF8，请确保当前文件为UTF8格式，中文字符串才能匹配上
    */
    if (len < 16 && strstr(cmd, "打开蓝牙") != NULL) {
        LOGD(TAG, "textcmd:open bt");
        return 0;
    } else if (len < 16 && strstr(cmd, "关闭蓝牙") != NULL) {
        LOGD(TAG, "textcmd:close bt");
        return 0;
    }

    return -1;
}
