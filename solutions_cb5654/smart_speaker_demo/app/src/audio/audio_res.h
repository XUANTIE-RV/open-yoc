#ifndef _AUDIO_RES_H_
#define _AUDIO_RES_H_
typedef enum {
LOCAL_AUDIO_HELLO,
LOCAL_AUDIO_NET_CFG_CONFIG,
LOCAL_AUDIO_NET_CFG_CONN,
LOCAL_AUDIO_NET_CFG_FAIL,
LOCAL_AUDIO_NET_CFG_START,
LOCAL_AUDIO_NET_CFG_SWITCH,
LOCAL_AUDIO_NET_CFG_TIMEOUT,
LOCAL_AUDIO_NET_FAIL,
LOCAL_AUDIO_NET_SUCC,
LOCAL_AUDIO_OK,
LOCAL_AUDIO_PLAY_ERR,
LOCAL_AUDIO_SORRY,
LOCAL_AUDIO_SORRY2,
LOCAL_AUDIO_STARTING,
LOCAL_AUDIO_END
} local_audio_name_t;
int local_audio_play(local_audio_name_t name);
extern const unsigned char local_audio_hello[6048];
extern const unsigned char local_audio_net_cfg_config[8541];
extern const unsigned char local_audio_net_cfg_conn[7677];
extern const unsigned char local_audio_net_cfg_fail[3033];
extern const unsigned char local_audio_net_cfg_start[5733];
extern const unsigned char local_audio_net_cfg_switch[8757];
extern const unsigned char local_audio_net_cfg_timeout[3681];
extern const unsigned char local_audio_net_fail[5733];
extern const unsigned char local_audio_net_succ[4869];
extern const unsigned char local_audio_ok[2385];
extern const unsigned char local_audio_play_err[3573];
extern const unsigned char local_audio_sorry[9405];
extern const unsigned char local_audio_sorry2[5949];
extern const unsigned char local_audio_starting[3573];
#define AUDIO_RES_ARRAY \
{local_audio_hello,sizeof(local_audio_hello)}, \
{local_audio_net_cfg_config,sizeof(local_audio_net_cfg_config)}, \
{local_audio_net_cfg_conn,sizeof(local_audio_net_cfg_conn)}, \
{local_audio_net_cfg_fail,sizeof(local_audio_net_cfg_fail)}, \
{local_audio_net_cfg_start,sizeof(local_audio_net_cfg_start)}, \
{local_audio_net_cfg_switch,sizeof(local_audio_net_cfg_switch)}, \
{local_audio_net_cfg_timeout,sizeof(local_audio_net_cfg_timeout)}, \
{local_audio_net_fail,sizeof(local_audio_net_fail)}, \
{local_audio_net_succ,sizeof(local_audio_net_succ)}, \
{local_audio_ok,sizeof(local_audio_ok)}, \
{local_audio_play_err,sizeof(local_audio_play_err)}, \
{local_audio_sorry,sizeof(local_audio_sorry)}, \
{local_audio_sorry2,sizeof(local_audio_sorry2)}, \
{local_audio_starting,sizeof(local_audio_starting)}, \
{NULL,0}
#endif
