/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/debug.h>
#include <yoc/aui_cloud.h>

#define TAG "TextCmd"

typedef struct _text_cmd_list {
    const char *cmd;
    const char *text;
    slist_t     next;
} text_cmd_list_t;

static slist_t     g_text_cmds    = {NULL};
static const char *g_def_delim    = ",";
static const char *g_pat_delim    = NULL;
static int         g_max_text_len = 16;

void aui_textcmd_matchinit(int max_text_len, const char *delim)
{
    g_max_text_len = max_text_len;
    if (delim == NULL) {
        g_pat_delim = g_def_delim;
    } else {
        g_pat_delim = delim;
    }
}

void aui_textcmd_matchadd(const char *cmd, const char *text)
{
    aos_check_return(cmd);
    aos_check_return(text);

    text_cmd_list_t *node = aos_zalloc(sizeof(text_cmd_list_t));

    if (node) {
        node->cmd  = cmd;
        node->text = text;
        slist_add(&node->next, &g_text_cmds);
    }
}

static int re_match2(const char *pattern, const char *text, const char *delim)
{
    int match = 0;

    /* copy pattern, strtok need */
    char *pat = strdup(pattern);

    char *saveptr = NULL;
    char *next    = strtok_r(pat, delim, &saveptr);

    while (next) {
        /* search in cmd text */
        char *fstr = strstr(text, next);
        if (fstr == NULL) {
            match = -1;
            break;
        }

        /* get next */
        next = strtok_r(NULL, delim, &saveptr);
    }

    free(pat);

    return match;
}

const char *aui_textcmd_find(const char *text)
{
    text_cmd_list_t *node;
    char *           short_text = malloc(g_max_text_len + 1);

    if (text != NULL) {
        strlcpy(short_text, text, g_max_text_len + 1);

        slist_for_each_entry(&g_text_cmds, node, text_cmd_list_t, next)
        {
            //LOGD(TAG, "%s,%s",node->text, short_text);
            if (re_match2(node->text, short_text, g_pat_delim) >= 0) {
                free(short_text);
                return node->cmd;
            }
        }
    }

    free(short_text);
    return NULL;
}

int aui_textcmd_matchnlp(aui_t *aui, const char *text)
{
    char js[64];

    aos_check_return_einval(aui);
    aos_check_return_einval(text);

    const char *cmd = aui_textcmd_find(text);

    if (cmd == NULL) {
        return -1;
    }

    snprintf(js, sizeof(js), "{\"textcmd\":\"%s\"}", cmd);
    aui->config.nlp_cb(js);

    return 0;
}