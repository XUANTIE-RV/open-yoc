/**
 * @file cloud_us_nlp.h
 * @brief KWS Engine C/C++ API Header
 * @authors jiangchuang@unisound.com
 * @copyright 2023-2023 Unisound AI Technology Co., Ltd. All rights reserved.
 */
#ifndef _CLOUD_US_NLP_H_
#define _CLOUD_US_NLP_H_

#ifdef __cplusplus 
extern "C"
{
#endif
#include <yoc/aui_cloud.h>

void aui_us_nlp_register(aui_t *aui, aui_nlp_cb_t cb, void *priv);

#ifdef __cplusplus 
}
#endif

#endif
