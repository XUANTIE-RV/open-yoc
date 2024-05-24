#include <stdio.h>
#include <aos/kernel.h>
#include <alsa/pcm.h>
#include "cvi_type.h"
#include "platform.h"
#include "audio.h"
#include "cvi_comm_aio.h"
#include "cviaudio_algo_interface.h"
#include "cv181x_adc_dac.h"

static aos_pcm_t *capture_handle,*playback_handle;
static unsigned int rate = 8000;       //ai ao default 16K
void *pssp_handle = NULL;

static void audio_capture_init(void)
{
	aos_pcm_hw_params_t *capture_hw_params;

	//init capture(i2s0). clk path: i2s3(master) -> internal_codec -> i2s0(slave)
	int dir = 1;
	aos_pcm_open (&capture_handle, "pcmC0", AOS_PCM_STREAM_CAPTURE, 0);//打开设备“pcmC0
	aos_pcm_hw_params_alloca (&capture_hw_params);//申请硬件参数内存空间
	aos_pcm_hw_params_any (capture_handle, capture_hw_params);//初始化硬件参数
	capture_hw_params->period_size = PERIOD_FRAMES_SIZE;
	capture_hw_params->buffer_size = PERIOD_FRAMES_SIZE*4;
	aos_pcm_hw_params_set_access (capture_handle, capture_hw_params, AOS_PCM_ACCESS_RW_NONINTERLEAVED);// 设置音频数据参数为非交错模式
	aos_pcm_hw_params_set_format (capture_handle, capture_hw_params, 16);//设置音频数据参数为小端16bit
	aos_pcm_hw_params_set_rate_near (capture_handle, capture_hw_params, &rate, &dir);//设置音频数据参数采样率为16K
	aos_pcm_hw_params_set_channels (capture_handle, capture_hw_params, 2);//设置音频数据参数为2通道
	aos_pcm_hw_params (capture_handle, capture_hw_params);//设置硬件参数到具体硬件中
//	byte_len = aos_pcm_frames_to_bytes(capture_handle, PERIOD_FRAMES_SIZE);
}

#if(ENABLE_AUDALGO)
static void audio_algo_init(void)
{
	char pstrVersion[52];

    AI_TALKVQE_CONFIG_S stVqeConfig;
    AI_TALKVQE_CONFIG_S *pstVqeConfig = &stVqeConfig;

    pstVqeConfig->para_client_config = 0;
    pstVqeConfig->u32OpenMask = AI_TALKVQE_MASK_AEC | AI_TALKVQE_MASK_AGC | AI_TALKVQE_MASK_ANR;
    pstVqeConfig->s32WorkSampleRate = rate;
    pstVqeConfig->s32RevMask = 0;
    pstVqeConfig->para_notch_freq = 0;
    /* AEC */
    pstVqeConfig->stAecCfg.para_aec_filter_len = 13;
    pstVqeConfig->stAecCfg.para_aes_std_thrd = 37;
    pstVqeConfig->stAecCfg.para_aes_supp_coeff = 60;
    /* ANR */
    pstVqeConfig->stAnrCfg.para_nr_snr_coeff = 15;
    pstVqeConfig->stAnrCfg.para_nr_init_sile_time = 0;

    /* AGC */
    pstVqeConfig->stAgcCfg.para_agc_max_gain = 0;
    pstVqeConfig->stAgcCfg.para_agc_target_high = 2;
    pstVqeConfig->stAgcCfg.para_agc_target_low = 72;
    pstVqeConfig->stAgcCfg.para_agc_vad_ena = 0;

    pstVqeConfig->stAecDelayCfg.para_aec_init_filter_len = 2;
    pstVqeConfig->stAecDelayCfg.para_dg_target = 1;
    pstVqeConfig->stAecDelayCfg.para_delay_sample = 1;

    CviAud_Algo_GetVersion(pstrVersion);
    printf("[cvi3aVersion:%s]\n", pstrVersion);

    pssp_handle = CviAud_Algo_Init(pstVqeConfig->u32OpenMask, pstVqeConfig);
    if (!pssp_handle) {
        printf("ssp init fail\n");
    }

}
#endif

static void audio_play_init(void)
{
	aos_pcm_hw_params_t *playback_hw_params;
	int dir = 0;
	aos_pcm_open (&playback_handle, "pcmP0", AOS_PCM_STREAM_PLAYBACK, 0); //打开设备“pcmP0”
	aos_pcm_hw_params_alloca(&playback_hw_params); //申请硬件参数内存空间
	aos_pcm_hw_params_any(playback_handle, playback_hw_params); //初始化硬件参数
	playback_hw_params->period_size = PERIOD_FRAMES_SIZE;
	playback_hw_params->buffer_size = PERIOD_FRAMES_SIZE*4;
	aos_pcm_hw_params_set_access(playback_handle, playback_hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED); // 设置音频数据参数为交错模式
	aos_pcm_hw_params_set_format(playback_handle, playback_hw_params, 16); //设置音频数据参数为小端16bit
	aos_pcm_hw_params_set_rate_near(playback_handle, playback_hw_params, &rate, &dir); //设置音频数据参数采样率为16K
	aos_pcm_hw_params_set_channels(playback_handle, playback_hw_params, 1); //设置音频数据参数为1通道
	aos_pcm_hw_params(playback_handle, playback_hw_params); //设置硬件参数到具体硬件?
}

int audio_pcm_read(unsigned char *buf)
{
    int ret = 0;
    ret = aos_pcm_readn(capture_handle, (void **)buf, PERIOD_FRAMES_SIZE);//接收非交错音频数
    if (ret > 0) {
        ret = aos_pcm_frames_to_bytes(capture_handle, ret);
    }
    return ret;
}

int audio_pcm_write(const unsigned char *buf,int len)
{
    int ret = 0;
    ret = aos_pcm_writei(playback_handle,buf,
            aos_pcm_bytes_to_frames(playback_handle,len));//发送交错音频数据
    return ret;
}


int audio_get_pcm_len(int type)
{
    if(type == 0)
        return aos_pcm_frames_to_bytes(capture_handle, PERIOD_FRAMES_SIZE);
    else
        return aos_pcm_frames_to_bytes(playback_handle, PERIOD_FRAMES_SIZE);
}

#define AUDIO_ADCL_VOL	15
#define AUDIO_ADCR_VOL	3


// void audio_set_vol(void)
// {
// 	u32 adcl_vol;
// 	u32 adcr_vol;

// 	adcl_vol = AUDIO_ADCL_VOL;
// 	cv182xadc_ioctl(ACODEC_SET_ADCL_VOL, (u64)&adcl_vol);

// 	adcr_vol = AUDIO_ADCR_VOL;
// 	cv182xadc_ioctl(ACODEC_SET_ADCR_VOL, (u64)&adcr_vol);
// }

int media_audio_init(void)
{
    audio_capture_init();
#if(ENABLE_AUDALGO)
	audio_algo_init();
#endif
    audio_play_init();
    PLATFORM_SpkMute(1);
	//audio_set_vol();
    printf("alios media audio init success\r\n");
    return 0;
}

int media_audio_deinit(void)
{
    aos_pcm_close(playback_handle); //关闭设备
#if(ENABLE_AUDALGO)
	CviAud_Algo_DeInit(pssp_handle);
#endif
	aos_pcm_close(capture_handle); //关闭设备
    PLATFORM_SpkMute(0);
    printf("alios audio deinit success\r\n");
    return 0;
}

