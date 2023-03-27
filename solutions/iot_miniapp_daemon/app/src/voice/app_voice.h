/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _APP_VOICE_H_
#define _APP_VOICE_H_

int app_mic_init(void);

/* 
 * @brief 确认是否在唤醒状态
 */
int app_mic_is_wakeup(void);

/* 
 * @brief 获取是否需要进行唤醒二次确认
 *        用户需要实现该函数，例如在夜间时进行二次确认，降低误唤醒
 */
int app_wwv_get_confirm();

/* 
 * @brief 设置是否进行二次唤醒确认 
 *        命令行模拟是否确认，实际应用无需使用该函数 
 */
void app_wwv_set_confirm(int confirm);

/* 
 * @brief 获取唤醒词PCM数据
 * @param data 返回唤醒词数据指针
 * @param size 返回唤醒词数据字节数
 * @return void
 */
void app_wwv_get_data(uint8_t **data, int *size);

/* 
 * @brief 使能算法线性AEC数据输出
 * @return void
 */
void app_linear_aec_init();

#endif
