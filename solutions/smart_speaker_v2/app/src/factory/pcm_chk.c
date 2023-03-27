#include <stdint.h>
#include <smart_audio.h>

#include <aos/aos.h>
#include <sys/stat.h>
#include <vfs.h>
#include <dirent.h>

#include <uservice/eventid.h>
#include <yoc/adb_server.h>

/* 导入录音接口 */
extern int  rec_start_mem(const char *url, int rb_size, int chncnt, uint8_t *mem, int size);
extern void rec_stop(void);
extern void play_sintest(int sec);

/* 定时停止录音事件 */
#define EVENT_FCT_REC_STOP (EVENT_USER + 1000)

/* 录音数据 */
static uint8_t *g_mem_record = NULL;
static int      g_mem_size   = 0;
static int      g_pcmchk     = 0;
static int      g_savefile   = 0;

/**
 * @brief 获取采样数据中一定数量的最大值
 * @param chid 通道id 0~2
 * @param mem 采样数据，三通道交织数据
 * @param byte 采样数据字节数
 * @param start_point 忽略点数
 * @param check_count 检查点数
 */
typedef struct __attribute__ ((packed)) _pcm_3ch {
    int16_t samples[3];
} pcm_3ch_t;

static int16_t pcm_get_max(uint8_t chid, uint8_t *mem, int byte, int start_point, int check_count)
{
    pcm_3ch_t *pcm                    = (pcm_3ch_t *)mem;
    int        single_ch_sample_count = byte / 3; //单通道点数

    if (mem == NULL || byte <=0 || chid > 2) {
        return 0;
    }

    if (start_point + check_count > single_ch_sample_count) {
        return 0;
    }

    int16_t max = 0;
    for (int i = start_point; i < start_point + check_count; i++) {
        if (pcm[i].samples[chid] > max) {
            max = pcm[i].samples[chid];
        }
    }

    return max;
}

static int pcm_valid_chk()
{
    if (g_mem_record == NULL || g_mem_size == 0) {
        return -1;
    }

    /* 录制到的是交织格式数据 */
    int16_t max0 = pcm_get_max(0, g_mem_record, g_mem_size, 16000, 16000);
    int16_t max1 = pcm_get_max(1, g_mem_record, g_mem_size, 16000, 16000);
    int16_t max2 = pcm_get_max(2, g_mem_record, g_mem_size, 16000, 16000);

    adbserver_send("max=[%d %d %d]\r\n", max0, max1, max2);

    /* 最小幅值判断 */
    if (max0 < 5000 || max1 < 5000 || max2 < 5000) {
        return -1;
    }

    /* 最大幅值判断 */
    if (max0 > 31000 || max1 > 31000 || max2 > 31000) {
        return -1;
    }

    /* MIC 的差异判断 */
    if (abs(max0 - max1) > 1000) {
        return -1;
    }

    return 0;
}

static int pcm_chk_save(char *file)
{
    int fd = aos_open(file, (O_WRONLY | O_CREAT | O_TRUNC));
    if (fd > 0) {
        int ret = aos_write(fd, g_mem_record, g_mem_size);
        aos_close(fd);
        if (ret < 0) { /* aos_write 返回值是实际写入的字节数，< 0：失败 */
            adbserver_send("aos_write failed, try to reduce the time\r\n");
            aos_unlink(file);
            return -1;
        }
        return 0;
    }
    return -1;
}

/* 录音停止事件 */
static void rec_stop_event(uint32_t event_id, const void *data, void *context)
{
    /* 停止录音 */
    rec_stop();

    /* 数据检查 */
    if (g_pcmchk) {
        adbserver_send("pcm chk start\r\n");
        if (pcm_valid_chk() == 0) {
            adbserver_send("pcm chk success\r\n");
        } else {
            adbserver_send("pcm chk failed\r\n");
        }
    }

    /* 保存文件 */
    if (g_savefile) {
        adbserver_send("save file start\r\n");
        if (pcm_chk_save("/rec_3ch.bin") == 0) {
            adbserver_send("save file success\r\n");
        } else {
            adbserver_send("save file failed\r\n");
        }
    }

    /* 释放内存 */
    aos_free(g_mem_record);
    g_mem_record = NULL;
    g_mem_size   = 0;
}

void fct_pcm_chk_record(char *play_url, int second, int vol, int pcmchk, int savefile)
{
    adbserver_send("pcm chk start\r\n");
    g_pcmchk   = pcmchk;
    g_savefile = savefile;

    event_subscribe(EVENT_FCT_REC_STOP, rec_stop_event, NULL);
    event_publish_delay(EVENT_FCT_REC_STOP, NULL, (second + 1) * 1000);

    /* 启动录音 */
    g_mem_size   = second * 1024 * 100; /*3路pcm，100K/s 计算*/
    g_mem_record = aos_zalloc_check(g_mem_size);
    rec_start_mem("mem://", 100 * 1024, 3, g_mem_record, g_mem_size);

    /* 播放音频 */
    if (vol >= 0) {
        smtaudio_vol_set(vol);
    }
    if (strcmp(play_url, "sintest://") == 0) {
        play_sintest(second);
    } else if (strcmp(play_url, "none://") == 0) {
        ;
    } else {
        smtaudio_start(MEDIA_MUSIC, play_url, 0, 0);
    }

    /* tydb 交互需要同步接口 */
    while (g_mem_size != 0) {
        aos_msleep(200);
    }
}
