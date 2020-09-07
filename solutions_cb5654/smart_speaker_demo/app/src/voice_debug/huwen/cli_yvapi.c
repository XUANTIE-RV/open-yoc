/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <aos/aos.h>
#include <aos/cli.h>

#include <aos/debug.h>
#include <yoc/uservice.h>
#include <yoc/mic.h>
#include <media.h>
#include <vfs.h>

#include "mic_adpator/huwen_yv_ap.h"
#define yv_kws_enable(x,y) huwen_yv_kws_enable(y)
#define yv_pcm_enable(x,y) huwen_yv_pcm_enable(y)
#define yv_wake_trigger(x,y)   huwen_yv_wake_trigger(y)
#define yv_debug_hook      huwen_yv_debug_hook

#define TAG "yvtest"

#if 0
#define RECORD_BUFFER_ADDR 0x02700000
#define RECORD_BUFFER_SIZE (1024*1024*1)
#define RECORD_BUFFER_BLOCK_SIZE (3*1024+256)
#define RECORD_BUFFER_BLOCK_COUNT (RECORD_BUFFER_SIZE/RECORD_BUFFER_BLOCK_SIZE)

static uint8_t *g_pcm_record = (uint8_t *)RECORD_BUFFER_ADDR;
static int g_pcm_mem_save2sd = 0;
static int g_print_pcm_data = 0;
static int g_wake_record_en = 0;
#endif

/* 16K采样、20ms、单通道字节数*/
#define RECORD_DATA_BYTE_PERCH (20 * 16 * 2)

#define RECORD_DIR_NAME "/fatfs0/yvrec"
#define RECORD_COUNT_FILE "/fatfs0/yvrec/count.txt"
#define RECORD_DATA_FILE "/fatfs0/yvrec/REC%d.RAW"

static void sd_speed_test(int buf_size, int loop_count)
{
    printf("buf size = %d, loop count = %d\n", buf_size, loop_count);

    int fd = aos_open("/fatfs0/yvrec/spt.dat", O_WRONLY | O_CREAT | O_TRUNC);
    uint8_t * buf = aos_malloc(buf_size);

    long long begin = aos_now_ms();
    if (fd) {
        for(int i = 0; i < loop_count; i++) {
            int ret = aos_write(fd, buf, buf_size);
            if (ret != buf_size) {
                LOGE(TAG,"write error %d", ret);
            }
        }
    }
    long long end = aos_now_ms();

    aos_close(fd);
    aos_free(buf);
    printf("sd speed = %lld KB/s %lld ms\n", (buf_size * loop_count) / (end - begin), (end - begin));
}

static int g_rec_fd = -1;
static int g_rec_count = -1;
static int record_read_count(void)
{
    int count = 0;
    char str_count[16];

    /* create dir, ready to record */
    aos_mkdir(RECORD_DIR_NAME);

    int rccfd = aos_open(RECORD_COUNT_FILE, O_RDONLY);
    if (rccfd > 0) {
        int ret = aos_read(rccfd, str_count, sizeof(str_count));
        if (ret > 0) {
            count = atoi(str_count);
        }
        aos_close(rccfd);
    }

    return count;
}

static int record_write_count(int count)
{
    int ret = -1;
    char str_count[16];

    sprintf(str_count, "%d\n", count);

    int rccfd = aos_open(RECORD_COUNT_FILE, O_WRONLY | O_CREAT | O_TRUNC);
    if (rccfd > 0) {
        ret = aos_write(rccfd, str_count, strlen(str_count));
        aos_close(rccfd);
    }

    if(ret < 0) {
        LOGE(TAG,"Sync rec count error");
    }

    return ret;
}

static int record_create_file(void)
{
    char filename[32];

    if (g_rec_fd > 0) {
        LOGI(TAG,"Already in Record");
        return -1;
    }

    if (g_rec_count > 50000) {
        LOGI(TAG,"count > %d", g_rec_count);
        return -1;
    }

    if (g_rec_count < 0) {
        g_rec_count = record_read_count();
    }

    sprintf(filename, RECORD_DATA_FILE, g_rec_count);

    LOGI(TAG,"Record %s", filename);
    g_rec_fd = aos_open(filename, O_WRONLY | O_CREAT);

    if (g_rec_fd < 0) {
        LOGE(TAG,"Create %s error", filename);
        return -1;
    }

    return 0;
}

