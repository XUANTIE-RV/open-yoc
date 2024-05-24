/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/tfft/fft.h"
#include "av/tfft/fft_all.h"

/**
 * @brief  regist fft for speex
 * @return 0/-1
 */
int fftx_register_speex()
{
    extern struct fftx_ops fftx_ops_speex;
    return fftx_ops_register(&fftx_ops_speex);
}

/**
 * @brief  regist fft for ipc
 * @return 0/-1
 */
int fftx_register_ipc()
{
    extern struct fftx_ops fftx_ops_ipc;
    return fftx_ops_register(&fftx_ops_ipc);
}


