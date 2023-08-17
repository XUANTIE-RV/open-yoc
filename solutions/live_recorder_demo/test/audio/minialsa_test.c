#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <posix/timer.h>
#include <drv/i2s.h>
#include <alsa/pcm.h>
//#include <aos_pcm.h>
#include "board.h"
#include "dirent.h"
#include "dw_i2s.h"
#include "fatfs_vfs.h"
#include "platform.h"

static int capture_running, play_running;
static aos_pcm_t *capture_handle;
static aos_pcm_t *playback_handle;

#define CAPTURE_SIZE 4096
#define PERIOD_FRAMES_SIZE 320

struct timespec start, end;
unsigned int rate = 16000;
int dir = 1;
int byte_len = 0;


unsigned int play_cnt = 0;
unsigned int capture_cnt = 0;

int play_state = 0;
int capture_state = 0;


static void audio_capture_test_entry(void *arg)
{
	int ret;
	char save_name[128] = SD_FATFS_MOUNTPOINT"/16k2ch_1.pcm";
	unsigned char *cap_buf;
	aos_pcm_hw_params_t *capture_hw_params;
	int fd;
	cap_buf = aos_malloc(CAPTURE_SIZE);
	fd = aos_open(save_name, O_CREAT | O_TRUNC | O_RDWR);
	if(fd <= 0) {
		printf("file %s open error\n", save_name);
	} else {
		printf("create pcm file:%s\n", save_name);
	}
	capture_state = 1;
	//init capture(i2s0). clk path: i2s3(master) -> internal_codec -> i2s0(slave)
	dir = 1;
	aos_pcm_open (&capture_handle, "pcmC0", AOS_PCM_STREAM_CAPTURE, 0);//打开设备“pcmC0
	aos_pcm_hw_params_alloca (&capture_hw_params);//申请硬件参数内存空间
	aos_pcm_hw_params_any (capture_handle, capture_hw_params);//初始化硬件参数
	capture_hw_params->period_size = PERIOD_FRAMES_SIZE;
	capture_hw_params->buffer_size = PERIOD_FRAMES_SIZE*4;
	aos_pcm_hw_params_set_access (capture_handle, capture_hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED);// 设置音频数据参数为交错模式
	aos_pcm_hw_params_set_format (capture_handle, capture_hw_params, 16);//设置音频数据参数为小端16bit
	aos_pcm_hw_params_set_rate_near (capture_handle, capture_hw_params, &rate, &dir);//设置音频数据参数采样率为16K
	aos_pcm_hw_params_set_channels (capture_handle, capture_hw_params, 2);//设置音频数据参数为2通道
	aos_pcm_hw_params (capture_handle, capture_hw_params);//设置硬件参数到具体硬件中
	byte_len = aos_pcm_frames_to_bytes(capture_handle, PERIOD_FRAMES_SIZE);
	capture_state = 2;
	while(capture_running) {
		capture_state = 3;
		ret = aos_pcm_readi(capture_handle, cap_buf, PERIOD_FRAMES_SIZE);//接收交错音频数据
		capture_cnt++;
		capture_state = 4;
		aos_write(fd, cap_buf, aos_pcm_frames_to_bytes(capture_handle, ret));
	}
	capture_state = 5;
	aos_close(fd);
	aos_free(cap_buf);
	aos_pcm_close(capture_handle); //关闭设备
	capture_handle = NULL;
	capture_state = 6;
}

