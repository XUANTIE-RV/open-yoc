#ifndef _AV_AUDIO_H_
#define _AV_AUDIO_H_

#define ENABLE_AUDALGO 1
#define CAPTURE_SIZE 4096
#define PERIOD_FRAMES_SIZE 160

int audio_pcm_read(unsigned char *buf);
int audio_pcm_write(const unsigned char *buf,int len);
int audio_get_pcm_len(int type);
int media_audio_init(void);
int media_audio_deinit(void);


#endif
