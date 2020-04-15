/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFTICORE_INTERNAL_H__
#define __FFTICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "tfft/fft_cls.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define FFTICORE_AP_IDX          (0)
#define FFTICORE_CP_IDX          (1)
#else
#define FFTICORE_AP_IDX          (1)
#define FFTICORE_CP_IDX          (2)
#endif

#define IPC_CMD_FFTICORE         (60)
#define FFTICORE_IPC_SERIVCE_ID  (0x15)

enum {
    ICORE_CMD_FFT_INVALID,
    ICORE_CMD_FFT_NEW,
    ICORE_CMD_FFT_FORWARD,
    ICORE_CMD_FFT_BACKWARD,
    ICORE_CMD_FFT_FREE,
};

typedef struct {
    size_t            size;      // N
    void              *fft;      // resp: fftx_t
} ffticore_new_t;

typedef struct {
    void              *fft;
    int16_t           *in;
    int16_t           *out;
} ffticore_forward_t;

typedef struct {
    void              *fft;
    int16_t           *in;
    int16_t           *out;
} ffticore_backward_t;

typedef struct {
    void      *fft;
} ffticore_free_t;

__END_DECLS__

#endif /* __FFTICORE_INTERNAL_H__ */

