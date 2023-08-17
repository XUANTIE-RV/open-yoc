/**
 * Copyright (C) 2023 Unisound.inc, All rights reserved.
 *
 * @file uni_nlu.h
 * @brief 本地nlu头文件
 * @version 0.1
 */

#ifndef SDK_VUI_VUI_SERVICE_INC_UNI_NLU_H_
#define SDK_VUI_VUI_SERVICE_INC_UNI_NLU_H_

#ifdef __cplusplus
extern "C" {
#endif

char* NluParseLasr(const char *rasr_result, float *score);
char* NluParseWkuRst(const char *asr_json, float *score, int *index);

#ifdef __cplusplus
}
#endif
#endif  //  SDK_VUI_VUI_SERVICE_INC_UNI_NLU_H_
