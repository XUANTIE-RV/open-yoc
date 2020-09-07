#include <yoc_config.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <csi_core.h>
#include <drv/codec.h>

#include <yv_ap.h>
#include <ipc.h>
#include "yv_defs.h"

// #include "ai/ai_text.h"
// #include "ai/ai_ro.h"
// #include "ai/ai_dnn.h"

#define TAG "AP-YV"

// FIXME:
// #define YV_RECODE_DEBUG
#ifdef YV_RECODE_DEBUG
uint32_t g_mem_adc[0x100000/4];
int g_mem_adc_len = 0;
uint32_t g_mem_ref[0x100000/4];
int g_mem_ref_len = 0;
#endif
// #define RECODE_DEBUG_TEST
#ifdef RECODE_DEBUG_TEST
int g_rec_start = 0;
uint32_t g_pcm_data[160 * 1024 /4] __attribute__ ((aligned (16)));
uint32_t g_pcm_len = 0;
uint32_t g_mic_data[160 * 1024 /4] __attribute__ ((aligned (16)));
uint32_t g_mic_len = 0;
uint32_t g_ref_data[160 * 1024 /4] __attribute__ ((aligned (16)));
uint32_t g_ref_len = 0;
static void __record_pcm_data(void *data)
{
    voice_t *vdata = (voice_t *)data;
    void* frame = vdata->data;
    int flen = vdata->len;
    int type = vdata->type;
    int vad = vdata->vad;

    if (g_rec_start == 0) {
        printf("rec stop...\n");
        return;
    }
    if (g_pcm_len % 32000 == 0) {
        printf(".%d,%d,%d,%d\n", g_pcm_len, flen, type, vad);
    }

    if (g_pcm_len + flen < sizeof(g_pcm_data)) {
        memcpy((void *)((uint32_t)g_pcm_data + g_pcm_len), frame, flen);
        g_pcm_len += flen;                
    }         
    if (g_mic_len + flen < sizeof(g_mic_data)) {
        memcpy((void *)((uint32_t)g_mic_data + g_mic_len), frame + 640, flen);
        g_mic_len += flen;
    }
    if (g_ref_len + flen < sizeof(g_ref_data)) {
        memcpy((void *)((uint32_t)g_ref_data + g_ref_len), frame + 1280, flen);
        g_ref_len += flen;
    }
}
#endif

#define MESSAGE_NUM 10

struct _yv_ap_ {
    ipc_t *ipc;
    yv_pcm_param_t *param;

    aos_task_t task;
    aos_queue_t queue;
    uint8_t buffer[sizeof(message_t) * MESSAGE_NUM];

    yv_event_t cb;
    void *priv;
};

static void *ipc_cmd_send(ipc_t *ipc, int cmd, void *data, int len, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.command = cmd;
    msg.service_id = YV_SERVICE_ID;
    msg.data = data;
    msg.len = len;
    msg.flag = sync;

    //   csi_dcache_clean_range(msg.data, msg.len);
    ipc_message_send(ipc, &msg, AOS_WAIT_FOREVER);

    return msg.data;
}

static void yv_vad_event(yv_t *yv, int type)
{
    int evt_id = YV_VAD_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, (void *)type, sizeof(int));
    }
}

// static void yv_pcm_data_event(yv_t *yv, void *data, int len)
// {
//     int evt_id = YV_VOICE_EVT;

//     if (yv->cb) {
//         yv->cb(yv->priv, evt_id, data, len);
//     }
// }

static void yv_voice_data_event(yv_t *yv, void *data)
{
    int evt_id = YV_VOICE_EVT;
    voice_t *vdata = (voice_t *)data;

#ifdef RECODE_DEBUG_TEST
    __record_pcm_data(data);
#endif
    if (yv->cb && vdata->data && vdata->len > 0) {
        yv->cb(yv->priv, evt_id, data, sizeof(voice_t));
    }
}

static void yv_asr_event(yv_t *yv, int type)
{
    int evt_id = YV_ASR_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, (void *)&type, sizeof(int));
    }
}

static void yv_silence_event(yv_t *yv)
{
    int evt_id = YV_SILENCE_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, NULL, 0);
    }
}

