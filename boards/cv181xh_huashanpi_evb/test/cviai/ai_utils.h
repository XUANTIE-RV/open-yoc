#ifndef __AI_UTILS_H__
#define __AI_UTILS_H__

#include <aos/cli.h>
#include <cvi_sys.h>
#if (CONFIG_SUPPORT_TPU == 1)

#define AI_TAG "AI_APP"

// #define ENABLE_AI_APP_VO

#define GOTO_IF_FAILED(func, result, label)                                         \
  do {                                                                              \
    result = (func);                                                                \
    if (result != CVI_SUCCESS) {                                                    \
      aos_cli_printf("failed! ret=%#x, at %s:%d\n", result, __FILE__, __LINE__);    \
      goto label;                                                                   \
    }                                                                               \
  } while (0)

#define RETURN_IF_FAILED(func, result)                                              \
  do {                                                                              \
    result = (func);                                                                \
    if (result != CVI_SUCCESS) {                                                    \
      aos_cli_printf("failed! ret=%#x, at %s:%d\n", result, __FILE__, __LINE__);    \
      return CVI_FAILURE;                                                           \
    }                                                                               \
  } while (0)


typedef struct {
  VPSS_GRP vpssGrp;
  CVI_U32 grpHeight;
  CVI_U32 grpWidth;
  PIXEL_FORMAT_E groupFormat;

#ifdef ENABLE_AI_APP_VO
  VPSS_CHN vpssChnVideoOutput;
  CVI_U32 voWidth;
  CVI_U32 voHeight;
  PIXEL_FORMAT_E voFormat;
#endif

  VPSS_CHN vpssChnAI;
  CVI_U32 aiWidth;
  CVI_U32 aiHeight;
  PIXEL_FORMAT_E aiFormat;

  VI_PIPE viPipe;
} VPSSConfigs;

typedef struct {
  VPSSConfigs vpssConfigs;

  struct {
    CVI_U32 DevNum;
    VI_PIPE ViPipe;
  } ViPipe;

} VideoSystemContext;

#if 1
CVI_S32 InitVideoSystem(VideoSystemContext *vsCtx, SIZE_S *aiInputSize,
                        PIXEL_FORMAT_E aiInputFormat, int voType);

int dump_vpss_frame_to_file(const char *sz_file, VIDEO_FRAME_INFO_S *frame);
int dump_buffer_to_file(const char *sz_file,const uint8_t *p_buf,uint32_t size);
#endif

#endif
#endif /* __AI_UTILS_H__ */
