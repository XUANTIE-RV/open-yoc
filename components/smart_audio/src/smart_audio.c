/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <smart_audio.h>
#include <av/player.h>
#include <csi_core.h>

#define TAG "smart_audio"

#define SMTAUDIO_MESSAGE_NUM 10

static uint32_t smtaudio_crtl_id;
static dlist_t smtaudio_ctrl_list_head;
static dlist_t smtaudio_delay_list_head;
static dlist_t smtaudio_resume_list_head;
static audio_evt_t audio_event_callback;

typedef struct smtaudio_type {
    smtaudio_sub_state_t cur_state;
    smtaudio_sub_state_t last_state;
    uint8_t              lpm_flag; // 0: not in lpm state, 1: in lpm state
    aos_mutex_t          smtaudio_mutex;
    aos_queue_t          smtaudio_queue;
    uint8_t *            smtaudio_q_buffer;
    uint8_t              resume_flag;
    uint8_t              run_flag;
    smtaudio_action_t    cur_action; // 记录action来管理因为反馈的异步导致反复可以执行action的问题
    long long            action_time;
} smtaudio_ctx_t;

static smtaudio_ctx_t smtaudio_ctx;
static bool pauseBeforeMute;

#define SMTAUDIO_LOCK()                                                 \
    do {                                                                \
        aos_mutex_lock(&smtaudio_ctx.smtaudio_mutex, AOS_WAIT_FOREVER); \
    } while (0)
#define SMTAUDIO_UNLOCK()                               \
    do {                                                \
        aos_mutex_unlock(&smtaudio_ctx.smtaudio_mutex); \
    } while (0)

static aos_mutex_t smtaudio_list_mutex;
#define SMTAUDIO_LIST_LOCK()                                     \
    do {                                                     \
        if (!aos_mutex_is_valid(&smtaudio_list_mutex))              \
            aos_mutex_new(&smtaudio_list_mutex);                    \
        aos_mutex_lock(&smtaudio_list_mutex, AOS_WAIT_FOREVER);     \
    } while (0)
#define SMTAUDIO_LIST_UNLOCK()                                    \
    do {                                                      \
        aos_mutex_unlock(&smtaudio_list_mutex);                      \
    } while (0)

#define LPM_RETURN()             \
    if (smtaudio_ctx.lpm_flag) { \
        return;                  \
    }
#define LPM_RETURN_RET(ret)      \
    if (smtaudio_ctx.lpm_flag) { \
        return ret;              \
    }

static void smtaudio_delay_list_insert(smtaudio_delay_list_node_t *smt_node);
static void smtaudio_delay_list_rm(smtaudio_delay_list_node_t *smt_node);
static void smtaudio_delay_list_clear(void);
static void smtaudio_resume_list_insert(smtaudio_resume_list_node_t *smt_node);
static void smtaudio_resume_list_rm(smtaudio_resume_list_node_t *smt_node);
static void smtaudio_resume_list_clear(void);
/***** internal APIs *****/
static void smtaudio_delay_list_rm(smtaudio_delay_list_node_t *smt_node)
{
    LOGD(TAG, "delete type:%d from delay list", smt_node->id);
    SMTAUDIO_LIST_LOCK();
    dlist_del(&smt_node->node);
    SMTAUDIO_LIST_UNLOCK();
    aos_free(smt_node->url);
    free(smt_node);
}

static void smtaudio_delay_list_clear(void)
{
    while (!dlist_empty(&smtaudio_delay_list_head)) {
        smtaudio_delay_list_node_t *first_node;
        first_node =
            dlist_entry(smtaudio_delay_list_head.next, smtaudio_delay_list_node_t, node);
        smtaudio_delay_list_rm(first_node);
    }
}

/* resume/delay list插入时 清空对方 list */
static void smtaudio_delay_list_insert(smtaudio_delay_list_node_t *smt_node)
{
    smtaudio_delay_list_node_t *tmp_node = NULL;

    smtaudio_resume_list_clear();

    dlist_for_each_entry(&smtaudio_delay_list_head, tmp_node, smtaudio_delay_list_node_t, node) {
        if ((tmp_node->delay_reason == smt_node->delay_reason) || (tmp_node->id == smt_node->id)) {
            smtaudio_delay_list_rm(tmp_node);
            break;
        }
    }
    LOGD(TAG, "add type:%d into delay list, reason:%d", smt_node->id, smt_node->delay_reason);
    /* sort by prio, small to big*/
    dlist_for_each_entry(&smtaudio_delay_list_head, tmp_node, smtaudio_delay_list_node_t, node) {
        if (tmp_node->prio > smt_node->prio) {
            break;
        }
    }
    SMTAUDIO_LIST_LOCK();
    dlist_add_tail(&smt_node->node, &tmp_node->node);
    SMTAUDIO_LIST_UNLOCK();
}

