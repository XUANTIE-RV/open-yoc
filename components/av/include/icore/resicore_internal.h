/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __RESICORE_INTERNAL_H__
#define __RESICORE_INTERNAL_H__

#include "icore/icore_msg.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define RESICORE_AP_IDX          (0)
#define RESICORE_CP_IDX          (1)
#else
#define RESICORE_AP_IDX          (1)
#define RESICORE_CP_IDX          (2)
#endif

#define IPC_CMD_RESICORE         (58)
#define RESICORE_IPC_SERIVCE_ID  (0x13)

enum {
    ICORE_CMD_RES_INVALID,
    ICORE_CMD_RES_NEW,
    ICORE_CMD_RES_GET_OSAMPLES,
    ICORE_CMD_RES_CONVERT,
    ICORE_CMD_RES_FREE,
};

typedef struct {
    uint32_t          irate;
    uint32_t          orate;
    uint8_t           bits;
    uint8_t           channels;
    void              *r;      // resp: resx_t
} resicore_new_t;

typedef struct {
    uint32_t          irate;
    uint32_t          orate;
    size_t            nb_isamples;
} resicore_getosa_t;

typedef struct {
    void              *r;
    void              *in;
    void              *out;
    size_t            nb_isamples;
    size_t            nb_osamples;
} resicore_convert_t;

typedef struct {
    void      *r;
} resicore_free_t;

__END_DECLS__

#endif /* __RESICORE_INTERNAL_H__ */

