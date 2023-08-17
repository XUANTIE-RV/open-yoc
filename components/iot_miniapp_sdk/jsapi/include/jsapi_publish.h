#ifndef JSAPIPUBLISH_H_
#define JSAPIPUBLISH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cJSON.h>

/* init main event */
void jsapi_miniapp_init_finish();

/* voice事件 */ 
void jsapi_voice_publish_sessionBegin(char* wk_word, int wk_score);
void jsapi_voice_publish_sessionEnd();
void jsapi_voice_publish_shortcutCMD(cJSON *object, const char * resultCMD);
void jsapi_voice_publish_nlpBegin();
void jsapi_voice_publish_nlpEnd(const char * data);

/* power事件 */
void jsapi_power_publish_batteryChange(int battery);
void jsapi_power_publish_charge(bool isCharging);
void jsapi_power_publish_batteryLow(int battery);
void jsapi_power_publish_batteryEmergency(int battery);

/* wifi事件 */ 
// void publish_wifi_jsapi();

/* audio事件 */
void jsapi_audio_publish_error();
void jsapi_audio_publish_start();
void jsapi_audio_publish_pause();
void jsapi_audio_publish_resume();
void jsapi_audio_publish_stop();

void jsapi_audio_publish_volChange();
void jsapi_audio_publish_mute();
void jsapi_audio_publish_finish();

/* gateway事件 */
#if defined(CONFIG_BT_MESH) && CONFIG_BT_MESH
#include <gateway.h>
void jsapi_gateway_publish_provShowDev();
void jsapi_gateway_publish_addNodeRst(const char* result, void* gw_evt_param);
void jsapi_gateway_publish_delNodeRst(const char* result);
void jsapi_gateway_publish_nodeStatusRpt(void* subdevstatus);
void add_scan_meshdev(gw_evt_discovered_info_t* g_gw_msg);
#endif

#ifdef __cplusplus
}
#endif

#endif  //JSAPIPUBLISH_H