static void _msg_process(yv_t *yv, message_t *msg)
{
    switch (msg->command) {
        // case YV_PCM_DATA_CMD: {
        //     yv_pcm_data_event(yv, msg->data, msg->len);
        //     csi_dcache_clean_range(msg->data, msg->len);
        //     break;
        // }

        case YV_VOICE_DATA_CMD: {
            yv_voice_data_event(yv, msg->data);
#ifndef CONFIG_CHIP_PANGU_CPU0
            voice_t *vdata = (voice_t *)msg->data;
            csi_dcache_clean_range(vdata->data, vdata->len);
#endif
            break;
        }

        case YV_ASR_CMD: {
            yv_asr_event(yv, (int)msg->data);
            break;
        }

        case YV_SILENCE_CMD: {
            yv_silence_event(yv);
            break;
        }

        case YV_VAD_CMD: {
            yv_vad_event(yv, (int)msg->data);
            break;
        }
        default:
            return;
        }

        if (msg->flag & MESSAGE_SYNC) {
            //csi_dcache_clean_range(msg->data, msg->len);
            ipc_message_ack(yv->ipc, msg, AOS_WAIT_FOREVER);
        }
}

static void param_init(yv_pcm_param_t *param)
{
    param->buffer_num = BUFFER_NUM;
    param->buffer_size = BUFFER_SIZE;
    param->sentence_time_ms = SENTENCE_TIME;

    param->channels = CHANNELS;
    param->sample_bits = SAMPLE_BITS;
    param->rate = RATE;
    param->encode = ENCODE;
}

static void _ipc_process(ipc_t *ipc, message_t *msg, void *priv)
{
    yv_t *yv = (yv_t *)priv;
    int ret = aos_queue_send(&yv->queue, msg, sizeof(message_t));

    if (ret < 0) {
        // LOGE(TAG, "queue send failed");
        if (msg->flag & MESSAGE_SYNC) {
            ipc_message_ack(ipc, msg, AOS_WAIT_FOREVER);
        }
    }
}

static void yv_entry(void *arg)
{
    yv_t *yv = (yv_t *)arg;
    message_t msg;
    uint32_t len;

    while (1) {
        aos_queue_recv(&yv->queue, AOS_WAIT_FOREVER, &msg, &len);
        _msg_process(yv, &msg);
    }
}

#include <yoc/partition.h>
#define reg_wr(reg_addr) *((volatile uint32_t *)reg_addr)
#define MIN(a, b) (a)<(b)? (a) : (b)
extern void mdelay(uint32_t ms);

#ifdef CONFIG_CHIP_PANGU_CPU0

#define FLASH_BASE_ADDR2 0x8000000
#define TCM0_ADDR (0x80010000)

static aos_sem_t g_adc_sem;
static aos_sem_t g_adc_ref_sem;
static aos_event_t g_adc_send_event;

#define DSIZE_80MS (32 * 80)

#define MIC_PERIOD DSIZE_80MS
#define REF_PERIOD DSIZE_80MS

#define ADC_EVENT_INIT  0
#define ADC_MIC_EVENT   1
#define ADC_REF_EVENT   (1 << 1)


uint16_t *g_yv_mem = (uint16_t *)TCM0_ADDR;
codec_input_t codec_adc_handle;
codec_input_t codec_ref_handle;

// TODO: ref
static void adc_ref_event_cb(codec_event_t event, void *arg)
{
    static int ref_full_cnt = 0;

    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE ||
        event == CODEC_EVENT_READ_BUFFER_FULL) {
        if (event == CODEC_EVENT_READ_BUFFER_FULL) {
            printf("ref full....[%d]\n", ++ref_full_cnt);
            csi_codec_input_buf_reset(&codec_ref_handle);
            return;
        }
        if (g_adc_ref_sem.hdl)
            aos_sem_signal(&g_adc_ref_sem);
    }
}

static void adc_ref_init(void)
{
    codec_ref_handle.buf = aos_malloc_check(1024 * 30);
    codec_ref_handle.buf_size = 1024 * 30;
    codec_ref_handle.cb = adc_ref_event_cb;
    codec_ref_handle.cb_arg = NULL;
    codec_ref_handle.ch_idx = 2;
    codec_ref_handle.codec_idx = 0;
    codec_ref_handle.period = REF_PERIOD;

    int ret = csi_codec_input_open(&codec_ref_handle);
    if (ret != 0) {
        printf("codec ref input open error\n");
        aos_free(codec_ref_handle.buf);
        return;
    }

    codec_input_config_t config;
    config.bit_width = 16;
    config.channel_num = 1;
    config.sample_rate = 16000;
    ret = csi_codec_input_config(&codec_ref_handle, &config);
    if (ret != 0) {
        printf("codec input config error\n");
        aos_free(codec_ref_handle.buf);
        return;
    }

    csi_codec_input_set_analog_gain(&codec_ref_handle, 0);
}