static int record_write_file5(uint8_t * buf, int size)
{
    if (g_rec_fd < 0) {
        return -1;
    }

    if (size != 4096) {
        LOGE(TAG,"write_file %d", size);
        return -1;
    }

#define RECORD_NUM 8
    static uint8_t mic_ref_ace[4096 * RECORD_NUM];
    static uint8_t record_cnt = 0;

    memcpy(&mic_ref_ace[4096 * record_cnt], buf, 4096);
    record_cnt ++;

    if (record_cnt >=  RECORD_NUM) {
        int wsize = aos_write(g_rec_fd, mic_ref_ace, 4096 * record_cnt);
        if (wsize != size * record_cnt) {
            LOGE(TAG,"write error %d", wsize);
        }
        record_cnt = 0;
    }

    return size;
}

static void record_close_file(void)
{
    aos_close(g_rec_fd);
    g_rec_fd = -1;

    if (g_rec_count >= 0) {
        g_rec_count ++;
        record_write_count(g_rec_count);
    }

    LOGI(TAG,"Record End");
}

static uservice_t  *g_sdrec_usrv = NULL;
typedef enum {
    SDREC_OPEN,
    SDREC_WRITE,
    SDREC_CLOSE,
    SDREC_MEM,
    SDREC_SDSPD,
    SDREC_END_CMD
} MIC_CMD;
typedef struct _sdrec_write_param {
    uint8_t *buf;
    int size;
} sdrec_write_param_t;

typedef struct _sdrec_sdspd_param {
    int block_size;
    int count;
} sdrec_sdspd_param_t;

static int sdrec_open(void *context, rpc_t *rpc);
static int sdrec_write(void *context, rpc_t *rpc);
static int sdrec_close(void *context, rpc_t *rpc);
static int sdrec_mem(void *context, rpc_t *rpc);
static int sdrec_sdspd(void *context, rpc_t *rpc);
static const rpc_process_t g_sdrec_usrv_proc[] = {
    {SDREC_OPEN,          (process_t)sdrec_open},
    {SDREC_WRITE,         (process_t)sdrec_write},
    {SDREC_CLOSE,         (process_t)sdrec_close},
    {SDREC_MEM,           (process_t)sdrec_mem},
    {SDREC_SDSPD,          (process_t)sdrec_sdspd},
    {SDREC_END_CMD,       (process_t)NULL},
};

static int sdrec_open(void *context, rpc_t *rpc)
{
    record_create_file();
    return 0;
}

static int sdrec_write(void *context, rpc_t *rpc)
{
    sdrec_write_param_t *param = rpc_get_buffer(rpc, NULL);

    record_write_file5(param->buf, param->size);

    aos_free(param->buf);

    return 0;
}

static int sdrec_close(void *context, rpc_t *rpc)
{
    record_close_file();
    return 0;
}

static int sdrec_sdspd(void *context, rpc_t *rpc)
{
    sdrec_sdspd_param_t *param = rpc_get_buffer(rpc, NULL);

    sd_speed_test(param->block_size, param->count);

    return 0;
}

static int sdrec_mem(void *context, rpc_t *rpc)
{
#if 0
    g_pcm_mem_save2sd = 1;

    int ret = record_create_file();

    if (ret == 0) {

        for(int i = 0; i < RECORD_BUFFER_BLOCK_COUNT; i++) {
            uint8_t *ptr = &g_pcm_record[i * RECORD_BUFFER_BLOCK_SIZE];
            record_write_file5(ptr, RECORD_DATA_BYTE_PERCH * 5);
        }

        record_close_file();
    }

    g_pcm_mem_save2sd = 0;
#endif
    return 0;
}

int sdrec_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, g_sdrec_usrv_proc);
}

int sdrec_call_open(void)
{
    int ret = -1;

    aos_check_return_einval(g_sdrec_usrv);

    ret = uservice_call_async(g_sdrec_usrv, SDREC_OPEN, NULL, 0);
    return ret;
}

int sdrec_call_write(uint8_t * buf, int size)
{
    int ret = -1;
    sdrec_write_param_t param;
    param.buf = buf;
    param.size = size;

    aos_check_return_einval(g_sdrec_usrv);

    ret = uservice_call_async(g_sdrec_usrv, SDREC_WRITE, (void*) &param, sizeof(param));
    return ret;
}