static void audio_play_test_entry(void *arg)
{
	char save_name[128] = SD_FATFS_MOUNTPOINT"/16k2ch.pcm";
	unsigned char *cap_buf;
	aos_pcm_hw_params_t *playback_hw_params;
	int ret;
	int fd;
	int dir;
	fd = aos_open(save_name, O_RDONLY);
	if(fd <= 0) {
		printf("file %s open error\n", save_name);
	} else {
		printf("aos_open pcm file:%s\n", save_name);
	}
	dir = 0;
	play_state = 1;
	cap_buf = aos_malloc(CAPTURE_SIZE);
	aos_pcm_open (&playback_handle, "pcmP0", AOS_PCM_STREAM_PLAYBACK, 0); //打开设备“pcmP0”
	aos_pcm_hw_params_alloca(&playback_hw_params); //申请硬件参数内存空间
	aos_pcm_hw_params_any(playback_handle, playback_hw_params); //初始化硬件参数
	playback_hw_params->period_size = PERIOD_FRAMES_SIZE;
	playback_hw_params->buffer_size = PERIOD_FRAMES_SIZE*4;
	aos_pcm_hw_params_set_access(playback_handle, playback_hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED); // 设置音频数据参数为交错模式
	aos_pcm_hw_params_set_format(playback_handle, playback_hw_params, 16); //设置音频数据参数为小端16bit
	aos_pcm_hw_params_set_rate_near(playback_handle, playback_hw_params, &rate, &dir); //设置音频数据参数采样率为16K
	aos_pcm_hw_params_set_channels(playback_handle, playback_hw_params, 2); //设置音频数据参数为2通道
	aos_pcm_hw_params(playback_handle, playback_hw_params); //设置硬件参数到具体硬件?
	play_state = 2;
	while(play_running) {
		play_state = 3;
		ret = aos_read(fd, cap_buf, aos_pcm_frames_to_bytes(playback_handle, PERIOD_FRAMES_SIZE));
		if(ret <= 0) {
			aos_lseek(fd, 0, SEEK_SET);
			continue;
		}
		play_state = 4;
		aos_pcm_writei(playback_handle,cap_buf, aos_pcm_bytes_to_frames(playback_handle,ret));//发送交错音频数据
		play_cnt++;
	}
	play_state = 5;
	aos_pcm_close(playback_handle); //关闭设备
	playback_handle = NULL;
	aos_close(fd);
	aos_free(cap_buf);
	play_state = 6;
}


void show_debug_info(void)
{
	printf("capture_state = %d\n",capture_state);
	printf("capture_cnt = %d\n",capture_cnt);
	printf("play_state = %d\n",play_state);
	printf("play_cnt = %d\n",play_cnt);
}

void audio_debug_capture_cmd(int32_t argc, char **argv)
{
	if(argc == 2) {
		if(strcmp(argv[1], "1") == 0) {
			if(capture_running) {
				printf("capture device is busy.\n");
				return;
			}
			capture_running = 1;
			aos_task_new("audio_capture_test", audio_capture_test_entry, NULL, 4096);
			return;
		} else if(strcmp(argv[1], "0") == 0) {
			capture_running = 0;
			return;
		}
	}
	printf("invalid cmd params.\n");
	printf("usage:%s 1/0\n",argv[0]);
}

void audio_debug_play_cmd(int32_t argc, char **argv)
{
	if(argc == 2) {
		if(strcmp(argv[1], "1") == 0) {
			if(play_running) {
				printf("play device is busy.\n");
				return;
			}
			play_running = 1;
			PLATFORM_SpkMute(1);
			aos_task_new("audio_play_test", audio_play_test_entry, NULL, 4096);
			return;
		} else if(strcmp(argv[1], "0") == 0) {
			PLATFORM_SpkMute(0);
			play_running = 0;
			return;
		}
	}
	printf("invalid cmd params.\n");
	printf("usage:%s 1/0\n",argv[0]);
}

void audio_debug_show_info(int32_t argc, char **argv)
{
	show_debug_info();
}

ALIOS_CLI_CMD_REGISTER(audio_debug_play_cmd, aplay, play audio);
ALIOS_CLI_CMD_REGISTER(audio_debug_capture_cmd, arecord, capture audio);
ALIOS_CLI_CMD_REGISTER(audio_debug_show_info, audio_info, show audio debug info);
