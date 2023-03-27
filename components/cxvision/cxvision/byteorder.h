/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef CXVISION_COMMON_BYTEORDER_H_
#define CXVISION_COMMON_BYTEORDER_H_

#ifdef __YOC__
#include <machine/endian.h>
#else
#include <endian.h>
#endif

#define __CX_BSWAP_32(x) \
  ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | \
  (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))

#if BYTE_ORDER == LITTLE_ENDIAN
#define CX_HTOLE32(x) (x)
#define CX_LE32TOH(x) (x)
#else
#define CX_HTOLE32(x) __CX_BSWAP_32(x)
#define CX_LE32TOH(x) __CX_BSWAP_32(x)
#endif

#endif  // CXVISION_COMMON_BYTEORDER_H_
