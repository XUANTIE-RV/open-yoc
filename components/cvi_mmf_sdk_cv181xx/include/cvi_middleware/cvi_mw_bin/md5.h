#ifndef _MD5_H_
#define _MD5_H_

#include "cvi_type.h"

#define MD5_STRING_LEN (32 + 1)

void calcute_md5_value(CVI_U8 *initial_buf, CVI_S32 initial_len, CVI_U8 *md5_value);

#endif