static void smtaudio_resume_list_rm(smtaudio_resume_list_node_t *smt_node)
{
    LOGD(TAG, "delete type:%d from resume list", smt_node->id);
    SMTAUDIO_LIST_LOCK();
    dlist_del(&smt_node->node);
    SMTAUDIO_LIST_UNLOCK();
    free(smt_node);
}

static void smtaudio_resume_list_clear(void)
{
    while (!dlist_empty(&smtaudio_resume_list_head)) {
        smtaudio_resume_list_node_t *first_node;
        first_node =
            dlist_entry(smtaudio_resume_list_head.next, smtaudio_resume_list_node_t, node);
        smtaudio_resume_list_rm(first_node);
    }
}

static void smtaudio_resume_list_insert(smtaudio_resume_list_node_t *smt_node)
{
    /* 清空 delay list */
    smtaudio_delay_list_clear();

    smtaudio_resume_list_clear();
    LOGD(TAG, "add type:%d into resume list, reason:%d", smt_node->id, smt_node->interrupt_reason);
    SMTAUDIO_LIST_LOCK();
    dlist_add_tail(&smt_node->node, &smtaudio_resume_list_head);
    SMTAUDIO_LIST_UNLOCK();
}

static void smtaudio_ctrl_list_pri_insert(smtaudio_ops_node_t *smt_node)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    /* sort by prio, small to big*/
    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->prio > smt_node->prio) {
            break;
        }
    }
    SMTAUDIO_LIST_LOCK();   
    dlist_add_tail(&smt_node->node, &tmp_node->node);
    SMTAUDIO_LIST_UNLOCK();
}

static smtaudio_ops_node_t *get_smtaudio_ctrl_ops_by_id(int id)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->id == id) {
            return tmp_node;
        }
    }
    return NULL;
}

static smtaudio_ops_node_t *find_first_playing_audio(void)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            return tmp_node;
        }
    }
    return NULL;
}

static smtaudio_ops_node_t *find_first_pause_audio(void)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PAUSE) {
            return tmp_node;
        }
    }
    return NULL;
}

smtaudio_ops_node_t *get_default_audio_ops(void)
{
    return get_smtaudio_ctrl_ops_by_id(DEFAULT_PLAY_TYPE);
}

/* 判断当前 是否允许 播放 resume/delay list 中的 音源 */
static int smtaudio_resume_state(int reason, int type)
{
    if (reason == SMTAUDIO_LOCAL_PLAY) {
        return smtaudio_ctx.resume_flag;
    } else {
        return 1;
    }
}

static void switch_state(smtaudio_sub_state_t old_state, smtaudio_sub_state_t new_state)
{
    if (old_state == new_state) {
        /* don't need to change state */
        return;
    }

    LOGD(TAG, "old state[%d] --> new state[%d]", old_state, new_state);
    smtaudio_ctx.last_state = old_state;
    smtaudio_ctx.cur_state  = new_state;
}

