#ifndef _APP_AUI_CLOUD_H_
#define _APP_AUI_CLOUD_H_

void app_text_cmd_init(void);
int app_aui_nlp_init(void);
int app_aui_wwv_init(void);

int app_aui_cloud_start(int do_wwv);
int app_aui_cloud_push_audio(void *data, size_t size);
int app_aui_push_wwv_data(void *data, size_t len);
int app_aui_get_wwv_result(unsigned int timeout);
int app_aui_cloud_stop(int force_stop);
int app_aui_cloud_start_tts();
int app_aui_cloud_tts_wait_finish();
int app_aui_cloud_tts_run(const char *text, int wait_last);
int app_aui_cloud_stop_tts();
int app_aui_cloud_push_text(char *text);

#endif