int sdrec_call_close(void)
{
    int ret = -1;

    ret = uservice_call_async(g_sdrec_usrv, SDREC_CLOSE, NULL, 0);
    return ret;
}

int sdrec_call_mem(void)
{
    int ret = -1;

    ret = uservice_call_async(g_sdrec_usrv, SDREC_MEM, NULL, 0);
    return ret;
}

int sdrec_call_sdspd(int block_size, int count)
{
    int ret = -1;
    sdrec_sdspd_param_t param;
    param.block_size = block_size;
    param.count = count;

    ret = uservice_call_async(g_sdrec_usrv, SDREC_SDSPD, (void*) &param, sizeof(param));
    return ret;
}

void sd_record_usrv_init(void)
{
    utask_t *task = utask_new("sdrec", 3 * 1024, 150, AOS_DEFAULT_APP_PRI + 5);
    g_sdrec_usrv = uservice_new("sdrec", sdrec_process_rpc, NULL);
    utask_add(task, g_sdrec_usrv);
}

#if 0
/* 
 * 核间通讯的数据临时存放到 RECORD_BUFFER_ADDR
 * 存放两个通道数据(mic/ref/aec)
*/
static int g_pcm_save2mem = 0;  /* 0:不录制  1 2 3为三种类型 */
static int g_pcm_save2mem_pos = 0; /* 双字节 最大RECORD_BUFFER_SIZE/2*/
int pcm_to_mem_size(void)
{
    return g_pcm_save2mem_pos * 2;
}

static int pcm_to_mem(uint8_t * buf, int size, int type)
{
    int i;

    short * mic1 = (short * ) buf;
    short * mic2 = (short * ) &buf[size/5];
    short * ref1 = (short * ) &buf[size/5 * 2];
    short * ref2 = (short * ) &buf[size/5 * 3];
    short * ace  = (short * ) &buf[size/5 * 4];

    /* 按short型计算 单路的采样点数 */
    int single_sample_count = size/5/2;

    if (g_pcm_save2mem_pos + single_sample_count * 2 > RECORD_BUFFER_SIZE / 2)
    {
        /* 录制满了 */
        return -1;
    }

    short * mem_rec = (short *) g_pcm_record;

    switch(type){
    case 1:
        for (i = 0; i < single_sample_count; i++) {
            mem_rec[g_pcm_save2mem_pos + 0] = mic1[i];
            mem_rec[g_pcm_save2mem_pos + 1] = mic2[i];
            g_pcm_save2mem_pos += 2;
        }
        break;
    case 2:
        for (i = 0; i < single_sample_count; i++) {
            mem_rec[g_pcm_save2mem_pos + 0] = ref1[i];
            mem_rec[g_pcm_save2mem_pos + 1] = ref2[i];
            g_pcm_save2mem_pos += 2;
        }
        break;
    case 3:
        for (i = 0; i < single_sample_count; i++) {
            mem_rec[g_pcm_save2mem_pos + 0] = ace[i];
            mem_rec[g_pcm_save2mem_pos + 1] = 0;
            g_pcm_save2mem_pos += 2;
        }
        break;
    case 4:
        for (i = 0; i < single_sample_count; i++) {
            mem_rec[g_pcm_save2mem_pos + 0] = mic1[i];
            mem_rec[g_pcm_save2mem_pos + 1] = ace[i];
            g_pcm_save2mem_pos += 2;
        }
        break;
    case 5:
        for (i = 0; i < single_sample_count; i++) {
            mem_rec[g_pcm_save2mem_pos + 0] = mic2[i];
            mem_rec[g_pcm_save2mem_pos + 1] = ace[i];
            g_pcm_save2mem_pos += 2;
        }
        break;
    default:
        ;
    }

    return 0;
}
#endif

