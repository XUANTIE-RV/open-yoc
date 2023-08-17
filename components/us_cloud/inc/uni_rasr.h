/**
 * @file uni_rasr.h
 * @brief KWS Engine C/C++ API Header
 * @authors jiangchuang@unisound.com
 * @copyright 2023-2023 Unisound AI Technology Co., Ltd. All rights reserved.
 */

#ifndef SDK_VUI_BASIC_INC_UNI_RASR_H_
#define SDK_VUI_BASIC_INC_UNI_RASR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* RasrHandle;
typedef int (*RasrRstCb)(void *usr_ptr, char *rst, int rst_len);

RasrHandle RasrCreate(const char *config_file, RasrRstCb rasr_rst_cb, void *rasr_rst_cb_usr_ptr);
void       RasrDestroy(RasrHandle handle);
int     RasrStart();
int     RasrStop();
int     RasrAcquist(char *buf, int size);

#ifdef __cplusplus
}
#endif
#endif