static void smtaudio_change_state(smtaudio_state_t new_state, smtaudio_player_type_t type)
{
    smtaudio_ops_node_t *tmp_node = NULL;
    smtaudio_ops_node_t *smtaudio_node = NULL;
    smtaudio_ops_node_t *first_playing_audio_ops = NULL;
    smtaudio_sub_state_t new_sub_state;

    //通过播放类型和节点的播放状态，转化为smtaudio_sub_state_t的全局状态
    switch(type) {
        case SMTAUDIO_ONLINE_MUSIC:
            new_sub_state = SMTAUDIO_SUBSTATE_ONLINE_PLAYING;
            break;
        case SMTAUDIO_LOCAL_PLAY:
            new_sub_state = SMTAUDIO_SUBSTATE_LOCAL_PLAYING;
            break;
        case SMTAUDIO_BT_A2DP:
            new_sub_state = SMTAUDIO_SUBSTATE_BT_A2DP_PLAYING;
            break;
        case SMTAUDIO_BT_HFP:
            new_sub_state = SMTAUDIO_SUBSTATE_BT_HFP_PLAYING;
            break;
        default:
            LOGE(TAG, "play type error", __FUNCTION__);
            return;
    }
    new_sub_state += new_state;

    LOGD(TAG, "Enter %s: current state:%d type:%d new_state:%d new_sub_state=%d", __FUNCTION__,
         smtaudio_ctx.cur_state, type, new_state, new_sub_state);

    if (smtaudio_ctx.cur_state == new_sub_state) {
        LOGD(TAG, "seem state, ignore check", __FUNCTION__);
        return;
    }

    smtaudio_node = get_smtaudio_ctrl_ops_by_id(type);
    if (smtaudio_node) {
        if (new_state == SMTAUDIO_STATE_PLAYING) {
            if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
                smtaudio_node->pause();
                return;
            }

            dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
                if ((tmp_node->status == SMTAUDIO_STATE_PLAYING) && (tmp_node->id != type)) {
                    first_playing_audio_ops = tmp_node;
                    break;
                }
            }

            if (first_playing_audio_ops) {
                LOGD(TAG, "first playig audio type:%d", first_playing_audio_ops->id);
                if (first_playing_audio_ops->prio < smtaudio_node->prio) {
                    smtaudio_node->pause();
                    smtaudio_delay_list_node_t *node =
                        (smtaudio_delay_list_node_t *)malloc(sizeof(smtaudio_delay_list_node_t));
                    node->id           = smtaudio_node->id;
                    node->prio         = smtaudio_node->prio;
                    node->seek_time    = 0;
                    node->url          = NULL;
                    node->delay_reason = first_playing_audio_ops->id;
                    node->valid = 1;
                    smtaudio_delay_list_insert(node);
                    return;
                } else if (first_playing_audio_ops->prio > smtaudio_node->prio) {
                    first_playing_audio_ops->pause();
                    smtaudio_resume_list_node_t *node =
                        (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
                    node->id               = first_playing_audio_ops->id;
                    node->prio             = first_playing_audio_ops->prio;
                    node->interrupt_reason = type;
                    node->valid = 1;
                    smtaudio_resume_list_insert(node);
                } else {
                    //同一优先级打断 不resume
                    first_playing_audio_ops->pause();
                }
            }
            switch_state(smtaudio_ctx.cur_state, new_sub_state);
        } else {
            /* 播放源切换后, pause stop 无效*/
            if (smtaudio_get_play_type() == type) {
                switch_state(smtaudio_ctx.cur_state, new_sub_state);
            }
        }
    }
}

static void actmgr_set_action(smtaudio_action_t action)
{
    smtaudio_ctx.cur_action = action;
    smtaudio_ctx.action_time =  aos_now_ms();
    return;
}

static void actmgr_action_check(smtaudio_state_t state)
{
    int do_clear = 0;
    if (smtaudio_ctx.cur_action == SMTAUDIO_ACTION_PAUSE && 
            (state == SMTAUDIO_STATE_PAUSE || state == SMTAUDIO_STATE_STOP) ) {
        do_clear = 1;
    }

    long long ms = aos_now_ms();
    if ((smtaudio_ctx.cur_action != SMTAUDIO_ACTION_NULL) &&
                            (ms - smtaudio_ctx.action_time > 5000) ) {
        LOGD(TAG, "action check timeout, clear");
        do_clear = 1;
    }

    if (do_clear) {
        smtaudio_ctx.cur_action  = SMTAUDIO_ACTION_NULL;
        smtaudio_ctx.action_time =  0;
    }
    return;
}

/* 所有播放源产生事件(start、pause、stop)时, 需要调用的回调函数 */
static void smtaudio_event_callback(int type, smtaudio_player_evtid_t evt_id)
{
    int            ret;
    audio_result_t result;

    result.type   = type;
    result.evt_id = evt_id;

    ret = aos_queue_send(&smtaudio_ctx.smtaudio_queue, &result, sizeof(audio_result_t));
    if (ret < 0) {
        LOGE(TAG, "queue send failed");
    }
}