static void play_record(int type);
static void yv_event_cb(void *priv, mic_event_id_t evt_id, void *data, int len);
static void cmd_cxc_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    //int ret;
    static int yv_debug = 0;

    if (argc >= 2) {

        if (strcmp(argv[1], "debug") == 0) {
            if (argc >= 3) {
                yv_debug = atoi(argv[2]);
                printf("yv debug %d\n", yv_debug);

                switch(yv_debug){
                case 1:
                    yv_debug_hook(yv_event_cb, yv_debug);
                    yv_kws_enable(g_yv_handle, 256); /* debug 启动内存录制 */
                    break;
                case 2:
                    yv_debug_hook(yv_event_cb, yv_debug);
                    break;
                default:
                    yv_debug_hook(NULL, 0);
                    yv_kws_enable(g_yv_handle, 1);
                }
            } else {
				printf("yv debug 0/1/2\n");
            }
            return;
        }

        if (yv_debug == 0) {
            printf("run 'yv debug' first\n");
            return;
        }

        if (strcmp(argv[1], "pcm") == 0) {
            if (argc == 3) {
                //if (g_pcm_mem_save2sd == 0) {
                    yv_pcm_enable(g_yv_handle, atoi(argv[2]));
                //} else {
                //    printf("mem2sd wait\n");
                //}
            }
        } else if (strcmp(argv[1], "asr") == 0) {
            if (argc == 3) {
                yv_kws_enable(g_yv_handle, atoi(argv[2]));
            }
        } else if (strcmp(argv[1], "wake") == 0) {
            if (argc == 3) {
                yv_wake_trigger(g_yv_handle, 0);
            }
        } else if (strcmp(argv[1], "wakerec") == 0) {
            printf("unsupport\n");
        } else if (strcmp(argv[1], "memrec") == 0) {
            printf("unsupport\n");
        } else if (strcmp(argv[1], "sdrec") == 0) {

            int fatfs_en = 0;

            aos_kv_getint("fatfs_en", &fatfs_en);

            if (fatfs_en == 0) {
                printf("fatfs diabled, use \"kv setint fatfs_en 1\" to enable fatfs, and reboot\n");
                return;
            }

            if (argc == 3) {
                int act = atoi(argv[2]);

                if (act == 0) {
                    /* 结束录制 */
                    yv_pcm_enable(g_yv_handle, 0);
                    sdrec_call_close();
                } else {
                    /* 开始录制 */
                    sdrec_call_open();
                    yv_pcm_enable(g_yv_handle, 15);
                }
            }
        } else if (strcmp(argv[1], "print") == 0) {
#if 0
            if (argc == 3) {
                g_print_pcm_data = atoi(argv[2]);
                yv_pcm_enable(g_yv_handle, g_print_pcm_data?15:0);
            }
#endif
        } else if (strcmp(argv[1], "sdspd") == 0) {
            if (argc == 4) {
                sdrec_call_sdspd(atoi(argv[2]), atoi(argv[3]));
            }
        } else if (strcmp(argv[1], "pcm2mem") == 0) {
#if 0
            if (argc == 3) {
                g_pcm_save2mem =  atoi(argv[2]);
                if (g_pcm_save2mem) {
                    g_pcm_save2mem_pos = 0;
                    printf("pcm2mem start\n");
                } else {
                    printf("pcm2mem stop %d\n", g_pcm_save2mem_pos * 2);
                }
                yv_pcm_enable(g_yv_handle, g_pcm_save2mem?15:0);
            } else {
                printf("yv pcm2mem 0/1/2/3");
            }
#endif
        } else if (strcmp(argv[1], "play") == 0) {
            if (argc == 3) {
                yv_pcm_enable(g_yv_handle, 15);
                play_record(atoi(argv[2]));
            }
        } else {
            printf("yv pcm/asr/ref/mic/sdrec/print/play/pcm2mem\n");
        }
    } else {
        printf("yv pcm/asr/ref/mic/sdrec/print/play/pcm2mem\n");
#if 0
        extern codec_adrb_t g_sram_mic_ring;
		printf("rb_mic=%p(%p,%d,%d,%d)\n", &g_sram_mic_ring,
								g_sram_mic_ring.buffer, g_sram_mic_ring.length, g_sram_mic_ring.head, g_sram_mic_ring.tail);

        extern codec_adrb_t g_ref_sound_ring;
		printf("rb_ref=%p(%p,%d,%d,%d)\n", &g_ref_sound_ring,
								g_ref_sound_ring.buffer, g_ref_sound_ring.length, g_ref_sound_ring.head, g_ref_sound_ring.tail);
#endif
    }

}

