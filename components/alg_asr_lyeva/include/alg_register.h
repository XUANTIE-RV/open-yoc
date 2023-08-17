/**
 * @file register.h
 * @author Liang Shui (Gao Hao)
 * @brief 获得相关算法句柄
 * @date 2023-05-16
 * 
 * Copyright (C) 2023 Alibaba Group Holding Limited
 * 
 */


#ifndef _AIE_REG_H
#define _AIE_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define aie_register(name) aie_##name##_register()

typedef void vad_handle;
typedef void agc_handle;
typedef void asr_wenet_handle;
typedef void kws_handle;


/**
 * 功能：注册模型，通过宏函数指定
 * 返回值：模型句柄
*/
vad_handle *aie_vad_register(void);

vad_handle *aie_nnvad_register(void);

agc_handle *aie_agc_register(void);

asr_wenet_handle *aie_asr_wenet_register(void);

#ifdef OPEN_MKWS
kws_handle aie_mkws_fsmn_register(void);
#endif // OPEN_MKWS

kws_handle *aie_skws_fsmn_register(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _AIE_REG_H

