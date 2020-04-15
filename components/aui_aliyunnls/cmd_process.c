/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <aos/debug.h>
#include <aos/kernel.h>
#include <yoc/aui_cloud.h>

void aui_nlp_process_add(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd)
{
    aos_check_return(set && cmd);

    aui_cmd_t *node = aos_zalloc(sizeof(aui_cmd_t));

    if (node) {
        node->cmd = cmd;
        slist_add(&node->next, &set->cmds);
    }
}

void aui_nlp_process_set_pre_check(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd)
{
    aos_check_return(set && cmd);

    set->pre_check_cmd = cmd;
}

void aui_nlp_process_remove(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd)
{
    aos_check_return(set && cmd);

    aui_cmd_t *node;
    slist_t * tmp;

    slist_for_each_entry_safe(&set->cmds, tmp, node, aui_cmd_t, next)
    {
        if (node->cmd == cmd) {
            slist_del(&node->next, &set->cmds);
            aos_free(node);
        }
    }
}

int aui_nlp_process_run(aui_cmd_set_t *set, const char *json_text)
{
    int ret = AUI_CMD_PROC_NOMATCH;

    if (set == NULL || json_text == NULL) {
        return AUI_CMD_PROC_ERROR;
    }

    cJSON *js = cJSON_Parse(json_text);
    if (js == NULL) {
        return AUI_CMD_PROC_ERROR;
    }

    if (set->pre_check_cmd) {
        int pre_check = set->pre_check_cmd(js, json_text);
        if (pre_check == -ENETUNREACH) {
            cJSON_Delete(js);
            return AUI_CMD_PROC_NET_ABNORMAL;
        } else if (pre_check < 0) {
            cJSON_Delete(js);
            return AUI_CMD_PROC_NOMATCH;
        }
    }

    aui_cmd_t *node;

    slist_for_each_entry(&set->cmds, node, aui_cmd_t, next)
    {
        int cmd_ret = node->cmd(js, json_text);

        if (cmd_ret == 0) {
            /* 解析正确 */
            ret = 0;
            break;
        } else if (cmd_ret == -2) {
            /* 符合格式要求，但内容有错误，跳出不进去其他格式的解析 */
            ret = AUI_CMD_PROC_MATCH_NOACTION;
            break;
        }
    }

    cJSON_Delete(js);
    return ret;
}