static uint16_t get_pcm_max(uint8_t * buf, int size)
{
    int16_t *ptr = (int16_t *) buf;
    int count = size / 2;

    int16_t max_pcm = -32760;
    for (int i = 0; i < count; i++) {
        if (ptr[i] > max_pcm) {
            max_pcm = ptr[i];
        }
    }

    return max_pcm;
}
#if 0
static uint16_t get_pcm_min(uint8_t * buf, int size)
{
    int16_t *ptr = (int16_t *) buf;
    int count = size / 2;

    int16_t min_pcm = 32760;
    for (int i = 0; i < count; i++) {
        if (ptr[i] < min_pcm) {
            min_pcm = ptr[i];
        }
    }

    return min_pcm;
}

static void timed_print_data(int16_t mic1, int16_t mic2, int16_t ref1, int16_t ref2)
{
    static int16_t last_mic1 = -32760;
    static int16_t last_mic2 = -32760;
    static int16_t last_ref1 = -32760;
    static int16_t last_ref2 = -32760;

    static long long last_ms = 0;

    if (last_ms == 0) {
        last_ms = aos_now_ms();
        last_mic1 = mic1;
        last_mic2 = mic2;
        last_ref1 = ref1;
        last_ref2 = ref2;
    } else {
        if (mic1 > last_mic1) {
            last_mic1 = mic1;
        }
        if (mic2 > last_mic2) {
            last_mic2 = mic2;
        }
        if (ref1 > last_ref1) {
            last_ref1 = ref1;
        }
        if (ref2 > last_ref2) {
            last_ref2 = ref2;
        }
        long long now = aos_now_ms();
        if (now - last_ms > 250) {
            printf("?,%05d,%05d,%05d,%05d\n",last_mic1 * 10000 / 32768, last_mic2 * 10000 / 32768, last_ref1 * 10000 / 32768, last_ref2 * 10000 / 32768);
            last_ms = aos_now_ms();
            last_mic1 = -32760;
            last_mic2 = -32760;
            last_ref1 = -32760;
            last_ref2 = -32760;
        }
    }
}
#endif

#include "avutil/named_straightfifo.h"
static int g_play_record_type = -1;
static nsfifo_t *g_play_fifo = NULL;

static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

static void play_record(int type)
{
    int                reof = 0;
    nsfifo_t *fifo = NULL;

    fifo = nsfifo_open("fifo://tts/1", O_CREAT, 16000 / 1000 * 16 / 8 * 10 * 40);

    g_play_fifo = fifo;

    aui_player_stop(MEDIA_SYSTEM);
    aui_player_play(MEDIA_SYSTEM, "fifo://tts/1", 1);


    uint8_t wav_head[] = {0x52, 0x49, 0x46, 0x46, 0x66, 0xC3, 0x0E, 0x00, 0x57, 0x41, 0x56, 0x45,
                          0x66, 0x6D, 0x74, 0x20, 0x12, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
                          0x80, 0x3E, 0x00, 0x00, 0x00, 0x7D, 0x00, 0x00, 0x02, 0x00, 0x10, 0x00,
                          0x00, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0xA6, 0x0E, 0x00};

    char *             pos  = NULL;
    int len = nsfifo_get_wpos(fifo, &pos, 4000);
    if (len > sizeof(wav_head)) {
        memcpy(pos, wav_head, sizeof(wav_head));
        nsfifo_set_wpos(fifo, sizeof(wav_head));
    } else {
        nsfifo_close(fifo);
        return;
    }

    g_play_record_type = type;

    while(1) {
        reof = nsfifo_is_reof(fifo); /** peer read reach to end */
        if (reof) {
            break;
        }
        aos_msleep(500);
    }

    /* set write eof */
    nsfifo_set_eof(fifo, 0, 1);

    g_play_record_type = -1;

    while (0 == nsfifo_is_reof(fifo)) {
        aos_msleep(100); //wait peer drain fifo
    }
    nsfifo_close(fifo);

    yv_pcm_enable(g_yv_handle, 0);
}

