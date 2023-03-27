#ifndef _RW_JSON_H
#define _RW_JSON_H

#include "cvi_type.h"
#include "cvi_bin.h"
#include "cvi_common.h"


typedef CVI_S32 (*pfn_cvi_bin_getbinsize)(CVI_U32 *size);
typedef CVI_S32 (*pfn_cvi_bin_getparamfrombin)(CVI_U8 *addr, CVI_U32 size);
typedef CVI_S32 (*pfn_cvi_bin_setparamtobin)(FILE *fp);
typedef CVI_S32(*pfn_cvi_bin_setparamtobuf)(unsigned char *buffer);
#if CONFIG_PQBIN_USE_JSON
typedef CVI_S32 (*pfn_cvi_json_getparamfrombuf)(const char *buffer, enum CVI_BIN_SECTION_ID id);
typedef CVI_S32 (*pfn_cvi_json_setparamtobuf)(signed char **buffer, enum CVI_BIN_SECTION_ID id, CVI_S32 *len);

CVI_S32 CVI_JSON_LoadParamFromBuffer(enum CVI_BIN_SECTION_ID id, CVI_U8 *buf,
				     pfn_cvi_json_getparamfrombuf pFuncgetParam);
CVI_S32 CVI_JSON_SaveParamToBuffer(unsigned char *buffer, enum CVI_BIN_SECTION_ID id, CVI_JSON_INFO *pJsonInfo,
				   pfn_cvi_json_setparamtobuf pFuncsetParam, CVI_U32 u32FreeSpaceSize);
CVI_S32 CVI_JSON_SaveParamToFile(FILE *fp, enum CVI_BIN_SECTION_ID id, CVI_JSON_INFO *pJsonInfo,
				 pfn_cvi_json_setparamtobuf pFuncsetParam);
#endif
#endif
