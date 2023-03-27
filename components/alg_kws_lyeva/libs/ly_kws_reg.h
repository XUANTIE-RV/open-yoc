#ifndef _LY_MODEL_H
#define _LY_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define aie_register(name) aie_##name##_register()

typedef void* kws_handle_t;

/**
 * 功能：注册模型，通过宏函数指定
 * 返回值：模型句柄
*/
kws_handle_t aie_mkws_fsmn_register(void);
kws_handle_t aie_skws_fsmn_register(void);

#ifdef __cplusplus
}
#endif

#endif