void codec_adc_ref_proc_task(void *arg)
{
    // yv_t *yv = (yv_t *)(arg);
    // codec_adc_t codec_ref_handle;
    int ret;
    int rec_size;

    ret = aos_sem_new(&g_adc_ref_sem, 1);

    if (ret != 0) {
        LOGE(TAG, "adc ref fail to create semaphore.\n");
        return;
    }
    
    // drv_codec_adc_start(&codec_ref_handle);

    void *ref_mem = (void *)((uint32_t)g_yv_mem + MIC_PERIOD);
    while(1) {
        aos_sem_wait(&g_adc_ref_sem, -1);
        rec_size = csi_codec_input_read(&codec_ref_handle, (void *)ref_mem, REF_PERIOD);
#ifdef YV_RECODE_DEBUG
        if (rec_size != REF_PERIOD) {
            printf("ref rec_size error,%d\n", rec_size);
        }
        if (g_mem_ref_len + rec_size > sizeof(g_mem_ref)) {
            // g_mem_ref_len = 0;
        } else {
            memcpy((void *)((uint32_t)g_mem_ref + g_mem_ref_len), ref_mem, rec_size);
            g_mem_ref_len += rec_size;            
        }
#endif
        if (rec_size == REF_PERIOD) {
            if (g_adc_send_event.hdl)
                aos_event_set(&g_adc_send_event, ADC_REF_EVENT, AOS_EVENT_OR);
        }
    }
}

// TODO: adc
static void test_adc_event_cb(codec_event_t event, void *arg)
{
    static int adc_full_cnt = 0;

    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE || 
        event == CODEC_EVENT_READ_BUFFER_FULL) {
        if (event == CODEC_EVENT_READ_BUFFER_FULL) {
            printf("adc full....[%d]\n", ++adc_full_cnt);
            csi_codec_input_buf_reset(&codec_adc_handle);
            return;
        }
        if (g_adc_sem.hdl)
            aos_sem_signal(&g_adc_sem);
    }
}

static void adc_init(void)
{
    codec_adc_handle.buf = aos_malloc_check(1024 * 30);
    codec_adc_handle.buf_size = 1024 * 30;
    codec_adc_handle.cb = test_adc_event_cb;
    codec_adc_handle.cb_arg = NULL;
    codec_adc_handle.ch_idx = 0;
    codec_adc_handle.codec_idx = 0;
    codec_adc_handle.period = MIC_PERIOD;

    int ret = csi_codec_input_open(&codec_adc_handle);
    if (ret != 0) {
        printf("codec mic input open error\n");
        aos_free(codec_adc_handle.buf);
        return;
    }

    codec_input_config_t config;
    config.bit_width = 16;
    config.channel_num = 1;
    config.sample_rate = 16000;
    ret = csi_codec_input_config(&codec_adc_handle, &config);
    if (ret != 0) {
        printf("codec input config error\n");
        aos_free(codec_adc_handle.buf);
        return;
    }

    csi_codec_input_set_analog_gain(&codec_adc_handle, 6);
}

void codec_adc_proc_task(void *arg)
{
    // codec_adc_t codec_adc_handle;
    // yv_t *yv = (yv_t *)(arg);
    int ret;
    int rec_size;

    ret = aos_sem_new(&g_adc_sem, 1);

    if (ret != 0) {
        LOGE(TAG, "adc task fail to create semaphore.\n");
        return;
    }

    // drv_codec_adc_start(&codec_adc_handle);

    void *mic_mem = (void *)((uint32_t)g_yv_mem);
    while(1) {
        aos_sem_wait(&g_adc_sem, -1);
        rec_size = csi_codec_input_read(&codec_adc_handle, mic_mem, MIC_PERIOD);
#ifdef YV_RECODE_DEBUG
        if (rec_size != MIC_PERIOD) {
            printf("adc rec_size error,%d\n", rec_size);
        }
        if (g_mem_adc_len + rec_size > sizeof(g_mem_adc)) {
            // g_mem_adc_len = 0;
        } else {
            memcpy((void *)((uint32_t)g_mem_adc + g_mem_adc_len), mic_mem, rec_size);
            g_mem_adc_len += rec_size;            
        }
#endif        
        if (rec_size == MIC_PERIOD) {
            if (g_adc_send_event.hdl)
                aos_event_set(&g_adc_send_event, ADC_MIC_EVENT, AOS_EVENT_OR);
        }
    }
}

