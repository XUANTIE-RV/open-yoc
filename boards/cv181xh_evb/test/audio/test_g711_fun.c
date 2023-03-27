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
#include "g711_table.h"
#include "g711.h"

#define PERIOD_FRAMES_SIZE   (320)
#define CHAR_PER_SAMPLE     (1)
#define SHORT_PER_SAMPLE     (2)

enum _e_g711_tp {
	TP_ALAW,	//G711A
	TP_ULAW		//G711U
};

static int audio_debug_g711_cmd(int argc, char *argv[])
{
    int fdin = -1;
    int fdout = -1;
    int channel = 2;
    int sample_rate = 8000;
    int inFrameByte = 0;
    int outFrameByte = 0;
    int s32Readbyte = 0;
    int g711Type = 0;
    char *datain;
    char *dataout;
    int enorDe;

    printf("***********************************************\n"
           "***Audio G711 TEST[internal test] ***\n"
           "usage	: <input.raw> <output.raw> <sample rate> <channel num> <g711 type> <encode or decode>\n"
           "sample rate, only suport 8K\n"
           "g711 type, 0:g711a, 1:g711u\n"
           "encode or decode, 0:encode, 1:decode\n"
           "***********************************************\n");
    printf("***********************************************\n"
           "***For example ***\n"
           "encode g711a: audio_test_g711 /mnt/sd/QA_8k.qa_pcm  /mnt/sd/QA_8k.qa_pcm.g711a 8000 2 0 0\n"
           "encode g711u: audio_test_g711 /mnt/sd/QA_8k.qa_pcm"  "/mnt/sd/QA_8k.qa_pcm.g711u 8000 2 1 0\n"
           "decode g711a: audio_test_g711 /mnt/sd/QA_8k.qa_pcm.g711a"  "/mnt/sd/QA_8k.qa_pcm.g711a.raw 8000 2 0 1\n"
           "decode g711u: audio_test_g711 /mnt/sd/QA_8k.qa_pcm.g711u"  "/mnt/sd/QA_8k.qa_pcm.g711u.raw 8000 2 1 1\n"
           "***********************************************\n");
    if (argc < 7) {
        printf("[Error]Please check the usage\n");
        printf("[Error]Input argument is not enough!!!\n");
        return -1;
    }

    sample_rate = atoi(argv[3]);
    if (sample_rate != 8000) {
        sample_rate = 8000;
        printf("sample_rate error,only support:%d.\n", sample_rate);
        return -1;
    }

    channel = atoi(argv[4]);
    g711Type = atoi(argv[5]);
    enorDe = atoi(argv[6]); //0, encode, 1, decode

    fdin = aos_open(argv[1], O_RDONLY);
    fdout = aos_open(argv[2], O_CREAT | O_TRUNC | O_RDWR);
    if (fdin <= 0 || fdout <= 0) {
        printf("can't open file [%d, %d] \n", fdin, fdout);
        return -1;
    }

    printf("sample_rate=%d, channel=%d, g711Type=%d, enorDe=%d\n", sample_rate, channel, g711Type, enorDe);
    if (enorDe == 0) {
        printf("encode g711: ");
        inFrameByte = PERIOD_FRAMES_SIZE * channel * SHORT_PER_SAMPLE;
        outFrameByte = PERIOD_FRAMES_SIZE * channel * CHAR_PER_SAMPLE;
        datain = malloc(inFrameByte);
        dataout = malloc(outFrameByte);
        pcm16_alaw_tableinit();
        pcm16_ulaw_tableinit();
    } else if (enorDe == 1) {
        printf("decode g711: ");
        inFrameByte = PERIOD_FRAMES_SIZE * channel * CHAR_PER_SAMPLE;
        outFrameByte = PERIOD_FRAMES_SIZE * channel * SHORT_PER_SAMPLE;
        datain = malloc(inFrameByte);
        dataout = malloc(outFrameByte);
        alaw_pcm16_tableinit();
        ulaw_pcm16_tableinit();
    } else {
        printf("encode or decode type wrong\n");
        return -1;
    }
    if (g711Type == TP_ALAW) {
        printf("alwa \n");
    } else if (g711Type == TP_ULAW) {
        printf("ulwa \n");
    } else {
        printf("not suppor type \n");
        return -1;
    }

    printf("process start\n");
    while (1) {
        s32Readbyte = aos_read(fdin, datain, inFrameByte);
        if (s32Readbyte == 0) {
            break;
        }
        if (enorDe == 0) {//encode
            if (g711Type == TP_ALAW) {
                pcm16_to_alaw(inFrameByte, datain, dataout);
            } else if (g711Type == TP_ULAW) {
                pcm16_to_ulaw(inFrameByte, datain, dataout);
            }
        } else if (enorDe == 1) {//decode
            if (g711Type == TP_ALAW) {
                alaw_to_pcm16(inFrameByte, datain, dataout);
            } else if (g711Type == TP_ULAW) {
                ulaw_to_pcm16(inFrameByte, datain, dataout);
            }
        }
        aos_write(fdout, dataout, outFrameByte);

    }

    free(datain);
    free(dataout);

    aos_close(fdout);
    aos_close(fdin);
    printf("process finish\n");
    return 0;
}


ALIOS_CLI_CMD_REGISTER(audio_debug_g711_cmd, audio_test_g711, audio g711 test);

