#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <posix/timer.h>
#include <alsa/pcm.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "board.h"
#include "aos/cli.h"
#include "vfs.h"
#include "cvi_comm_aio.h"
#include "cviaudio_algo_interface.h"


#define CVIAUDIO_AEC_LENGTH 160
#define CVIAUDIO_PER_SAMPLE 2

typedef struct _BUF_INFO {
    short *mic_in;
    short *ref_in;
    short *datain;
    short *dataout;
} BUF_INFO;



static int audio_debug_3a_cmd(int argc, char *argv[])
{
    char pstrVersion[52];
    int fdin = -1;
    int fdout = -1;
    void *pssp_handle = NULL;
    int channel = 2;/* only support 2chn*/
    int sample_rate = 8000;
    int AecLenByte = 0;
    int s32Readbyte = 0;
    int RetframeLen = 0;
    BUF_INFO stbufinfo;

    printf("***********************************************\n"
           "***Audio AEC TEST[internal test] ***\n"
           "usage	: <input.raw> <output.raw> <sample rate>\n"
           "***********************************************\n");
    if (argc < 4) {
        printf("[Error]Please check the usage\n");
        printf("[Error]Input argument is not enough!!!\n");
        return -1;
    }

    sample_rate = atoi(argv[3]);
    if (sample_rate != 8000 && sample_rate != 16000) {
        printf("sample_rate error,not support:%d.\n", sample_rate);
        return -1;
    }


    AI_TALKVQE_CONFIG_S stVqeConfig;
    AI_TALKVQE_CONFIG_S *pstVqeConfig = &stVqeConfig;

    pstVqeConfig->para_client_config = 0;
    pstVqeConfig->u32OpenMask = AI_TALKVQE_MASK_AEC | AI_TALKVQE_MASK_AGC | AI_TALKVQE_MASK_ANR;
    pstVqeConfig->s32WorkSampleRate = sample_rate;
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

    fdin = aos_open(argv[1], O_RDONLY);
    fdout = aos_open(argv[2], O_CREAT | O_TRUNC | O_RDWR);
    if (fdin <= 0 || fdout <= 0) {
        printf("can't open file [%d, %d] \n", fdin, fdout);
        return -1;
    }

    CviAud_Algo_GetVersion(pstrVersion);
    printf("[cvi3aVersion:%s]\n", pstrVersion);

    pssp_handle = CviAud_Algo_Init(pstVqeConfig->u32OpenMask, pstVqeConfig);
    if (!pssp_handle) {
        printf("ssp init fail\n");
        goto ERROR;
    }

    AecLenByte = CVIAUDIO_AEC_LENGTH * channel * CVIAUDIO_PER_SAMPLE;
    stbufinfo.mic_in = malloc(AecLenByte);
    stbufinfo.ref_in = malloc(AecLenByte);
    stbufinfo.datain = malloc(AecLenByte);
    stbufinfo.dataout = malloc(AecLenByte);

    while (1) {
        s32Readbyte = aos_read(fdin, stbufinfo.datain, AecLenByte);
        if (s32Readbyte == 0) {
            break;
        }

        for (int i = 0; i < CVIAUDIO_AEC_LENGTH; i++) {
            stbufinfo.mic_in[i] = stbufinfo.datain[i * 2];
            stbufinfo.ref_in[i] = stbufinfo.datain[i * 2 + 1];
        }

        if ((pstVqeConfig->u32OpenMask & AI_TALKVQE_MASK_AEC) == AI_TALKVQE_MASK_AEC) {//aec
            RetframeLen = CviAud_Algo_Process(pssp_handle, stbufinfo.mic_in, stbufinfo.ref_in, stbufinfo.dataout, CVIAUDIO_AEC_LENGTH);
            if (RetframeLen != CVIAUDIO_AEC_LENGTH) {
                printf("[aec] ssp process fail\n");
                return -1;
            }

        } else {
            RetframeLen = CviAud_Algo_Process(pssp_handle, stbufinfo.mic_in, NULL, stbufinfo.dataout, CVIAUDIO_AEC_LENGTH);
            if (RetframeLen != CVIAUDIO_AEC_LENGTH) {
                printf("[anr agc] ssp process fail\n");
                return -1;
            }
        }

        aos_write(fdout, stbufinfo.dataout, CVIAUDIO_AEC_LENGTH * CVIAUDIO_PER_SAMPLE);

    }

    CviAud_Algo_DeInit(pssp_handle);
    free(stbufinfo.mic_in);
    free(stbufinfo.ref_in);
    free(stbufinfo.datain);
    free(stbufinfo.dataout);
ERROR:
    aos_close(fdout);
    aos_close(fdin);

    return 0;
}


ALIOS_CLI_CMD_REGISTER(audio_debug_3a_cmd, audio_test_3a, audio vqe test);