static void adc_send_task(void *arg)
{
    yv_t *yv = (yv_t *)arg;
    unsigned int flag;
    aos_event_new(&g_adc_send_event, ADC_EVENT_INIT);

    while(1) {
        aos_event_get(&g_adc_send_event, ADC_MIC_EVENT | ADC_REF_EVENT, AOS_EVENT_AND_CLEAR, &flag, -1);
        ipc_cmd_send(yv->ipc, YV_PCM_DATA_CMD, g_yv_mem, MIC_PERIOD + REF_PERIOD, MESSAGE_ASYNC);
    }
}

void cpu1_init(void)
{
#define CPU1_RUN_ADDR   (0x18700000)
    partition_t p = partition_open("cpu1");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu1->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu1->length=0x%x", info->length);
    // partition_read(p, 0, (void *)CPU1_RUN_ADDR, info->length);
    memcpy((void *)CPU1_RUN_ADDR, (void *)(info->start_addr + FLASH_BASE_ADDR2), info->length);
    partition_close(p);

    // FIXME:
    reg_wr(0x30000080) = CPU1_RUN_ADDR + 8;
    reg_wr(0x30000008) &= ~0x1;
    reg_wr(0x30000008) |= 0x1;
    mdelay(200);
}

void ai_init(void)
{
#define CPU2_RUN_ADDR               (0x18780000)

    partition_t p = partition_open("cpu2");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu2->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu2->length=0x%x", info->length);
    // partition_read(p, 0, (void *)CPU1_RUN_ADDR, info->length);
    memcpy((void *)CPU2_RUN_ADDR, (void *)(info->start_addr + FLASH_BASE_ADDR2), info->length);
    partition_close(p);

    // FIXME:
    // const uint32_t real_start_addr = 0x187b7ef8;    // 0912-ok
    // const uint32_t real_start_addr = 0x187b7f04;    // #54
    // const uint32_t real_start_addr = 0x187b7f44;    //0924
    reg_wr(0x30000044) = CPU2_RUN_ADDR;
    reg_wr(0x30000008) &= ~0x02;
    reg_wr(0x30000008) |= 0x02;
    mdelay(200);
}

#else

void cpu1_init(void)
{
#define CPU1_RUN_ADDR   (0x60000)
    partition_t p = partition_open("cpu1");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu1->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu1->length=0x%x", info->length);
    partition_read(p, 0, (void *)CPU1_RUN_ADDR, info->length);
    partition_close(p);

    reg_wr(0x8002002c) = 0x1;
    reg_wr(0x80020010) |= 0x1;
    mdelay(600);
}

