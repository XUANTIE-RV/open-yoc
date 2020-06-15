#ifndef _AUDIO_RES_H_
#define _AUDIO_RES_H_
typedef enum {
    LOCAL_AUDIO_HELLO,
    LOCAL_AUDIO_NET_FAIL,
    LOCAL_AUDIO_NET_SUCC,
    LOCAL_AUDIO_OK,
    LOCAL_AUDIO_SORRY2,
    LOCAL_AUDIO_SORRY,
    LOCAL_AUDIO_STARTING,
} local_audio_name_t;
int local_audio_play(local_audio_name_t name);
extern const unsigned char local_audio_hello[864];
extern const unsigned char local_audio_net_fail[3456];
extern const unsigned char local_audio_net_succ[2880];
extern const unsigned char local_audio_ok[1224];
extern const unsigned char local_audio_sorry2[3600];
extern const unsigned char local_audio_sorry[5904];
extern const unsigned char local_audio_starting[2016];
#define AUDIO_RES_ARRAY \
{local_audio_hello,sizeof(local_audio_hello)}, \
{local_audio_net_fail,sizeof(local_audio_net_fail)}, \
{local_audio_net_succ,sizeof(local_audio_net_succ)}, \
{local_audio_ok,sizeof(local_audio_ok)}, \
{local_audio_sorry2,sizeof(local_audio_sorry2)}, \
{local_audio_sorry,sizeof(local_audio_sorry)}, \
{local_audio_starting,sizeof(local_audio_starting)}, \
{NULL,0}
#endif