static void smtaudio_event_task(void *arg)
{
    size_t         len;
    audio_result_t       audio_result;
    smtaudio_ops_node_t *smtaudio_node;
    smtaudio_resume_list_node_t *first_resume_node;
    smtaudio_delay_list_node_t *first_delay_node;
    aui_play_time_t tempTime;

    while (smtaudio_ctx.run_flag) {
        aos_queue_recv(&smtaudio_ctx.smtaudio_queue, AOS_WAIT_FOREVER, &audio_result, &len);
        LOGD(TAG, "smtaudio_event_callback type:%d evt_id:%d\n", audio_result.type,
             audio_result.evt_id);
        smtaudio_node = get_smtaudio_ctrl_ops_by_id(audio_result.type);
        switch (audio_result.evt_id) {
        case SMTAUDIO_PLAYER_EVENT_START:
            if(audio_result.type == MEDIA_MUSIC) {
                aui_player_get_time(MEDIA_MUSIC, &tempTime);
                smtaudio_node->duration=tempTime.duration;
            }
        case SMTAUDIO_PLAYER_EVENT_RESUME:
            smtaudio_node->status = SMTAUDIO_STATE_PLAYING;
            smtaudio_change_state(SMTAUDIO_STATE_PLAYING, audio_result.type);
            break;
        case SMTAUDIO_PLAYER_EVENT_ERROR:
        case SMTAUDIO_PLAYER_EVENT_STOP:
            smtaudio_node->status = SMTAUDIO_STATE_STOP;
            smtaudio_change_state(SMTAUDIO_STATE_STOP, audio_result.type);
            break;
        case SMTAUDIO_PLAYER_EVENT_PAUSE:
            smtaudio_node->status = SMTAUDIO_STATE_PAUSE;
            smtaudio_change_state(SMTAUDIO_STATE_PAUSE, audio_result.type);
            break;
        
        case SMTAUDIO_PLAYER_EVENT_PAUSE_BY_REMOTE:
            smtaudio_node->status = SMTAUDIO_STATE_PAUSE;
            smtaudio_change_state(SMTAUDIO_STATE_PAUSE, audio_result.type);

            smtaudio_resume_list_node_t *node =
                        (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
            node->id               = smtaudio_node->id;
            node->prio             = smtaudio_node->prio;
            node->interrupt_reason = SMTAUDIO_INTERRUPT_REASON_BY_REMOTE;
            node->valid = 1;

            smtaudio_resume_list_insert(node);
            break;
        default:
            break;
        }

        if(audio_event_callback) {
            audio_event_callback(audio_result.type, audio_result.evt_id);
        }

        //清理action标记
        if (smtaudio_node) {
            actmgr_action_check(smtaudio_node->status);
        }

        if ((audio_result.evt_id == SMTAUDIO_PLAYER_EVENT_STOP) ||
            (audio_result.evt_id == SMTAUDIO_PLAYER_EVENT_PAUSE)) {
            if (!dlist_empty(&smtaudio_resume_list_head)) {
                dlist_for_each_entry(&smtaudio_resume_list_head, first_resume_node, smtaudio_resume_list_node_t, node) {
                    if((first_resume_node->interrupt_reason == audio_result.type) && (first_resume_node->valid)) {
                        if (smtaudio_resume_state(first_resume_node->interrupt_reason,
                                                  audio_result.type)) {
                            smtaudio_node = get_smtaudio_ctrl_ops_by_id(first_resume_node->id);
                            if (smtaudio_node) {
                                LOGD(TAG, "resume type:%d from resume list", smtaudio_node->id);
                                smtaudio_node->resume();
                            }
                            smtaudio_resume_list_rm(first_resume_node);
                            break;
                        } else {
                            LOGD(TAG, "not in play state");
                        }
                    }
                }
            } else if (!dlist_empty(&smtaudio_delay_list_head)) {
                dlist_for_each_entry(&smtaudio_delay_list_head, first_delay_node, smtaudio_delay_list_node_t, node) {
                    if ((first_delay_node->delay_reason == audio_result.type) && (first_delay_node->valid)){
                        if (smtaudio_resume_state(first_delay_node->delay_reason, audio_result.type)) {
                            smtaudio_node = get_smtaudio_ctrl_ops_by_id(first_delay_node->id);
                            if (smtaudio_node) {
                                LOGD(TAG, "start type:%d from delay list", smtaudio_node->id);
                                smtaudio_node->start(first_delay_node->url, first_delay_node->seek_time, 0);
                                if (smtaudio_node->url) {
                                    free(smtaudio_node->url);
                                    smtaudio_node->url = NULL;
                                }

                                if (first_delay_node->url) { 
                                    smtaudio_node->url = strdup(first_delay_node->url);
                                }
                            }
                            smtaudio_delay_list_rm(first_delay_node);
                            break;
                        } else {
                            LOGD(TAG, "not in play state");
                        }
                    }
                }
            } 
        }
    }
    aos_task_exit(0);
}



/****************************************/
/*          external APIs               */
/****************************************/
int8_t smtaudio_init(audio_evt_t audio_evt_cb)
{
    static int init_flag = 0;
    if(init_flag) {
        LOGE(TAG, "smtaudio already init");
        return -1;
    }
    LOGD(TAG, "Enter %s", __FUNCTION__);
    aos_task_t smt_task_hdl;

    memset(&smtaudio_ctx, 0, sizeof(smtaudio_ctx_t));
    aos_mutex_new(&smtaudio_ctx.smtaudio_mutex);
    smtaudio_ctx.smtaudio_q_buffer = (uint8_t *)malloc(sizeof(audio_result_t) * SMTAUDIO_MESSAGE_NUM);
    if (smtaudio_ctx.smtaudio_q_buffer == NULL) {
        LOGE(TAG, "smtaudio_ctx.smtaudio_q_buffer malloc fail");
        return -EINVAL;
    }
    aos_queue_new(&smtaudio_ctx.smtaudio_queue, smtaudio_ctx.smtaudio_q_buffer,
                  SMTAUDIO_MESSAGE_NUM * sizeof(audio_result_t), sizeof(audio_result_t));

    SMTAUDIO_LOCK();
    smtaudio_ctx.cur_state  = SMTAUDIO_SUBSTATE_LOCAL_STOP;
    smtaudio_ctx.last_state = SMTAUDIO_SUBSTATE_LOCAL_STOP;
    smtaudio_ctx.run_flag   = 1;
    smtaudio_ctx.cur_action = SMTAUDIO_ACTION_NULL;

    aos_task_new_ext(&smt_task_hdl, "smtaudio_event_task", smtaudio_event_task, NULL, CONFIG_SMART_AUDIO_STACK_SIZE,
                     AOS_DEFAULT_APP_PRI);
    SMTAUDIO_LIST_LOCK();

    dlist_init(&smtaudio_ctrl_list_head);
    dlist_init(&smtaudio_delay_list_head);
    dlist_init(&smtaudio_resume_list_head);
    if(audio_evt_cb) {
        audio_event_callback = audio_evt_cb;
    }
    SMTAUDIO_LIST_UNLOCK();

    SMTAUDIO_UNLOCK();

    player_init();

    init_flag = 1;
    return 0;
}

int8_t smtaudio_vol_up(int16_t vol)
{
    smtaudio_ops_node_t *node_adjust_ops;
    smtaudio_ops_node_t *audio_default_ops = get_default_audio_ops();

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    audio_default_ops->vol_up(vol); //确保默认音量被先处理
    dlist_for_each_entry(&smtaudio_ctrl_list_head, node_adjust_ops, smtaudio_ops_node_t, node) {
        if(node_adjust_ops->vol_up && node_adjust_ops != audio_default_ops) {
            node_adjust_ops->vol_up(vol);
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_vol_down(int16_t vol)
{
    smtaudio_ops_node_t *node_adjust_ops;
    smtaudio_ops_node_t *audio_default_ops = get_default_audio_ops();

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    audio_default_ops->vol_down(vol);//确保默认音量被先处理
    dlist_for_each_entry(&smtaudio_ctrl_list_head, node_adjust_ops, smtaudio_ops_node_t, node) {
        if(node_adjust_ops->vol_down && node_adjust_ops != audio_default_ops) {
            node_adjust_ops->vol_down(vol);
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_vol_set(int16_t set_vol)
{
    smtaudio_ops_node_t *node_adjust_ops;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    dlist_for_each_entry(&smtaudio_ctrl_list_head, node_adjust_ops, smtaudio_ops_node_t, node) {
        if(node_adjust_ops->vol_set) {
            node_adjust_ops->vol_set(set_vol);
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_vol_get(void)
{
    int vol = 0;
    smtaudio_ops_node_t *audio_default_ops;
    smtaudio_ops_node_t *first_playing_audio_ops;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    SMTAUDIO_LOCK();
    first_playing_audio_ops = find_first_playing_audio();
    if (first_playing_audio_ops && first_playing_audio_ops->vol_get) {
        vol = first_playing_audio_ops->vol_get();
        SMTAUDIO_UNLOCK();
        return vol;
    }

    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        vol = audio_default_ops->vol_get();
    }
    SMTAUDIO_UNLOCK();
    return vol;
}

int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume)
{
    int8_t               ret = 0;
    smtaudio_ops_node_t *smtaudio_node;
    smtaudio_ops_node_t *first_playing_audio_ops;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.(%s)", url);
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state:%d type:%d resume_flag:%d", __FUNCTION__, smtaudio_ctx.cur_state, type, resume);

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_ONLINE_PLAYING) {
        if (type != SMTAUDIO_LOCAL_PLAY) {
            /* 解决两次重复播放音乐导致的状态异常问题 */
            smtaudio_stop(SMTAUDIO_TYPE_ALL);
        }
    }

    if ((type < 0) || (type >= SMTAUDIO_PLAY_TYPE_NUM)) {
        LOGD(TAG, "invalid play type");
        return -1;
    }

    SMTAUDIO_LOCK();
    smtaudio_node = get_smtaudio_ctrl_ops_by_id(type);
    if (smtaudio_node) {
        first_playing_audio_ops = find_first_playing_audio();
        if (first_playing_audio_ops) {
            if (first_playing_audio_ops->prio < smtaudio_node->prio) {
                LOGD(TAG, "smaller prio, insert delay list");
                smtaudio_delay_list_node_t *node =
                    (smtaudio_delay_list_node_t *)malloc(sizeof(smtaudio_delay_list_node_t));
                node->id        = smtaudio_node->id;
                node->prio      = smtaudio_node->prio;
                node->seek_time = seek_time;
                node->url       = strdup(url);
                if (!node->url) {
                    SMTAUDIO_UNLOCK();
                    return 0;
                }
                node->delay_reason     = first_playing_audio_ops->id;
                node->valid = 1;
                smtaudio_delay_list_insert(node);
                SMTAUDIO_UNLOCK();
                return 0;
            } else if (first_playing_audio_ops->prio > smtaudio_node->prio) {
                LOGD(TAG, "larger prio, put %d insert resume list", first_playing_audio_ops->id);
                first_playing_audio_ops->pause();
                smtaudio_resume_list_node_t *node =
                    (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
                node->id               = first_playing_audio_ops->id;
                node->prio             = first_playing_audio_ops->prio;
                node->interrupt_reason = type;
                node->valid = 1;
                smtaudio_resume_list_insert(node);
            } else {
                LOGD(TAG, "equal prio, pause pre");
                first_playing_audio_ops->pause();
            }
        } else {
            LOGD(TAG, "not in playing");
        }
        ret = smtaudio_node->start(url, seek_time, resume);
        if (smtaudio_node->url) {
            free(smtaudio_node->url);
        }
        smtaudio_node->url = strdup(url);
        smtaudio_ctx.resume_flag = resume;
    } else {
        LOGD(TAG, "%s ops not found", __func__);
    }
    SMTAUDIO_UNLOCK();
    return ret;
}

int8_t smtaudio_pause()
{
    smtaudio_ops_node_t *tmp_node = NULL;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d] action [%d]", __FUNCTION__, smtaudio_ctx.cur_state, smtaudio_ctx.cur_action);

    if (smtaudio_ctx.cur_action != SMTAUDIO_ACTION_NULL) {
        return 0;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }

    //重复调用暂停，直接返回
    if ( smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_ONLINE_PAUSE ||
        smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_ONLINE_STOP ||
        smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_BT_A2DP_PAUSE ||
        smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_BT_A2DP_STOP ||
        smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_BT_HFP_PAUSE ||
        smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_BT_HFP_STOP
       ) {
        return 0;
    }

    LOGD(TAG, "do pause ...");

    SMTAUDIO_LOCK();
    actmgr_set_action(SMTAUDIO_ACTION_PAUSE);

    smtaudio_clear_ready_list();
    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            tmp_node->pause();
            smtaudio_resume_list_node_t *node =
                        (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
            node->id               = tmp_node->id;
            node->prio             = tmp_node->prio;
            node->interrupt_reason = SMTAUDIO_INTERRUPT_REASON_BY_USER;
            node->valid = 1;
            smtaudio_resume_list_insert(node);
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_stop(int type)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    SMTAUDIO_LOCK();
    if(SMTAUDIO_TYPE_ALL == type) {
        dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
            if (tmp_node->status == SMTAUDIO_STATE_PLAYING || tmp_node->status == SMTAUDIO_STATE_PAUSE) {
                tmp_node->stop();
                if (tmp_node->url) {
                    free(tmp_node->url);
                    tmp_node->url = NULL;
                }
            }
        }
    } else if((type >=0) && (type <SMTAUDIO_PLAY_TYPE_NUM)) {
        tmp_node = get_smtaudio_ctrl_ops_by_id(type);
        if (tmp_node) {
            if (tmp_node->status == SMTAUDIO_STATE_PLAYING || tmp_node->status == SMTAUDIO_STATE_PAUSE) {
                tmp_node->stop();
                if (tmp_node->url) {
                    free(tmp_node->url);
                    tmp_node->url = NULL;
                }
            }
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_mute()
{
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    
    //重复调用静音，直接返回
    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE){
        return 0;
    }

    pauseBeforeMute=false;  
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE){
        dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
            if(tmp_resume_list_node->id == MEDIA_MUSIC){
                if(!tmp_resume_list_node->valid || tmp_resume_list_node->interrupt_reason==SMTAUDIO_INTERRUPT_REASON_BY_USER){
                    pauseBeforeMute=true;//语音暂停/按键暂停
                }
                break;
            }
        }
    }
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PLAYING){
        //pause内判断cur_state，只能先调用
        smtaudio_pause();
    }

    SMTAUDIO_LOCK();
    switch_state(smtaudio_ctx.cur_state, SMTAUDIO_SUBSTATE_MUTE);
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_unmute()
{
    smtaudio_ops_node_t *tmp_node = NULL;
    int flag=0;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    
    //重复调用取消静音，直接返回
    if (smtaudio_ctx.cur_state != SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    //先切换状态
    SMTAUDIO_LOCK();
    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            flag=1;
            switch_state(SMTAUDIO_SUBSTATE_MUTE, tmp_node->id * 3 + SMTAUDIO_STATE_PLAYING);
            break;
        }
    }
    if(flag==0){
        switch_state(SMTAUDIO_SUBSTATE_MUTE, SMTAUDIO_SUBSTATE_LOCAL_STOP);
    }
    SMTAUDIO_UNLOCK(); 
    //修改前bug:播放->按键暂停->静音->取消静音  ->恢复播放（应该还是暂停状态才对）
    //修改前语音暂停，因为此处没有调用smtaudio_enable_ready_list(1)，所以不会恢复播放
    smtaudio_ctx.cur_action  = SMTAUDIO_ACTION_NULL;
    if(!pauseBeforeMute){
        smtaudio_resume();
    }
    return 0;
}


int8_t smtaudio_resume()
{
    int                  ret = -1;
    smtaudio_ops_node_t *smtaudio_node;
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    smtaudio_state_t state = smtaudio_get_state();
    if (state != SMTAUDIO_STATE_PAUSE && state != SMTAUDIO_STATE_STOP) {
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d] action [%d]", __FUNCTION__, smtaudio_ctx.cur_state, smtaudio_ctx.cur_action);

    if (smtaudio_ctx.cur_action != SMTAUDIO_ACTION_NULL) {
        return -1;
    }

    SMTAUDIO_LOCK();
    dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if((tmp_resume_list_node) && (tmp_resume_list_node->valid)) {
            smtaudio_node = get_smtaudio_ctrl_ops_by_id(tmp_resume_list_node->id);
            if ((smtaudio_node) && (smtaudio_node->status != SMTAUDIO_STATE_PLAYING)) {
                ret = smtaudio_node->resume();
                smtaudio_resume_list_rm(tmp_resume_list_node);
                break;
            }
        }
    }
    SMTAUDIO_UNLOCK();
    return ret;
}

void smtaudio_clear_ready_list(void)
{
    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    smtaudio_delay_list_clear();
    smtaudio_resume_list_clear();
}

void smtaudio_enable_ready_list(int enable)
{
    smtaudio_delay_list_node_t *tmp_delay_list_node = NULL;
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    dlist_for_each_entry(&smtaudio_delay_list_head, tmp_delay_list_node, smtaudio_delay_list_node_t, node) {
        if(tmp_delay_list_node) {
            tmp_delay_list_node->valid = enable;
            LOGD(TAG, "set type:%d in delay list, delay reason:%d set valid:%d", tmp_delay_list_node->id, tmp_delay_list_node->delay_reason, enable);
        }
    }

    dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if(tmp_resume_list_node) {
            tmp_resume_list_node->valid = enable;
            LOGD(TAG, "set type:%d in resume list, interrupt reason:%d valid:%d", tmp_resume_list_node->id, tmp_resume_list_node->interrupt_reason, enable);
        }
    }
}

void smtaudio_check_ready_list(void)
{
    smtaudio_delay_list_node_t *tmp_delay_list_node = NULL;
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    dlist_for_each_entry(&smtaudio_delay_list_head, tmp_delay_list_node, smtaudio_delay_list_node_t, node) {
        if(tmp_delay_list_node) {
            LOGD(TAG, "type:%d in delay list, delay reason:%d valid:%d", tmp_delay_list_node->id, tmp_delay_list_node->delay_reason, tmp_delay_list_node->valid);
        }
    }

    dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if(tmp_resume_list_node) {
            LOGD(TAG, "type:%d in resume list, interrupt reason:%d valid:%d", tmp_resume_list_node->id, tmp_resume_list_node->interrupt_reason, tmp_resume_list_node->valid);
        }
    }
}

bool smtaudio_check_resume_list_by_id(int id)
{
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return false;
    }


    dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if(tmp_resume_list_node->id == id && tmp_resume_list_node->valid && tmp_resume_list_node->interrupt_reason!=SMTAUDIO_INTERRUPT_REASON_BY_USER) {
            return true;
        }
    }
    return false;
}

smtaudio_state_t smtaudio_get_state(void)
{
    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return SMTAUDIO_STATE_NOINIT;
    }

    switch (smtaudio_ctx.cur_state) {
        case SMTAUDIO_SUBSTATE_MUTE:
            return SMTAUDIO_STATE_MUTE;
        case SMTAUDIO_SUBSTATE_ONLINE_PLAYING:
        case SMTAUDIO_SUBSTATE_LOCAL_PLAYING:
        case SMTAUDIO_SUBSTATE_BT_A2DP_PLAYING:
        case SMTAUDIO_SUBSTATE_BT_HFP_PLAYING:
            return SMTAUDIO_STATE_PLAYING;
        case SMTAUDIO_SUBSTATE_ONLINE_PAUSE:
        case SMTAUDIO_SUBSTATE_LOCAL_PAUSE:
        case SMTAUDIO_SUBSTATE_BT_A2DP_PAUSE:
        case SMTAUDIO_SUBSTATE_BT_HFP_PAUSE:
            return SMTAUDIO_STATE_PAUSE;
        case SMTAUDIO_SUBSTATE_ONLINE_STOP:
        case SMTAUDIO_SUBSTATE_LOCAL_STOP:
        case SMTAUDIO_SUBSTATE_BT_A2DP_STOP:
        case SMTAUDIO_SUBSTATE_BT_HFP_STOP:
            return SMTAUDIO_STATE_STOP;
    default:
        break;
    }
    return SMTAUDIO_STATE_STOP;
}

