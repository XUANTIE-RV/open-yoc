/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __APP_AT_CMD_H_
#define __APP_AT_CMD_H_


#ifdef __cplusplus
extern "C" {
#endif

/* 
 * @brief at 组件初始化
 */
int app_at_cmd_init();

/* 
 * @brief at 组件输出pcm数据
 */
void app_at_pcm_data_out(void *data, int len);

/* 
 * @brief at 组件输出对话开始信息
 */
void app_at_session_start(char* wakeup_word);

/* 
 * @brief at 组件输出对话结束信息
 */
void app_at_session_stop();

#ifdef __cplusplus
}
#endif

#endif