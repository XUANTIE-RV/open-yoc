#ifndef _AUDIO_H_
#define _AUDIO_H_


#define CAPTURE_SIZE 4096
#define PERIOD_FRAMES_SIZE 160

int audio_pcm_read(unsigned char *buf);
int audio_pcm_write(const unsigned char *buf,int len);
int audio_get_pcm_len(int type);
int media_audio_init(void);
int media_audio_deinit(void);


#endif