smtaudio_state_t smtaudio_get_state_by_id(int id)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->id == id) {
            return tmp_node->status;
        }
    }
    return SMTAUDIO_STATE_NOINIT;
}

smtaudio_player_type_t smtaudio_get_play_type(void)
{
    int ret = -1;

    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_SUBSTATE_ONLINE_PLAYING:
    case SMTAUDIO_SUBSTATE_ONLINE_PAUSE:
        ret = SMTAUDIO_ONLINE_MUSIC;
        break;
    case SMTAUDIO_SUBSTATE_BT_A2DP_PLAYING:
    case SMTAUDIO_SUBSTATE_BT_A2DP_PAUSE:
        ret = SMTAUDIO_BT_A2DP;
        break;
    case SMTAUDIO_SUBSTATE_BT_HFP_PLAYING:
        ret = SMTAUDIO_BT_HFP;
        break;
    case SMTAUDIO_SUBSTATE_LOCAL_PLAYING:
    case SMTAUDIO_SUBSTATE_LOCAL_PAUSE:
        ret = SMTAUDIO_LOCAL_PLAY;
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

char *smtaudio_get_play_url(void)
{
    smtaudio_ops_node_t *audio_ops = find_first_playing_audio();
    if (audio_ops == NULL) {
        audio_ops = find_first_pause_audio();
    }

    if (audio_ops) {
        return audio_ops->url;
    }

    return NULL;
}

char *smtaudio_get_url_by_id(int id)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->id == id) {
            return tmp_node->url;
        }
    }
    return NULL;
}

int smtaudio_get_duration_by_id(int id)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->id == id) {
            return tmp_node->duration;
        }
    }
    return 0;
}

void smtaudio_substate_get(int *cur_state, int *last_state)
{
    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    *cur_state  = smtaudio_ctx.cur_state;
    *last_state = smtaudio_ctx.last_state;
}

int8_t smtaudio_lpm(uint8_t state)
{
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();
    aos_kernel_sched_suspend();
    smtaudio_ctx.lpm_flag = state;
    aos_kernel_sched_resume();
    csi_irq_restore(irq_flag);

    return 0;
}

int smtaudio_enter_lpm_check(void)
{
    smtaudio_ops_node_t *tmp_node = NULL;
    dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            return 0;
        }
    }
    return 1;
}

int8_t smtaudio_ops_register(smtaudio_ops_node_t *ops)
{
    aos_assert(ops);

    if(!aos_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if((smtaudio_crtl_id & (1 << ops->id)) != 0) {
        LOGE(TAG, "id:%d has been register", ops->id);
        return -1;
    }

    smtaudio_crtl_id |= (1 << ops->id);
    smtaudio_ctrl_list_pri_insert(ops);
    ops->callback = smtaudio_event_callback;

    return 0;
}