void ai_init(void)
{
#define DTCM_RUN_ADDR   (0x91000000)
#define PTCM_RUN_ADDR   (0x91200000)
#define SHAREMM_ADDR    (0xA0000)
#define DTCM_RUN_ADDR2  (0xA1000)
#define PTCM_RUN_ADDR2  (0xCA000)
#define DTCM2_MAX_SIZE  (0xCA000 - 0xA1000) //(250 * 1024)
#define PTCM2_MAX_SIZE  (SHAREMM_ADDR + 0x40000 - PTCM_RUN_ADDR2)

    // PMU_BASE->UD4_RSTCTRL &= ~0x80;
    reg_wr(0x80020010) &= ~0x80;
    mdelay(100);
    partition_t p = partition_open("dtcm1");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "dtcm1info->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "dtcm1info->length=0x%x", info->length);
    partition_read(p, 0, (void *)DTCM_RUN_ADDR, info->length);
    partition_close(p);

    p = partition_open("dtcm2");
    info = hal_flash_get_info(p);
    LOGD(TAG, "dtcm2info->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "dtcm2info->length=0x%x", info->length);
    LOGD(TAG, "dtcm2info->relength=0x%x", MIN(info->length, DTCM2_MAX_SIZE));
    partition_read(p, 0, (void *)DTCM_RUN_ADDR2, MIN(info->length, DTCM2_MAX_SIZE));
    partition_close(p);

    p = partition_open("ptcm");
    info = hal_flash_get_info(p);
    LOGD(TAG, "ptcminfo->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "ptcminfo->length=0x%x", info->length);
    partition_read(p, 0, (void *)PTCM_RUN_ADDR, info->length);
    partition_close(p);
    LOGD(TAG, "dsp copy over");
    csi_dcache_clean_invalid();
    csi_icache_invalid();

    mdelay(50);
    // PMU_BASE->UD10_APBCLKCTRL = 2;
    reg_wr(0x80020028) = 2;
    reg_wr(0x80020010) &= ~0x84;
    mdelay(500);
    reg_wr(0x80020010) |= 0x84;
    mdelay(50);
}
#endif /*CONFIG_CHIP_PANGU_CPU0*/

yv_t *yv_init(yv_event_t cb, void *priv)
{
    yv_t *yv = aos_malloc_check(sizeof(yv_t));

    yv->cb = cb;
    yv->priv = priv;

    int ret = aos_queue_new(&yv->queue, yv->buffer, MESSAGE_NUM * sizeof(message_t), sizeof(message_t));
    aos_check(!ret, EIO);

    ret = aos_task_new_ext(&yv->task, "yv", yv_entry, yv, 1024 * 4, AOS_DEFAULT_APP_PRI - 7);
    aos_check(!ret, EIO);

    yv->ipc = ipc_get(CP_DSP_IDX);
    ipc_add_service(yv->ipc, YV_SERVICE_ID, _ipc_process, yv);
    yv->param = aos_malloc_check(sizeof(yv_pcm_param_t));
    param_init(yv->param);

#ifdef CONFIG_CHIP_PANGU_CPU0
    aos_task_t task, task1, task2;
    
    adc_ref_init();
    adc_init();

    csi_codec_input_start(&codec_ref_handle);
    csi_codec_input_start(&codec_adc_handle);

    ret = aos_task_new_ext(&task, "adc-mic", codec_adc_proc_task, (void *)yv, 4096, 27);
    aos_check_param(!ret);
    ret = aos_task_new_ext(&task1, "adc-ref", codec_adc_ref_proc_task, (void *)yv, 4096, 27);
    aos_check_param(!ret);
    ret = aos_task_new_ext(&task2, "adc-send", adc_send_task, (void *)yv, 4096, 26);
    aos_check_param(!ret);
#endif
    return yv;
}

int yv_deinit(yv_t *yv)
{
    //todo ipc free

    aos_free(yv->param);

    aos_free(yv);
    return 0;
}

int yv_set_vad_timeout(yv_t *yv, int ms)
{
    int ret = *((int *)ipc_cmd_send(yv->ipc, YV_VAD_TIMEOUT_CMD, (void *)ms, sizeof(ms), MESSAGE_ASYNC));

    return ret;
}

//pcm
int yv_pcm_param_set(yv_t *yv, yv_pcm_param_t *hw_param)
{
    int ret = *((int *)ipc_cmd_send(yv->ipc, YV_PCM_SET_CMD, hw_param, sizeof(yv_pcm_param_t), MESSAGE_SYNC));

    return ret;
}

int yv_pcm_param_get(yv_t *yv, yv_pcm_param_t *hw_param)
{
    ipc_cmd_send(yv->ipc, YV_PCM_GET_CMD, hw_param, sizeof(yv_pcm_param_t), MESSAGE_ASYNC);

    return 0;
}

#if 0
static char *pcm_start = (char *)0xa6000;
static char *yv_pcm_copy(yv_t *yv, const void *data, int len)
{
    memcpy(pcm_start, data, len);

    return pcm_start;
}
#endif

int yv_pcm_send(yv_t *yv, void *pcm, int len)
{
    if (yv && pcm && len > 0) {
        csi_dcache_clean_range(pcm, len);
        ipc_cmd_send(yv->ipc, YV_PCM_DATA_CMD, pcm, len, MESSAGE_ASYNC);
        return 0;
    }
    return -1;

    // char *s_buf = yv_pcm_copy(yv, pcm, len);

    // if (pcm && s_buf && len > 0) {
    //     ipc_cmd_send(yv->ipc, YV_PCM_DATA_CMD, s_buf, len, MESSAGE_ASYNC);
    //     return 0;
    // }

    // return -1;
}

int yv_pcm_auto(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_PCM_ENABLE_CMD, (void *)0, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_pcm_enable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_PCM_ENABLE_CMD, (void *)1, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_pcm_disable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_PCM_ENABLE_CMD, (void *) -1, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_kws_enable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_KWS_ENABLE_CMD, (void *)1, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_asr_disable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_KWS_ENABLE_CMD, (void *)0, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_aec_enable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_AEC_ENABLE_CMD, (void *)1, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_aec_disable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_AEC_ENABLE_CMD, (void *)0, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_vad_enable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_VAD_ENABLE_CMD, (void *)1, 1, MESSAGE_ASYNC);

    return 0;
}

int yv_vad_disable(yv_t *yv)
{
    ipc_cmd_send(yv->ipc, YV_VAD_ENABLE_CMD, NULL, 0, MESSAGE_ASYNC);

    return 0;
}