static void pcm_to_fifo(void *data, int len)
{
    static uint8_t data_bak[640];
    static int data_bak_len = 0;

    int                reof = 0;
    char *             pos  = NULL;
    int                wlen  = 0;
    reof = nsfifo_is_reof(g_play_fifo); /** peer read reach to end */
    if (reof) { 
        return;
    }

    wlen = nsfifo_get_wpos(g_play_fifo, &pos, 4000);
    int pl_len = len / 5;
    uint8_t * all_data = (uint8_t * ) data;
    uint8_t * pl_data = (uint8_t * ) &all_data[pl_len * g_play_record_type];

    /* 检查备份数据，先写 */
    if (wlen < data_bak_len) {
        LOGE(TAG,"wait fifo write buf");
        return;
    }

    if(data_bak_len) {
        memcpy(pos, data_bak, data_bak_len);
        nsfifo_set_wpos(g_play_fifo, data_bak_len);
        data_bak_len = 0;

        /* 重新获取写位置,用于当前数据写入播放缓存 */
        wlen = nsfifo_get_wpos(g_play_fifo, &pos, 4000);
    }

    if (wlen >= pl_len) {
        /* 播放器缓存足够 */
        memcpy(pos, pl_data, pl_len);
        nsfifo_set_wpos(g_play_fifo, pl_len);
    } else {
        if (wlen > 0) {
            /* 播放器缓存不够，填充完剩余空间 */
            memcpy(pos, pl_data, wlen);
            nsfifo_set_wpos(g_play_fifo, wlen);

            /* 备份剩余数据 */
            data_bak_len = pl_len - wlen;
            memcpy(data_bak, &pl_data[wlen], data_bak_len);
        }
    }
}


/**
 * 数据格式说明：
 * 使用yv_pcm_enable(g_yv_handle, 15)获得的数据
 * 算法1： DSP会传输 32 * 20 * 5 = 3200 字节的数据 (单包640)
 * 算法2： DSP会传输 32 * 16 * 5 = 2560 字节的数据 (单包512)
 * 1ms 32字节，20ms传输一次，共5通道
*/
static void yv_event_cb(void *priv, mic_event_id_t evt_id, void *data, int len)
{
    switch (evt_id)
    {
        case MIC_EVENT_PCM_DATA:

            /* PCM转发到播放器 */
            if (g_play_record_type >= 0 && g_play_fifo) {
                pcm_to_fifo(data, len);
            }

            /* PCM录制到SD卡 */
            if (g_rec_fd > 0 /*&& g_pcm_mem_save2sd == 0*/) {
                /* SD卡连续录制 */
                uint8_t * rec_data = aos_malloc(len);
                int ret;
                memcpy(rec_data, data, len);
                ret = sdrec_call_write(rec_data, len);
                if (ret < 0) {
                    aos_free(rec_data);
                    LOGE(TAG, "sdrec_call_write");
                }
            }
#if 0
            /* 打印输出通道数据 */
            if (g_print_pcm_data && len >= 1280) {
                uint8_t * ptr = data;
                int pk_len = len / 5;
                int16_t mic1 = get_pcm_max(ptr, pk_len);
                int16_t mic2 = get_pcm_max(&ptr[pk_len * 1], pk_len);
                int16_t ref1 = get_pcm_max(&ptr[pk_len * 2], pk_len);
                int16_t ref2 = get_pcm_max(&ptr[pk_len * 3], pk_len);
                timed_print_data(mic1, mic2, ref1, ref2);
            }

            /* PCM 录制到内存 */
            if (g_pcm_save2mem) {
                pcm_to_mem(data, len, g_pcm_save2mem);
            }
#endif
            break;
        case MIC_EVENT_SESSION_START:
            LOGI(TAG, "MIC_EVENT_SESSION_START %d", *(int *)data);

#if 0
        case YV_ASR_EVT:
            LOGI(TAG, "YV_ASR_EVT %d", *(int *)data);
            if (g_wake_record_en) {
                sdrec_call_mem();
            }
            break;
        case YV_SILENCE_EVT:
            LOGI(TAG, "YV_SILENCE_EVT");
            break;
        case YV_VAD_EVT:
            //LOGI(TAG, "YV_VAD_EVT %d", *(int *)data);
            break;
#endif
        default:
            break;
    }
}

void cli_reg_cmd_yvdbg(void)
{
    static const struct cli_command cmd_info = {"yv", "yv debug", cmd_cxc_func};

    aos_cli_register_command(&cmd_info);

    int fatfs_en = 0;

    aos_kv_getint("fatfs_en", &fatfs_en);

    if (fatfs_en == 0) {
        //LOGI(TAG, "fatfs diabled, use \"kv setint fatfs_en 1\" to enable fatfs, and reboot");
        return;
    }

    sd_record_usrv_init();

}

